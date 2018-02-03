#include "CEC13_LSGO_F14.h"
#include <string>

using namespace std;

unsigned ID;
int64_t M;
int64_t A;
int64_t m_seed;
int64_t MASK;
double m_nextGaussian;
bool  m_havenextGaussian;
bool setOvectorToZero;

int s_size;
int overlap;
double *Ovector = NULL;
double **OvectorVec = NULL;
int*    Pvector = NULL;
double* RotMatrix = NULL;
double** MultiRotMatrix1D = NULL;

double anotherz[1000];
double* anotherz1 = NULL;
double* anotherz2 = NULL;

vector<bool> interArray;

int minX;
int maxX;
int dimension;
int nonSeparableGroupSize;
int64_t functionInitRandomSeed;
struct IndexMap *indexMap;
unsigned arrSize;

double** r25 = NULL;
double** r50 = NULL;
double** r100 = NULL;
int* s = NULL;
double* w = NULL;

bool CEC13_LSGO_F14::init (const vector<string> &params){

  /*if (params.size() != 1) {
    cerr << "Error in init: Number of variables must be indicated" << endl;
    exit(-1);
  }*/
	
  // Dimension debe valer 905 debido al overlapping, aunque quizas
  // deba valer dimension = dimension - overlap * (s_size - 1)
  setNumberOfVar(NUMVAR);
  dimension = NUMVAR;
	setNumberOfObj(1);
	Benchmarks();
	minX = -100;
	maxX = 100;
	ID = 14;
	s_size = 20;
	overlap = 5;
	return true;
}


void CEC13_LSGO_F14::evaluate() {
  int i;
  double result=0.0;
  
  if(OvectorVec == NULL)
    {   
      s = readS(s_size);
      OvectorVec = readOvectorVec();
      Pvector = readPermVector();
      r25 = readR(25);
      r50 = readR(50);
      r100 = readR(100);
      w = readW(s_size);
    } 

  // Esto hace falta para pasarle todo el vector de variables a rotateVectorConflict
  double aux[dimension];
  for (int i = 0; i < dimension; i++) {
    aux[i] = getVar(i);
  }

  // s_size non-separable part with rotation
  int c = 0;
  for (i = 0; i < s_size; i++)
    {   
      anotherz1 = rotateVectorConflict(i, c, aux); 
      result += w[i] * schwefel(anotherz1, s[i]);
      delete []anotherz1;
    }
  
  setObj(0, result);
}

Individual* CEC13_LSGO_F14::clone (void) const{
	return new CEC13_LSGO_F14();
}

