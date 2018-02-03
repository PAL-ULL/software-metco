/***********************************************************************************
 * FECHA
 *    Mayo 2009
 ************************************************************************************/

#include "R2.h"
#include <vector>
#include <time.h>
#include <sys/time.h>

// Initialization. 
bool R2::init (const vector<string> &params){
	if (params.size() != 0){
	  cerr << "Error R2 called with parameters" << endl;
		return false;
	}
	
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	return true;
}

// Evaluacion
void R2::evaluate (void){
	setObj(0, getVar(0));//f1 = y1
	double sum = 0;
	for (int i = 1; i < NPARAM; i++)
		sum += getVar(i)*getVar(i) - 10*cos(M_PI*getVar(i));;
	double g = 1 + (10*(NPARAM - 1)) + sum;
	double h = 1.0 + exp(-getVar(0)/g) + ((getVar(0)+1.0)/g)*(sin(5*M_PI*getVar(0)));
	setObj(1, g*h);//f2
}

// Clonacion
Individual* R2::clone (void) const{
	return new R2();
}

long R2::delay;
