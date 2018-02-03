/***********************************************************************************
 * AUTORES
 *   - Antonio J. Nebro
 * 
 * FECHA
 *    Febreri 2008
 ************************************************************************************/

#include "Kursawe.h"
#include <vector>

// Valores minimos de cada variable
const double Kursawe::MINIMUM[] = {-5.0, -5.0, -5.0};
// Valores maximos de cada variable
const double Kursawe::MAXIMUM[] = {5.0, 5.0, 5.0};

// Inicialización. 
// Fija:
//   - Numero de variables
//   - Numero de objetivos
//   - Direccion de optimizacion de los objetivos
//   - Rangos de las variables (mínimos y máximos)
bool Kursawe::init (const vector<string> &params){
	if (params.size() != 0){
		cerr << "Error: Kursawe called with parameters" << endl;
		return false;
	}
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	return true;
}

// Evaluacion
void Kursawe::evaluate (void){
	// first function
	double result = 0.0 ; 
	for (int i = 0; i  < (NPARAM - 1); i++) 
		result += -10.0 * exp((-0.2)*sqrt(pow(getVar(i),2.0) + 
		                                  pow (getVar(i+1),2.0))) ;
		
	setObj(0, result) ;
	
	// second function
	const double a = 0.8 ;
	const double b = 3 ;
	result = 0.0 ;
	for (int i = 0; i < NPARAM; i++) 
		result += pow(fabs(getVar(i)), a) + 5.0 * sin(pow(getVar(i), b)) ;
	setObj(1, result) ;	
}

// Clonacion
Individual* Kursawe::clone (void) const{
	return new Kursawe();
}
