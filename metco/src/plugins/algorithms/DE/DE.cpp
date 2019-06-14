/***********************************************************************************
 * AUTHORS
 *   - María Dolores Jaríz Simón
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * DATE
 *   January 2008
 **********************************************************************************/


#include "DE.h"
#include <float.h>


// Static variable for the current objective being ordered
int DE::objOrder = 0;


// Define una generación de búsqueda del algoritmo 
void DE::runGeneration() {
	// Evaluate the initial population
	if (getGeneration() == 0) {
		rankPopulation();
	} 
	// Push the children at the end of the population
	createChildPop();
	// Calculate the fitness as the rank of each element
	rankCrowdingPopulation();
}
	

// Inicializa los parámetros iniciales del algoritmo
bool DE::init(const vector<string> &params) {
	// Check for the algorithm parameters	
	if (params.size() != 3) {
		cerr << "Error DE: Incorrect parameters (F CR pSize)" << endl;
		return false;
	}
		
	this->F = atof(params[0].c_str());
	this->CR = atof(params[1].c_str());

	if ((F < 0.3) || (F > 1.0)) {
		cerr << "Error DE: Incorrect value for F (F must be in the interval [0.4, 1.0])" << endl;
		return false;
	}

	if ((CR < 0.1) || (CR > 1.0)) {
		cerr << "Error DE: Incorrect value for CR (CR must be in the interval [0.1, 1.0])" << endl;
		return false;
	}
	
	// Initiate the parameters
	setPopulationSize(atoi(params[2].c_str()));
	return true;
}


// Rellena un frente con las soluciones actuales
void DE::getSolution(MOFront *p) {
	for (unsigned int i = 0;  i < population->size(); i++) {
		p->insert((*population)[i]);
	}
}


// Muestra la información relativa al algoritmo
void DE::printInfo(ostream &os) const {
	os << "Genetic Algorithm DE" << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Weight factor = " << F << endl; 
	os << "Crossover = " << CR << endl; 
	os << "Population Size = " << getPopulationSize() << endl;
}


/*******************************************************************************
 * Los siguientes métodos son específicos del NSGA-II y describen las operaciones
 * internas y el propio funcionamiento del algoritmo.
 * *****************************************************************************/

/*************************************************************************
 * Create fronts using the non-domination rank until pSize elements have
 * been selected.
 * Each front is ordered using the local crowding distance.
 * In: population without any order with size = pSize * 2
 * Exit: population ordered using the crowded comparison operator with size pSize
 * ***********************************************************************/
void DE::rankCrowdingPopulation() {
	int inserted = 0;

	vector < vector < Individual * > > fronts;
	vector < Individual *> notClassified;

	//Calculate fronts
	int differentsInFront0;
	FastNondominatedSort(population, fronts, notClassified, differentsInFront0, getPopulationSize());

	// Order each front using the local crowding distance 
	for (int i = 0; i < fronts.size(); i++)
		crowOrder(fronts[i]);

	//Create the ordered population
	population->clear();
	for (int i = 0; i < fronts.size() - 1; i++){
		for (int j = 0; j < fronts[i].size(); j++){
			population->push_back(fronts[i][j]);
		}
	}
	
	//Last front
	for (int j = 0; j < fronts[fronts.size() - 1].size(); j++){
		if (population->size() < (unsigned int) getPopulationSize()){
			population->push_back((fronts[fronts.size() - 1][j]));
		} else {
			delete (fronts[fronts.size() - 1][j]);
		}
	}

	//Delete not used individuals
	for (int i = 0; i < notClassified.size(); i++){
		delete (notClassified[i]);
	}
}

void DE::rankPopulation() {
	vector < vector < Individual * > > fronts;
	vector < Individual *> notClassified;
	int differentsInFront0 = 0;
	FastNondominatedSort(population, fronts, notClassified, differentsInFront0, population->size());
	population->clear();
	for (int i = 0; i < fronts.size(); i++){
		for (int j = 0; j< fronts[i].size(); j++){
			population->push_back(fronts[i][j]);
		}
	}
}

/*******************************************************************************
 * Create a child population by applying:
 *  - Selection
 *  - Crossover
 *  - Recombination
 * *****************************************************************************/

void DE::createChildPop() {
	int  n;                     // Counting variables. 
	int   r1, r2, r3;           // Placeholders for random indexes.   
	double y; 
	double value;
	
	int tamPop = population->size();                     // Size of the population 
	int D = (*population)[tamPop-1]->getNumberOfVar();   // Number of the genes
	
	for (int i=0; i<tamPop; i++)      {     
		do {                        // Pick a random population member 
			r1 = (int)randomUniform(0.0, (double)tamPop);
		} while(r1==i);            

	  do {                        // Pick a random population member 
			r2 = (int)randomUniform(0.0, (double)tamPop); 
	  } while((r2==i) || (r2==r1));

		do {                        // Pick a random population member 
			r3 = (int)randomUniform(0.0, (double)tamPop); 
		} while((r3==i) || (r3==r1) || (r3==r2));
	        

	  /*------------------------- DE/rand/1/bin strategy ------------------------------*/
	        
	  population->push_back((*population)[i]->internalClone());  /* the individual of
																    the population is
																    cloned */                                                       
                                                                   
	  n = (int)randomUniform(0.0,(double)D);
		for (int L=0; L<D; L++){   // perform D binomial trials
	  	if ((randomUniform(0.0,1.0) < CR) || L == (D-1)){ // change at least one parameter
				value = (*population)[r1]->getVar(n) + F*((*population)[r2]->getVar(n)-(*population)[r3]->getVar(n));	   
				(*population)[i]->setVar(n, value);
		 	}
			n = (n+1)%D;
		}
	  
		for (int j=0; j<D; j++){
			if ((*population)[i]->getVar(j) > (*population)[i]->getMaximum(j)){
   	  	(*population)[i]->setVar(j, (*population)[i]->getMaximum(j));
			} else if ((*population)[i]->getVar(j) < (*population)[i]->getMinimum(j)){
				(*population)[i]->setVar(j, (*population)[i]->getMinimum(j));
			}
		} // End for j
		evaluate((*population)[i]);
		insertInArchive((*population)[i]);
	} // End for i
}