void Benchmarks(){
  nonSeparableGroupSize = 50;
  MASK = ((L(1)) << (L(48))) - (L(1));
  m_havenextGaussian = false;
  if (dimension<nonSeparableGroupSize){
    cerr<<"ERROR: In configuration file, dimension is smaller than the non-separable part"<<endl;
    exit(-1);
  }
  setOvectorToZero = false;
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

int next(int bits) {
  int64_t s;
  int64_t result;
  m_seed = s = (((m_seed * M) + A) & MASK);
  result = (s >> (L(48 - bits)));
  return((int)result);
}

int nextInt(int n) {
  int bits, val;

  if ((n & (-n)) == n) {
    return((int) ((n * L(next(31))) >> L(31)));
  }

  do {
    bits = next(31);
    val  = bits % n;
  } while (bits - val + (n - 1) < 0);

  return(val);
}

double nextDouble(){
  return ((((L(next(26))) <<(L(27))) + (L(next(27)))) / (double) ((L(1)) << (L(53))));
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
  d      = new double[dim];

  for (i = (dim - 1); i >= 0; i--) {
    if (setOvectorToZero == true){
      d[i] = 0;
    }else{
      do {
        double tempGauss = nextGaussian();
        s = (middle + (tempGauss * hw));
      } while ((s < min) || (s > max));
      d[i] = s;
    }
  }
  return(d);
}

double* readOvector()
{
  double* d = new double[1000];
  stringstream ss;
  ss<< "/home/edusegre/CEC13_LSGO/cdatafiles/" << "F" << ID << "-xopt.txt";
  ifstream file (ss.str().c_str());
  string value;
  string line;
  int c=0;
  
  if (file.is_open())
    {
      stringstream iss;
      while ( getline(file, line) )
        {
          iss<<line;
          while (getline(iss, value, ','))
            {
              d[c++] = stod(value);
            }
          iss.clear();
        }
      file.close();
    }
  else
    {
      cout<<"Cannot open datafiles"<<endl;
    }
  return d;
}

double** readOvectorVec()
{
  double** d = (double**) malloc(s_size*sizeof(double*));
  stringstream ss;
  ss<< "/home/edusegre/CEC13_LSGO/cdatafiles/" << "F" << ID << "-xopt.txt";
  ifstream file (ss.str());
  string value;
  string line;
  int c = 0;                      // index over 1 to dim
  int i = -1;                      // index over 1 to s_size
  int up = 0;                   // current upper bound for one group
  
  if (file.is_open())
    {
      stringstream iss;
      while ( getline(file, line) )
        {
          if (c==up)             // out (start) of one group
            {
              i++;
              d[i] =  (double*) malloc(s[i]*sizeof(double));
              up += s[i];
            }
          iss<<line;
          while (getline(iss, value, ','))
            {
              d[i][c-(up-s[i])] = stod(value);
              c++;
            }
          iss.clear();
        }
      file.close();
    }
  else
    {
      cout<<"Cannot open datafiles"<<endl;
    }
  return d;  
}


void transform_osz(double* z, int dim)
{
  // apply osz transformation to z
  for (int i = 0; i < dim; ++i)
    {
      z[i] = sign(z[i]) * exp( hat(z[i]) + 0.049 * ( sin( c1(z[i]) * hat(z[i]) ) + sin( c2(z[i])* hat(z[i]) )  ) ) ;
    }
}

void transform_asy(double* z, double beta, int dim)
{
  for (int i = 0; i < dim; ++i)
    {
      if (z[i]>0)
        {
          z[i] = pow(z[i], 1 + beta * i/((double) (dim-1)) * sqrt(z[i]) );
        }
    }
}

void Lambda(double* z, double alpha, int dim)
{
  for (int i = 0; i < dim; ++i)
    {
      z[i] = z[i] * pow(alpha, 0.5 * i/((double) (dim-1)) );
    }
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

int* readPermVector(){
  int* d;

  d = new int[dimension];

  stringstream ss;
  ss<< "/home/edusegre/CEC13_LSGO/cdatafiles/" << "F" << ID << "-p.txt";
  ifstream file (ss.str());
  int c=0;
  string value;

  if (file.is_open())
    {
      while (getline(file,value,','))
        {
          d[c++] = stod(value) - 1;
        }
    }
  return(d);
}


//Create a random rotation matrix
double** createRotMatrix(int dim){
  double** m;
  int      i, j, k;
  double   dp, t;
  m = new double*[dim];

  for(i = 0; i < dim; i++) {
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

  a = new double*[num];

  for (i = 0; i < num; i++) {
    a[i] = createRotMatrix1D(dim);
  }

  return (a);
}

double** readR(int sub_dim)
{
  double** m;

  m = new double*[sub_dim];
  for (int i = 0; i< sub_dim; i++)
    {
      m[i] = new double[sub_dim];
    }

  stringstream ss;
  ss<< "/home/edusegre/CEC13_LSGO/cdatafiles/" << "F" << ID << "-R"<<sub_dim<<".txt";

  ifstream file (ss.str());
  string value;
  string line;
  int i=0;
  int j;

  if (file.is_open())
    {
      stringstream iss;
      while ( getline(file, line) )
        {
          j=0;
          iss<<line;
          while (getline(iss, value, ','))
            {
              m[i][j] = stod(value);
              j++;
            }
          iss.clear();
          i++;
        }
      file.close();
    }
  else
    {
      cout<<"Cannot open datafiles"<<endl;
    }
  return m;
}

int* readS(int num)
{
  s = new int[num];

  stringstream ss;
  ss<< "/home/edusegre/CEC13_LSGO/cdatafiles/" << "F" << ID << "-s.txt";
  ifstream file (ss.str());
  int c=0;
  string value;
  if (file.is_open())
    {
      while (getline(file,value))
        {
          s[c++] = stod(value);
        }
    }
  return s;
}

double* readW(int num)
{
  w = new double[num];

  stringstream ss;
  ss<< "/home/edusegre/CEC13_LSGO/cdatafiles/" << "F" << ID << "-w.txt";
  ifstream file (ss.str());
  int c=0;
  string value;
  if (file.is_open())
    {
      while (getline(file,value))
        {
          w[c++] = stod(value);
        }
    }

  return w;
}

double* rotateVector(int i, int &c)
{
  double* z = new double[s[i]];
  // copy values into the new vector
  for (int j = c; j < c+s[i]; ++j)
    {
      z[j-c] = anotherz[Pvector[j]];
    }

  if (s[i]==25)
    {
      anotherz1 = multiply( z, r25, s[i]);
    }
  else if (s[i] == 50)
    {    
      anotherz1 = multiply( z, r50, s[i]);
    }
  else if (s[i] == 100) 
    {
      anotherz1 = multiply( z, r100, s[i]);
    }
  else
    {
      cout<< "size of rotation matrix out of range" <<endl;
    }
  delete []z;
  c = c + s[i];
  return anotherz1;
}

double* rotateVectorConform(int i, int &c)
{
  double* z = new double[s[i]];
  // copy values into the new vector
  for (int j = c - i*overlap; j < c +s[i] - i*overlap; ++j)
    {
      z[j-(c - i*overlap)] = anotherz[Pvector[j]];
    }
  if (s[i]==25)
    {
      anotherz1 = multiply( z, r25, s[i]);
    }
  else if (s[i] == 50)
    {    
      anotherz1 = multiply( z, r50, s[i]);
    }
  else if (s[i] == 100) 
    {
      anotherz1 = multiply( z, r100, s[i]);
    }
  else
    {
      cout<< "size of rotation matrix out of range" <<endl;
    }
  delete []z;
  c = c + s[i];
  return anotherz1;
}

double* rotateVectorConflict(int i, int &c, double* x)
{
  double* z = new double[s[i]];
  
  // copy values into the new vector
  for (int j = c - i*overlap; j < c +s[i] - i*overlap; ++j)
    {
      // cout<<"j-c "<<j-c<<" p "<<Pvector[j]<<endl;
      z[j-(c - i*overlap)] = x[Pvector[j]] - OvectorVec[i][j-(c - i*overlap)];  
    }

  // cout<<"copy done"<<endl;
  if (s[i]==25)
    {
      anotherz1 = multiply( z, r25, s[i]);
    }
  else if (s[i] == 50)
    {    
      anotherz1 = multiply( z, r50, s[i]);
    }
  else if (s[i] == 100) 
    {
      anotherz1 = multiply( z, r100, s[i]);
    }
  else
    {
      cout<< "size of rotation matrix out of range" <<endl;
    }
  delete []z;
  c = c + s[i];
  return anotherz1;
}


double elliptic(double*x,int dim) {
  double result = 0.0;
  int    i;
  
  transform_osz(x, dim);

  // for(i = dim - 1; i >= 0; i--) {
  for(i=0; i<dim; i++)
    {
      // printf("%f\n", pow(1.0e6,  i/((double)(dim - 1)) ));
      result += pow(1.0e6,  i/((double)(dim - 1)) ) * x[i] * x[i];
    }
  
  return(result);
}


unsigned getID(){
  return ID;
}

// rastrigin function for F1~F8
double rastrigin(double*x,int dim){
  double sum = 0;
  int    i;
  
  // T_{osz}
  transform_osz(x, dim);
  
  // T_{asy}^{0.2}
  transform_asy(x, 0.2, dim);

  // lambda
  Lambda(x, 10, dim);

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

  // T_{osz}
  transform_osz(x,dim);
  
  // T_{asy}^{0.2}
  transform_asy(x, 0.2, dim);

  // lambda
  Lambda(x, 10, dim);

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

double* multiply(double*vector, double**matrix,int dim){
  int    i,j;
  //double*result = (double*)malloc(sizeof(double) * dim);
  double*result = new double[dim];

  for(i = dim - 1; i >= 0; i--) {
    result[i] = 0;

    for(j = dim - 1; j >= 0; j--) {
      result[i] += vector[j] * matrix[i][j];
    }
  }

  return(result);
}


// Rotated Rastrigin Function for F1~F8
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
  
  // T_{osz}
  transform_osz(x,dim);
  
  // T_{asy}^{0.2}
  transform_asy(x, 0.2, dim);

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

int sign(double x)
{
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}

double hat(double x)
{
  if (x==0)
    {
      return 0;
    }
  else
    {
      return log(abs(x));
    }
}

double c1(double x)
{
  if (x>0)
    {
      return 10;
    }
  else
    {
      return 5.5;
    }
}

double c2(double x)
{
  if (x>0)
    {
      return 7.9;
    }
  else
    {
      return 3.1;
    }
}


