/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 ************************************************************************************/

#include "ZDT6.h"
#include <vector>

// Valores minimos de cada variable
const double ZDT6::MINIMUM[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Valores maximos de cada variable
const double ZDT6::MAXIMUM[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

// Inicialización. 
// Fija:
//   - Numero de variables
//   - Numero de objetivos
//   - Direccion de optimizacion de los objetivos
//   - Rangos de las variables (mínimos y máximos)
bool ZDT6::init (const vector<string> &params){ 
	if (params.size() != 0){
		cerr << "ZDT6 called with parameters" << endl;
		return false;
	}
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	return true;
}

// Mutacion uniforme
void ZDT6::dependentMutation (double pm) {
	mutate_Pol2(pm);
}

// Crossover uniforme
void ZDT6::dependentCrossover (Individual* ind) {
	crossover_SBX2(ind);
}

// Evaluacion
void ZDT6::evaluate (void) {
	setObj(0, 1 - exp(-4*getVar(0)) * pow(sin(6*M_PI*getVar(0)),6));
	double sum = 0;
	for (int i = 1; i < NPARAM; i++)
		sum += getVar(i);
	double g = 1 + 9.0 * pow(sum/9.0, 0.25);
	setObj(1, g*(1- pow(getObj(0)/g, 2)));
}

// Clonacion
Individual* ZDT6::clone () const {
	return new ZDT6();
}
