/***********************************************************************************
 * AUTORES
 *   - Antonio J. Nebro
 * 
 * FECHA
 *    Febrero 2008
 ************************************************************************************/

#include "Schaffer.h"
#include <vector>

// Valores minimos de cada variable
const double Schaffer::MINIMUM[] = {-1000.0};
// Valores maximos de cada variable
const double Schaffer::MAXIMUM[] = {1000.0};
// Direcciones de optimizacion
const int Schaffer::OBJFUNC[] = {MINIMIZE, MINIMIZE};

// Inicialización. 
// Fija:
//   - Numero de variables
//   - Numero de objetivos
//   - Direccion de optimizacion de los objetivos
//   - Rangos de las variables (mínimos y máximos)
bool Schaffer::init (const vector<string> &params){
	if (params.size() != 0){
		cerr << "Error: Schaffer called with parameters" << endl;
		return false;
	}
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	return true;
}

// Evaluacion
void Schaffer::evaluate (void){
	// first function	
	setObj(0, getVar(0)*getVar(0)) ;
	
	// second function

	setObj(1, pow(getVar(0)-2.0, 2.0)) ;	
}
    

// Clonacion
Individual* Schaffer::clone (void) const{
	return new Schaffer();
}
