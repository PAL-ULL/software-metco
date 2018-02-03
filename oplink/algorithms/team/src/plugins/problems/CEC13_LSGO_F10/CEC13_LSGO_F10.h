#ifndef _CEC13_LSGO_F10_H
#define _CEC13_LSGO_F10_H


#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>
using namespace std;

#define PI (3.141592653589793238462643383279)
#define E  (2.718281828459045235360287471352)
#define L(i) ((int64_t)i)
#define D(i) ((double)i)
#define NUMVAR (1000)

struct IndexMap{
  unsigned arrIndex1;
  unsigned arrIndex2;
};

int next(int bits);
int nextInt(int n);
double nextDouble();
double nextGaussian();
double* createShiftVector(int dim, double min,double max);
int* createPermVector(int dim);
double** createRotMatrix(int dim);
double* createRotMatrix1D(int dim);
double** createMultiRotateMatrix1D(int dim, int num);


// Basic mathematical functions' declaration
double* multiply(double*vector, double*matrix,int dim);
double* multiply(double*vector, double**matrix,int dim);
double elliptic(double*x,int dim);
double rastrigin(double*x,int dim);
double rastrigin(double *x, int dim, int k); 
double ackley(double*x,int dim);
double ackley(double*x,int dim, int k);
double rot_rastrigin(double*x,int dim);
double rot_rastrigin(double *x,int dim,int k);
double rot_ackley(double*x,int dim);
double rot_ackley(double*x,int dim,int k);
double schwefel(double*x,int dim);
double schwefel(double*x,int dim, int k);
double sphere(double*x,int dim);
double sphere(double*x,int dim, int k);
double rosenbrock(double*x,int dim);
double rosenbrock(double*x,int dim, int k);
unsigned convertMatrixToArrayIndex ( unsigned i, unsigned j );
void createIndexMapping (  ); 
  /* void extractElemByPerm(); */
double* rotateVector(int i, int &c);
double* rotateVectorConform(int i, int &c);
double* rotateVectorConflict(int i, int &c, double* x);

  

void Benchmarks();
	
int getMinX();
int getMaxX();
unsigned getID();

void setMinX(int);
void setMaxX(int);
void setSeed(int64_t);
void setDimension(int);
void setNonSeparableGroupSize(int);
vector<bool> getInterArray (  );
void ArrToMat ( unsigned I1, unsigned I2, unsigned &matIndex );
void MatToArr ( unsigned &I1, unsigned &I2, unsigned matIndex );
        
/* for CEC2013SS */
double* readOvector();
double** readOvectorVec();
int* readPermVector();
double** readR(int sub_dim);
int* readS(int num);
double* readW(int num);

void transform_osz(double* z, int dim);
void transform_asy(double* z, double beta, int dim);
void Lambda(double* z, double alpha, int dim);
int sign(double x);
double hat(double x);
double c1(double x);
double c2(double x);

#include "Individual.h"

class CEC13_LSGO_F10:public Individual{
public:

	// Initialization method
	bool init (const vector<string> &params);

	// Evaluation
	void evaluate (void);

	// Ranges
	double inline getMaximum(const int i) const { return 32; }
	double inline getMinimum(const int i) const { return -32; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

	// Clone
	Individual* clone (void) const;


	double compute(double* x) ;
};
#endif
