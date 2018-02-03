#include "CEC10_LSGO_F4.h"
#include <string>

using namespace std;

// running time setting for benchmarks
unsigned ID;
int minX;
int maxX;
int dimension;
int nonSeparableGroupSize;
int64_t functionInitRandomSeed;
struct IndexMap *indexMap = NULL;
unsigned arrSize;
vector<bool> interArray;

int64_t M;
int64_t A;
int64_t m_seed;
int64_t MASK;
double m_nextGaussian;
bool  m_havenextGaussian;
bool setOvectorToZero;

double *Ovector = NULL;
int*    Pvector = NULL;
double* RotMatrix = NULL;
double** MultiRotMatrix1D = NULL;
double *lookup = NULL;
double *lookup2 = NULL;
	
double anotherz[1000];
double anotherz1[50]; 
double anotherz2[1000];



bool CEC10_LSGO_F4::init (const vector<string> &params){

  if (params.size() != 1) {
    cerr << "Error in init: Number of variables must be indicated" << endl;
    exit(-1);
  }
	
  setNumberOfVar(atoi(params[0].c_str()));
	setNumberOfObj(1);


  dimension = atoi(params[0].c_str());
	Benchmarks();
	minX = -100;
	maxX = 100;
	ID = 4;
	m_havenextGaussian=0;
	lookup2 = lookupprepare(nonSeparableGroupSize);
	lookup = lookupprepare(dimension - nonSeparableGroupSize);
	return true;
}


void CEC10_LSGO_F4::evaluate() {
  int    i;
  double result = 0.0;

  if(Ovector == NULL) {
    Ovector   = createShiftVector(dimension,minX,maxX);
		Pvector   = createPermVector(dimension);
    RotMatrix = createRotMatrix1D(nonSeparableGroupSize);
  }

  for(i = 0; i < dimension; i++) {
    anotherz[i] = getVar(i) - Ovector[i];
  }

  for(i = 0; i < nonSeparableGroupSize; i++) {
    anotherz1[i] = anotherz[Pvector[i]];
  }

  for(i = nonSeparableGroupSize; i < dimension; i++) {
    anotherz2[i - nonSeparableGroupSize] = anotherz[Pvector[i]];
  }

  result = rot_elliptic(anotherz1,nonSeparableGroupSize) * 1e6 + elliptic(anotherz2,dimension - nonSeparableGroupSize);
	setObj(0, result);
}

Individual* CEC10_LSGO_F4::clone (void) const{
	return new CEC10_LSGO_F4();
}




void Benchmarks(){
	nonSeparableGroupSize = 50;
	MASK = ((L(1)) << (L(48))) - (L(1));
	m_havenextGaussian = false;

	if (dimension<nonSeparableGroupSize){
		cerr<<"ERROR: In configuration file, dimension is smaller than the non-separable part"<<endl;
		exit(-1);
	}

	// allocate the memory

	// Runtime Parameters setting
	setOvectorToZero = false;

	functionInitRandomSeed = L(0);
	m_seed= functionInitRandomSeed;
	M  = 0x5DEECE66D;
	A  = 0xB;

}

void setMinX(int inVal){
	minX = inVal;	
}
void setMaxX(int inVal){
	maxX = inVal;
}

void setSeed(int64_t inVal){
	functionInitRandomSeed = inVal;
}

void setDimension(int inVal){
	dimension = inVal;
}

void setNonSeparableGroupSize(int inVal){
	nonSeparableGroupSize = inVal;
}

int next_(int bits) {
	int64_t s;
	int64_t result;
	m_seed = s = (((m_seed * M) + A) & MASK);
	result = (s >> (L(48 - bits)));
	return((int)result);
}

int nextInt(int n) {
	int bits, val;

	if ((n & (-n)) == n) {
		return((int) ((n * L(next_(31))) >> L(31)));
	}

	do {
		bits = next_(31);
		val  = bits % n;
	} while (bits - val + (n - 1) < 0);

	return(val);
}

double nextDouble(){
	return ((((L(next_(26))) <<
					(L(27))) + (L(next_(27)))) / (double) ((L(1)) << (L(53))));
}

double nextGaussian(){
	double multiplier, v1, v2, s;

	if (m_havenextGaussian) {
		m_havenextGaussian = false;
		return(m_nextGaussian) ;
	}

	do {
		v1 = ((D(2.0) * nextDouble()) - D(1.0));
		v2 = ((D(2.0) * nextDouble()) - D(1.0));
		s  = ((v1 * v1) + (v2 * v2));
	} while ((s >= D(1.0)) || (s <= D(0.0)));

	multiplier = sqrt(D(-2.0) * log(s) / s);
	m_nextGaussian    = (v2 * multiplier);
	m_havenextGaussian = true;
	return (v1 * multiplier);
}

