/***********************************************************************************
 * FECHA
 *    Mayo 2009
 ************************************************************************************/

#include "R1.h"
#include <vector>
#include <time.h>
#include <sys/time.h>

// Initialization. 
bool R1::init (const vector<string> &params){
	if (params.size() != 0){
	  cerr << "Error R1 called with parameters" << endl;
		return false;
	}
	
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	return true;
}

// Evaluacion
void R1::evaluate (void){
	setObj(0, getVar(0));//f1 = y1
	double sum = 0;
	for (int i = 1; i < NPARAM; i++)
		sum += getVar(i)*getVar(i) - 10*cos(4*M_PI*getVar(i));;
	double g = 1 + (10*(NPARAM - 1)) + sum;
	setObj(1, g*(exp(-getVar(0)/g)));//f2
}

// Clonacion
Individual* R1::clone (void) const{
	return new R1();
}

