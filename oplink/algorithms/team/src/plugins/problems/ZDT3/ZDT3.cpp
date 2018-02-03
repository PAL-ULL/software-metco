/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 ************************************************************************************/

#include "ZDT3.h"
#include <vector>
#include <time.h>
#include <sys/time.h>

// Inicialización. 
// Fija:
//   - Numero de variables
//   - Numero de objetivos
//   - Direccion de optimizacion de los objetivos
//   - Rangos de las variables (mínimos y máximos)
bool ZDT3::init (const vector<string> &params){
	if (params.size() != 0){
		cerr << "Error: ZDT3 called with parameters" << endl;
		return false;
	}
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	return true;
}

// Evaluacion
void ZDT3::evaluate (void) {
	struct timeval ini;
	if (delay != 0)
		gettimeofday(&ini, NULL);

	setObj(0, getVar(0));
	double sum = 0;
	for (int i = 1; i < NPARAM; i++)
		sum += getVar(i);
	double g = 1 + 9.0/(NPARAM - 1)* sum ;
	double tmp = getVar(0)/ g;
	setObj(1, g*(1 - sqrt(tmp) - tmp*sin(10*M_PI*getObj(0))));

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
Individual* ZDT3::clone (void) const {
	return new ZDT3();
}

long ZDT3::delay;