double* createShiftVector(int dim, double min,double max) {

	double* d;
	double  hw, middle;
	double  s;
	int     i;
	hw     = (D(0.5) * (max - min));
	middle = (min + hw);
	//d      = (double*)malloc(sizeof(double) * dim);
	d      = new double[dim];

//	printf("middle = %1.20E\n",middle);
//	printf("hw = %1.20E\n",hw);

	for (i = (dim - 1); i >= 0; i--) {
		if (setOvectorToZero == true){
			d[i] = 0;
		}else{
			do {
				double tempGauss = nextGaussian();
				s = (middle + (tempGauss * hw));
				//				printf("nextGaussian = %1.20E\n",tempGauss);
				/*
				   s = (middle + (nextGaussian() * hw));
				   */
			} while ((s < min) || (s > max));
			d[i] = s;
//			printf("shift vector = %1.20E\n", d[i]);
		}
	}
	return(d);
}

int* createPermVector(int dim){
	int* d;
	int  i, j, k, t;
	//d = (int*)malloc(sizeof(int) * dim);
	d = new int[dim];

	for (i = (dim - 1); i >= 0; i--) {
		d[i] = i;
	}

	for (i = (dim << 3); i >= 0; i--) {
		j = nextInt(dim);

		do {
			k = nextInt(dim);
		} while (k == j);

		t    = d[j];
		d[j] = d[k];
		d[k] = t;
	}

	return(d);
}

//Create a random rotation matrix
double** createRotMatrix(int dim){
	double** m;
	int      i, j, k;
	double   dp, t;
	//m = (double**)malloc(sizeof(double*) * dim);
	m = new double*[dim];

	for(i = 0; i < dim; i++) {
		//m[i] = (double*)malloc(sizeof(double) * dim);
		m[i] = new double[dim];
	}

loop:
	for (;;) {
		for (i = (dim - 1); i >= 0; i--) {
			for (j = (dim - 1); j >= 0; j--) {
				m[i][j] = nextGaussian();
			}
		}

		// main loop of gram/schmidt
		for (i = (dim - 1); i >= 0; i--) {
			for (j = (dim - 1); j > i; j--) {
				// dot product
				dp = 0;

				for (k = (dim - 1); k >= 0; k--) {
					dp += (m[i][k] * m[j][k]);
				}

				// subtract
				for (k = (dim - 1); k >= 0; k--) {
					m[i][k] -= (dp * m[j][k]);
				}
			}

			// normalize
			dp = 0;

			for (k = (dim - 1); k >= 0; k--) {
				t   = m[i][k];
				dp += (t * t);
			}

			// linear dependency -> restart
			if (dp <= 0) {
				goto loop;
			}

			dp = (1 / sqrt(dp));

			for (k = (dim - 1); k >= 0; k--) {
				m[i][k] *= dp;
			}
		}

		return(m) ;
	}
}

/**
 * Create a random rotation matrix
 */
double* createRotMatrix1D(int dim){
	double** a;
	double*  b;
	int      i, j, k;
	a = createRotMatrix(dim);
	//b = (double*)malloc(sizeof(double) * (dim * dim));
	b = new double[dim*dim];
	k = 0;

	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			b[k++] = a[i][j];
		}
	}

	// release the memory that previously allocated for storing a 2D array
	for(i=0;i<dim;i++){
		delete []a[i];
	}
	delete[] a;

	return(b);
}

/*
 * create several 1-D rotation matrix randomly
 */
double** createMultiRotateMatrix1D(int dim, int num){
	double** a;
	int i;

	/*  allocate storage for an array of pointers */
	//a =(double **) malloc(num * sizeof(double *));
	a = new double*[num];

	/* for each pointer, allocate storage for an array of ints */
	for (i = 0; i < num; i++) {
		a[i] = createRotMatrix1D(dim);
	}

	return (a);
}

double* lookupprepare(int dim) {
	double pownum;
	int    i;
	double* lookup;
	i         = (dim - 1);
	pownum    = (1.0 / i);
	//lookup    = (double*)malloc(dim * sizeof(double));
	lookup    = new double[dim];
	lookup[i] = 1.0e6;
	lookup[0] = 1.0;

	for (--i; i > 0; i--) {
		lookup[i] = pow(1.0e6, i * pownum);
	}
	return lookup;
}

/* 
 * Basic Mathematical Functions' Implementation
 */
// elliptic function for F4 ~ F8
double elliptic(double*x,int dim) {
	double result = 0.0;
	int    i;

	for(i = dim - 1; i >= 0; i--) {
		if (dim == nonSeparableGroupSize){
			result += lookup2[i] * x[i] * x[i];
		}else{
			result += lookup[i] * x[i] * x[i];
		}
	}

	return(result);
}

