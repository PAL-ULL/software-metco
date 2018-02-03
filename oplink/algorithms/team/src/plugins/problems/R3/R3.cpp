/***********************************************************************************
 * FECHA
 *    Mayo 2009
 ************************************************************************************/

#include "R3.h"
#include <vector>
#include <time.h>
#include <sys/time.h>

// Initialization. 
bool R3::init (const vector<string> &params){
	if (params.size() != 0){
	  cerr << "Error R3 called with parameters" << endl;
		return false;
	}
	
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	return true;
}

// Evaluacion
void R3::evaluate (void){
	double f1 = 1.0 - exp(2.0*getVar(0))*pow(sin(6*M_PI*getVar(0)),6)/9.0;
	setObj(0, f1);//f1
	double sum = 0;
	for (int i = 1; i < NPARAM; i++)
		sum += getVar(i)*getVar(i) - 10*cos(M_PI*getVar(i));
	double g = 1 + (10*(NPARAM - 1)) + sum;
	double h = 1.0 - pow((f1/g),2);
	setObj(1, g*h);//f2
}

// Clonacion
Individual* R3::clone (void) const{
	return new R3();
}

