/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *   Noviembre 2007
 *
 * *********************************************************************************/

#include <vector>
#include <math.h>

#include "DTLZ1.h"

// Inicialización del problema
bool DTLZ1::init (const vector<string> &params) {
	// Check for the problem parameters
	if (params.size() != 1) {
		cout << "Error DTLZ1: Incorrect parameters (number_of_objectives)" << endl;
		return false;
	}

	// Init problem parameters
	int nObj = atoi(params[0].c_str());
	setNumberOfVar(nObj + k - 1);
	setNumberOfObj(nObj);
	return true;
}

// Evaluación de un individuo DTLZ1
// La función g implementada es la sugerida en el paper de referencia
void DTLZ1::evaluate (void) {
	double g = 0.0;
		
	for (unsigned int i = getNumberOfObj(); i <= getNumberOfVar(); i++)
		g += pow(getVar(i-1)-0.5,2) - cos(20 * M_PI * (getVar(i-1)-0.5));
  
	g = 100 * (k + g);
    
	for (unsigned int i = 1; i <= getNumberOfObj(); i++) {
		double f = 0.5 * (1 + g);
		for (int j = getNumberOfObj() - i; j >= 1; j--) 
			f *= getVar(j-1);
		if (i > 1)
			f *= (1 - getVar(getNumberOfObj() - i));
		setObj(i-1, f);
	}
} 

// Clonación de un individuo
Individual* DTLZ1::clone (void) const {
	return new DTLZ1();
}

