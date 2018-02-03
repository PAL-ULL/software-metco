/***********************************************************************************
 * AUTORES
 *   - Antonio J. Nebro
 * 
 * FECHA
 *    Febrero 2008
 ************************************************************************************/

#include "Fonseca.h"
#include <vector>

// Valores minimos de cada variable
const double Fonseca::MINIMUM[] = {-4.0, -4.0, -4.0};
// Valores maximos de cada variable
const double Fonseca::MAXIMUM[] = {4.0, 4.0, 4.0};

// Inicialización.
// Fija:
//   - Numero de variables
//   - Numero de objetivos
//   - Direccion de optimizacion de los objetivos
//   - Rangos de las variables (mínimos y máximos)
bool Fonseca::init (const vector<string> &params){
	if (params.size() != 0){
		cerr << "Error: Fonseca called with parameters" << endl;
		return false;
	}
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	return true;
}

// Evaluacion
void Fonseca::evaluate (void){
	// first function	
	double sum ; 
	sum = 0.0 ;
	
	for (int i = 0; i < NPARAM; i++)
    sum += pow(getVar(i) - (1.0/sqrt((double)NPARAM)),2.0);         
   	
	setObj(0, 1 - exp(-1.0*sum)) ;
	
	// second function
	sum = 0.0 ;
	
	for (int i = 0; i < NPARAM; i++)
    sum += pow(getVar(i) + (1.0/sqrt((double)NPARAM)),2.0);         
   	
	setObj(1, 1 - exp(-1.0*sum)) ;
}
    

// Clonacion
Individual* Fonseca::clone (void) const{
	return new Fonseca();
}
