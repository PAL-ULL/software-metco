/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 ************************************************************************************/

#include "ZDT1.h"
#include <vector>
#include <time.h>
#include <sys/time.h>

// Initialization. 
//   There is one optional parameters. It specifies the minimum time (usec) that the 
//   evaluation must require (0 if not specified)
//   
bool ZDT1::init (const vector<string> &params){
	switch(params.size()){
		case 0: 
			delay = 0;
			break;
		case 1:
			delay = atoi(params[0].c_str());
		default:
			cerr << "Error: ZDT1 incorrect parameters" << endl;
			return false;
	}
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	return true;
}

// Evaluacion
void ZDT1::evaluate (void){
	struct timeval ini;
	if (delay != 0)
		gettimeofday(&ini, NULL);
	
	setObj(0, getVar(0));//f1 = x1
	double sum = 0;
	for (int i = 1; i < NPARAM; i++)
		sum += getVar(i);
	double g = 1 + (9.0/(NPARAM - 1)) * sum;
	setObj(1, g*(1 - sqrt(getVar(0)/g)));//f2

	if (delay != 0){
		struct timeval fin;
		double at;
		do {
			gettimeofday(&fin, NULL);
			at = (double) (fin.tv_sec) * 1.0e6 + (double) (fin.tv_usec) - (double) (ini.tv_sec) * 1.0e6 - (double) (ini.tv_usec);
		} while(at < delay);
	}
}

// Clonacion
Individual* ZDT1::clone (void) const{
	return new ZDT1();
}

long ZDT1::delay;