unsigned getID(){
	return ID;
}

// elliptic function for F9 ~ 
double elliptic(double*x, int dim, int k) {
	double result = 0.0;
	int    i;

	for(i=dim/k-1;i>=0;i--)
	{
		result+=lookup2[i]*x[Pvector[dim/k+i]]*x[Pvector[dim/k+i]];
	}

	return(result);
}

// rastrigin function for F4~F8
double rastrigin(double*x,int dim){
	double sum = 0;
	int    i;

	for(i = dim - 1; i >= 0; i--) {
		sum += x[i] * x[i] - 10.0 * cos(2 * PI * x[i]) + 10.0;
	}

	return(sum);
}

// rastrigin function for F9 ~
double rastrigin(double *x, int dim, int k)
{
	double result=0.0;
	int i;
	for(i=dim/k-1;i>=0;i--)
	{
		result+=x[Pvector[dim/k+i]]*x[Pvector[dim/k+i]]-10.0*cos(2*PI*x[Pvector[dim/k+i]])+10.0;
	}
	return(result);
}

// ackley function for single group non-separable 
double ackley(double*x,int dim){
	double sum1 = 0.0;
	double sum2 = 0.0;
	double sum;
	int    i;

	for(i = dim - 1; i >= 0; i--) {
		sum1 += (x[i] * x[i]);
		sum2 += cos(2.0 * PI * x[i]);
	}

	sum = -20.0 * exp(-0.2 * sqrt(sum1 / dim)) - exp(sum2 / dim) + 20.0 + E;
	return(sum);
}

// ackley function for m-group non-separable 
double ackley(double *x, int dim, int k)
{
	double sum1=0.0;
	double sum2=0.0;
	double result=0.0;
	int i;

	for(i=dim/k-1;i>=0;i--)
	{
		sum1+=x[Pvector[dim/k+i]]*x[Pvector[dim/k+i]];
		sum2+=cos(2.0*PI*x[Pvector[dim/k+i]]);
	}

	result=-20.0*exp(-0.2*sqrt(sum1/(dim/k)))-exp(sum2/(dim/k))+20.0+E;

	return(result);
}

double* multiply(double*vector, double*matrix,int dim){
	int    i,j;
	//double*result = (double*)malloc(sizeof(double) * dim);
	double*result = new double[dim];

	for(i = dim - 1; i >= 0; i--) {
		result[i] = 0;

		for(j = dim - 1; j >= 0; j--) {
			result[i] += vector[j] * matrix[dim * j + i];
		}
	}

	return(result);
}

// Rotated Elliptic Function for F4 & F4
double rot_elliptic(double*x,int dim){
	double result = 0.0;
	double *z = multiply(x,RotMatrix,dim);

	result = elliptic(z,dim);

	delete[] z;
	return(result);
}

// Rotated Elliptic Function for F9 & F44
double rot_elliptic(double*x,int dim, int k){
	double result=0.0;

	int i,j;
	for(i=dim-1;i>=0;i--)
	{
		anotherz1[i]=0;
		for(j=dim-1;j>=0;j--)
		{
			anotherz1[i]+=x[Pvector[(k-1)*dim+j]]*RotMatrix[dim*j+i];
		}
	}
	for(i=dim-1;i>=0;i--)
	{
		result+=lookup[i]*anotherz1[i]*anotherz1[i];
	}
	return(result);
}

// Rotated Rastrigin Function for F4~F8
double rot_rastrigin(double*x,int dim){
	double result = 0.0;
	double*z      = multiply(x,RotMatrix,dim);
	result = rastrigin(z,dim);

	delete[] z;
	return(result);
}

// Rotated Rastrigin Function for F9 ~
double rot_rastrigin(double *x,int dim,int k)
{
	double result=0.0;

	int i,j;
	for(i=dim-1;i>=0;i--)
	{
		anotherz1[i]=0;
		for(j=dim-1;j>=0;j--)
		{
			anotherz1[i]+=x[Pvector[(k-1)*dim+j]]*RotMatrix[dim*j+i];
		}
	}

	for(i=dim-1;i>=0;i--)
	{
		result+=anotherz1[i]*anotherz1[i]-10.0*cos(2*PI*anotherz1[i])+10.0;
	}
	return(result);
}

// rotated ackley function for single group non-separable
double rot_ackley(double*x,int dim){
	double result = 0.0;
	double*z      = multiply(x,RotMatrix,dim);
	result = ackley(z,dim);
	delete[] z;
	return(result);
}

