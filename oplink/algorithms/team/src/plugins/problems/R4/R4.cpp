/***********************************************************************************
 * FECHA
 *    Mayo 2009
 ************************************************************************************/

#include "R4.h"
#include <vector>
#include <time.h>
#include <sys/time.h>

// Initialization. 
bool R4::init (const vector<string> &params){
	if (params.size() != 0){
	  cerr << "Error R4 called with parameters" << endl;
		return false;
	}
	
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	return true;
}

// Evaluacion
void R4::evaluate (void){
	setObj(0, getVar(0));//f1 = y1
	double sum = 0;
	for (int i = 1; i < NPARAM; i++)
		sum += getVar(i)*getVar(i) - 0.25*(getVar(i)*sin(32.0*sqrt(fabs(getVar(i)))));
	double g = 1.0 + 0.015578 *(NPARAM - 1.0) + sum;
	setObj(1, g*exp(-getVar(0)/g));//f2
}

// Clonacion
Individual* R4::clone (void) const{
	return new R4();
}
