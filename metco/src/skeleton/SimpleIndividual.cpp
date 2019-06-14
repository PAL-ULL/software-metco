/***********************************************************************************
 * AUTORES
 *   - Carlos Segura González
 * 
 * FECHA
 *    Mayo 2010 
 ************************************************************************************/

#include "SimpleIndividual.h"

void SimpleIndividual::evaluate (void){
	cout << "It shouldn't be executed never SimpleIndividual::evaluate" << endl;
}

// Clonacion
Individual* SimpleIndividual::clone (void) const{
	SimpleIndividual *simp = new SimpleIndividual();
	simp->optDirections = optDirections;
	return simp; 
}