// rotated ackley function for m group non-separable
double rot_ackley(double *x,int dim,int k)
{
	double result=0.0;
	double sum1=0.0;
	double sum2=0.0;

	int i,j;
	for(i=dim-1;i>=0;i--)
	{
		anotherz1[i]=0;
		for(j=dim-1;j>=0;j--)
		{
			anotherz1[i]+=x[Pvector[(k-1)*dim+j]]*RotMatrix[dim*j+i];
		}
	}
	for(i=dim-1;i>=0;i--)
	{
		sum1+=anotherz1[i]*anotherz1[i];
		sum2+=cos(2.0*PI*anotherz1[i]);
	}
	result=-20.0*exp(-0.2*sqrt(sum1/dim))-exp(sum2/dim)+20.0+E;
	return(result);
}

// for single group non-separable function
double sphere(double*x,int dim){
	double sum = 0;
	int    i;

	for(i = dim - 1; i >= 0; i--) {
		sum += pow(x[i],2);
	}

	return(sum);
}

// for m groups non-separable function
double sphere(double*x,int dim, int k){
	double result=0.0;
	int i;

//	cout<<"partial sum of sphere function ="<<endl;
	for(i=dim/k-1;i>=0;i--)
	{
		result+=x[Pvector[dim/k+i]]*x[Pvector[dim/k+i]];
//		printf("x = %1.16E\t", x[Pvector[dim/k+i]]);
//		printf("partial sum = %1.16E\t", result);
	}

	return(result);
}

// for single group non-separable function
double schwefel(double*x,int dim){
	int    j;
	double s1 = 0;
	double s2 = 0;

	for (j = 0; j < dim; j++) {
		s1 += x[j];
		s2 += (s1 * s1);
	}

	return(s2);
}

// for m groups non-separable function
double schwefel(double*x,int dim, int k){
	double sum1=0.0;
	double sum2=0.0;
	int i;
	for(i=0;i<dim;i++)
	{
		sum1+=x[Pvector[(k-1)*dim+i]];
		sum2+=sum1*sum1;
	}
	return(sum2);
}

// single group non-separable function
double rosenbrock(double*x,int dim){
	int    j;
	double oz,t;
	double s = 0.0;
	j = dim - 1;

	for (--j; j >= 0; j--) {
		oz = x[j + 1];
		t  = ((x[j] * x[j]) - oz);
		s += (100.0 * t * t);
		t  = (x[j] - 1.0);
		s += (t * t);
	}
	return(s);
}

// m groups non-separable function
double rosenbrock(double*x,int dim, int k){
	int j;
	double oz,t;
	double result=0.0;
	j=dim-1;
	for (--j; j >= 0; j--) {
		oz = x[Pvector[(k-1)*dim+j+1]];
		t = ((x[Pvector[(k-1)*dim+j]] * x[Pvector[(k-1)*dim+j]]) - oz);
		result += (100.0 * t * t);
		t = (x[Pvector[(k-1)*dim+j]] - 1.0);
		result += (t * t);
	}
	return(result);
}

int getMinX(){
	return minX;
}
	
int getMaxX(){
	return maxX;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getInterArray
 *  Description:  get the variable interaction information in the representation of one
 *  				dimensional array
 * =====================================================================================
 */
	vector<bool>
getInterArray (  )
{
	return interArray;
}		/* -----  end of function getInterArray  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  convertMatrixToArrayIndex
 *  Description:  
 * =====================================================================================
 */
	unsigned	
convertMatrixToArrayIndex ( unsigned i, unsigned j )
{
	return ( i* (2*dimension-i-3) / 2 + j - 1);
}		/* -----  end of function convertMatrixToArrayIndex  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  createIndexMapping
 *  Description:  
 * =====================================================================================
 */
	void
createIndexMapping (  )
{
	unsigned N = dimension, indexCounter = 0;

	indexMap = new struct IndexMap[arrSize];

	for (unsigned i=0; i<N; i++){
		for (unsigned j=i+1; j<N; j++){
			indexMap[indexCounter].arrIndex1 = i;
			indexMap[indexCounter].arrIndex2 = j;
			indexCounter++;
		}
	}
}		/* -----  end of function CCVIL::createIndexMapping  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ArrToMat
 *  Description:  
 * =====================================================================================
 */
	void
ArrToMat ( unsigned I1, unsigned I2, unsigned &matIndex )
{
	for (unsigned i=0; i<arrSize; i++){
		if (indexMap[i].arrIndex1 == I1 && indexMap[i].arrIndex2 == I2){
			matIndex = i;
			return ;
		}
	}
	
	printf ( "Cannot locate the matrix index from given array indices\n" );
	exit(EXIT_FAILURE);
}		/* -----  end of function ArrToMat  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  MatToArr
 *  Description:  
 * =====================================================================================
 */
	void
MatToArr ( unsigned &I1, unsigned &I2, unsigned matIndex )
{
	I1 = indexMap[matIndex].arrIndex1;
	I2 = indexMap[matIndex].arrIndex2;
}		/* -----  end of function MatToArr  ----- */
