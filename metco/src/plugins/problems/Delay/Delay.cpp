/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 ************************************************************************************/

#include "Delay.h"
#include <vector>
#include <sys/time.h>
#include <time.h>

long Delay::delay;

// Inicialización. 
// Fija:
//   - Numero de variables
//   - Numero de objetivos
//   - Direccion de optimizacion de los objetivos
//   - Rangos de las variables (mínimos y máximos)
bool Delay::init (const vector<string> &params){
	if (params.size() != 1){
		cerr << "Error: Delay called with parameters" << endl;
		return false;
	}
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	delay = atoi(params[0].c_str());
	return true;
}

// Evaluacion
void Delay::evaluate (void){
	struct timeval ini;
	gettimeofday(&ini, NULL);
	setObj(0, getVar(0));//f1 = x1
	double sum = 0;
	for (int i = 1; i < NPARAM; i++)
		sum += getVar(i);
	double g = 1 + (9.0/(NPARAM - 1)) * sum;
	setObj(1, g*(1 - sqrt(getVar(0)/g)));//f2
	struct timeval fin;
	double at;
	do {
		gettimeofday(&fin, NULL);
		at = (double) (fin.tv_sec) * 1.0e6 + (double) (fin.tv_usec) - (double) (ini.tv_sec) * 1.0e6 - (double) (ini.tv_usec);
	} while(at < delay);
}

// Clonacion
Individual* Delay::clone (void) const{
	return new Delay();
}
