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

#include "DTLZ3.h"


// Initialization function
bool DTLZ3::init (const vector<string> &params) {

	// Check for the problem parameters
	if (params.size() != 1) {
		cout << "Error DTLZ3: Incorrect parameters (number_of_objectives)" << endl;
		return false;	
	}

	// Init the problem parameters
	int nObj = atoi(params[0].c_str());
	setNumberOfVar(nObj + k - 1);
	setNumberOfObj(nObj);
	return true;
}

// Individual evaluation and selection
// La función g implementada es la sugerida en el paper de referencia
void DTLZ3::evaluate (void) {
   double g = 0.0;
   
   for (int i = getNumberOfObj(); i <= getNumberOfVar(); i++)
     g += pow(getVar(i-1)-0.5,2) - cos(20 * M_PI * (getVar(i-1)-0.5));
   
	 g = 100 * (k + g);
     
   for (int i = 1; i <= getNumberOfObj(); i++) {
     double f = (1 + g);
     for (int j = getNumberOfObj() - i; j >= 1; j--) 
       f *= cos(getVar(j-1) * M_PI / 2);
 
     if (i > 1)
       f *= sin(getVar(getNumberOfObj() - i) * M_PI / 2);
     
     setObj(i-1, f);
   } 
} 


// Clonación de un individuo
Individual* DTLZ3::clone (void) const {
	return new DTLZ3();
}

