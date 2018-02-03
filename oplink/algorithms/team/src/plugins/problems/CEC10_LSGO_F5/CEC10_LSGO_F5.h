#ifndef _CEC10_LSGO_F5_H
#define _CEC10_LSGO_F5_H

#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include	<cmath>
#include	<ctime>
using namespace std;

#define PI (3.141592653589793238462643383279)
#define E  (2.718281828459045235360287471352)
#define L(i) ((int64_t)i)
#define D(i) ((double)i)

struct IndexMap{
	unsigned arrIndex1;
	unsigned arrIndex2;
};

	int next_(int bits);
	int nextInt(int n);
	double nextDouble();
	double nextGaussian();
	double* createShiftVector(int dim, double min,double max);
	int* createPermVector(int dim);
	double** createRotMatrix(int dim);
	double* createRotMatrix1D(int dim);
	double** createMultiRotateMatrix1D(int dim, int num);

	double* lookupprepare(int dim);

	// Basic mathematical functions' declaration
	double* multiply(double*vector, double*matrix,int dim);
	double elliptic(double*x,int dim);
	double elliptic(double*x, int dim, int k);
	double rastrigin(double*x,int dim);
	double rastrigin(double *x, int dim, int k); 
	double ackley(double*x,int dim);
	double ackley(double*x,int dim, int k);
	double rot_elliptic(double*x,int dim);
	double rot_elliptic(double*x,int dim, int k);
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



#include "Individual.h"

class CEC10_LSGO_F5:public Individual{
public:

	// Initialization method
	bool init (const vector<string> &params);

	// Evaluation
	void evaluate (void);

	// Ranges
	double inline getMaximum(const int i) const { return 5; }
	double inline getMinimum(const int i) const { return -5; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

	// Clone
	Individual* clone (void) const;


	double compute(double* x) ;
};
#endif
