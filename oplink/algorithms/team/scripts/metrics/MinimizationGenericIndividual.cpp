#include "MinimizationGenericIndividual.h"

bool MinimizationGenericIndividual::init (const vector<string> &params){
	return true;
}

// Mutacion uniforme
void MinimizationGenericIndividual::mutation (double pm){
	cerr << "Error mutation llamado en MinimizationGenericIndividual" << endl << flush;
	exit(-1);
}

// Crossover uniforme
void MinimizationGenericIndividual::crossover (Individual* ind){
	cerr << "Error crossover llamado en MinimizationGenericIndividual" << endl << flush;
	exit(-1);
}

// Evaluacion
void MinimizationGenericIndividual::evaluate (void){
	cerr << "Error evaluate llamado en MinimizationGenericIndividual" << endl << flush;
	exit(-1);
}

// Clonacion
Individual* MinimizationGenericIndividual::clone (void) const{
	return new MinimizationGenericIndividual();
}
