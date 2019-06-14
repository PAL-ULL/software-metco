/***********************************************************************************
 * AUTHORS
 *   - Carlos Segura González
 * 
 * DATE 
 * 	January 2013 
 * *********************************************************************************/


#include "NSGA2_MultiObjectivizedWithReference.h"
#include <float.h>


// Define una generación de búsqueda del algoritmo 
void NSGA2_MultiObjectivizedWithReference::runGeneration() {

	// Evaluate the initial population
	if (getGeneration() == 0) {
		multiObjectivize(population, population, population);
		rankPopulation();
	}
	//Multiobjectivize for parent selection
	multiObjectivize(population, population, population);
	// Push the children at the end of the population
	createChildPop();
	//Fill External Archive
	fillExternalArchive();
	// Calculate the fitness as the rank of each element
	rankCrowdingPopulation();
}
	

// Inicializa los parámetros iniciales del algoritmo
bool NSGA2_MultiObjectivizedWithReference::init(const vector<string> &params) {
	
	// Check for the algorithm parameters
	if (params.size() != 3) {
		cerr << "Error NSGA-II: Incorrect parameters (pm pc pSize)" << endl;
		return false;
	}

	// Initiate the parameters
	this->pm = atof(params[0].c_str());
	this->pc = atof(params[1].c_str());
	setPopulationSize(atoi(params[2].c_str()));
	offSpring = new vector<Individual *>();
	return true;
}


// Rellena un frente con las soluciones actuales
void NSGA2_MultiObjectivizedWithReference::getSolution(MOFront *p) {
	//Look for the best one in the original objective
	int bestIndex = 0;
	double currentBestValue = (*population)[0]->getObj(0);
	//Select the individual with best "original" objective
	for (int i = 1; i < population->size(); i++){
		if ( ((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < currentBestValue)){
			bestIndex = i;
			currentBestValue = (*population)[i]->getObj(0);
		} else if ( ((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > currentBestValue)){
			bestIndex = i;	
			currentBestValue = (*population)[i]->getObj(0);
		}
	}
	p->insert((*population)[bestIndex]);
}


// Muestra la información relativa al algoritmo
void NSGA2_MultiObjectivizedWithReference::printInfo(ostream &os) const {
	os << "Genetic Algorithm NSGA2_MultiObjectivizedWithReference" << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Mutation Probability = " << pm << endl;
	os << "Crossover Probability = " << pc << endl;
	os << "Population Size = " << getPopulationSize() << endl;
}


/*******************************************************************************
 * Los siguientes métodos son específicos del NSGA-II y describen las operaciones
 * internas y el propio funcionamiento del algoritmo.
 * *****************************************************************************/

// NSGA-II crowding operator 


/*************************************************************************
 * Create fronts using the non-domination rank until pSize elements have
 * been selected.
 * Each front is ordered using the local crowding distance.
 * In: population without any order with size = pSize * 2
 * Exit: population ordered using the crowded comparison operator with size pSize
 * ***********************************************************************/
void NSGA2_MultiObjectivizedWithReference::rankCrowdingPopulation() {
	//Join population and offspring
	for (int i = 0; i < offSpring->size(); i++){
		population->push_back( (*offSpring)[i] );
	}

	int inserted = 0;

	vector < vector < Individual * > > fronts;
	vector < Individual *> notClassified;

	vector< Individual* > survivors;

	vector<int> indexesBest(1, 0);
	double currentBestValue = (*population)[0]->getObj(0);
	//Select the individual with best "original" objective
	for (int i = 1; i < population->size(); i++){
		if ((*population)[i]->getObj(0) == currentBestValue){
			indexesBest.push_back(i);
		} else if ( ((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < currentBestValue)){
			indexesBest.clear();
			indexesBest.push_back(i);
			currentBestValue = (*population)[i]->getObj(0);
		} else if ( ((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > currentBestValue)){
			indexesBest.clear();
			indexesBest.push_back(i);
			currentBestValue = (*population)[i]->getObj(0);
		}
	}
	int selectedIndex = indexesBest[generateRandom(0, indexesBest.size() - 1)];

	survivors.push_back((*population)[selectedIndex]);
	(*population)[selectedIndex] = (*population)[population->size() - 1];
	population->pop_back();

	while (survivors.size() != (unsigned int) getPopulationSize()){
		multiObjectivize(population, population, &survivors);
		//Calculate fronts
		int differentsInFront0;
		//FastNondominatedSort(population, fronts, notClassified, differentsInFront0, getPopulationSize());
		vector<int> nonDominatedIndexes;
		markNonDominated(*population, nonDominatedIndexes);
		//Select a random individual from the first front
		int selectedIndex = nonDominatedIndexes[generateRandom(0, nonDominatedIndexes.size() - 1)];
		survivors.push_back((*population)[selectedIndex]);
		//Remove element from population. 
		(*population)[selectedIndex] = (*population)[population->size() - 1];
		population->resize(population->size() - 1);
	}
	//Eliminate remaining individuals
	for (int i = 0; i < population->size(); i++){
		delete (*population)[i];
		(*population)[i] = survivors[i];
	}
	// Order each front using the local crowding distance 
	//for (int i = 0; i < fronts.size(); i++)
	//	crowOrder(fronts[i]);

	//Create the ordered population
	//population->clear();
	//for (int i = 0; i < fronts.size() - 1; i++){
//		for (int j = 0; j < fronts[i].size(); j++){
//			population->push_back(fronts[i][j]);
//		}
//	}
	
	//Last front
//	for (int j = 0; j < fronts[fronts.size() - 1].size(); j++){
//		if (population->size() < (unsigned int) getPopulationSize()){
//			population->push_back((fronts[fronts.size() - 1][j]));
//		} else {
//			delete (fronts[fronts.size() - 1][j]);
//		}
//	}

	//Delete not used individuals
//	for (int i = 0; i < notClassified.size(); i++){
//		delete (notClassified[i]);
//	}
}


// Sort and rank the population
// Population has size N, it reorders the population based on the 
void NSGA2_MultiObjectivizedWithReference::rankPopulation() {
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
void NSGA2_MultiObjectivizedWithReference::createChildPop() {
	int tamPop = population->size();
	vector<int> a1;
	
	// Se genera un vector con los numeros de 0 a tamPop-1 (2 veces)
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < tamPop; j++) {
			a1.push_back(j);
		}
	}
	
	// Se reordena la primera mitad
	for (int i = 0; i < tamPop; i++) {
		int rand_value = i + (int) (((double)(tamPop-i))*rand()/(RAND_MAX+1.0));
		int tmp = a1[rand_value];
		a1[rand_value] = a1[i];
		a1[i] = tmp;
	}
	
	// Reodenamos la segunda mitad
	for (int i = tamPop; i < tamPop*2; i++) {
		int rand_value = i + (int) (((double)(2*tamPop-i))*rand()/(RAND_MAX+1.0));
		int tmp = a1[rand_value];
		a1[rand_value] = a1[i];
		a1[i] = tmp;
	}

	offSpring->clear();

	for (int i = 0; i < (tamPop*2)-((tamPop*2) % 4); i += 4){
		// Selection
		int ind1 = a1[i];
		int ind2 = a1[i+1];
		// Binary tournment
		if (ind1 < ind2){
			offSpring->push_back((*population)[ind1]->internalClone());
		} else{
			offSpring->push_back((*population)[ind2]->internalClone());
		}
		int ind3 = a1[i+2];
		int ind4 = a1[i+3];
		if (ind3 < ind4){
			offSpring->push_back((*population)[ind3]->internalClone());
		} else {
			offSpring->push_back((*population)[ind4]->internalClone());
		}
		
		// Crossover
		double vcross = rand() / (RAND_MAX + 1.0);
		if (vcross < pc) {  
			(*offSpring)[offSpring->size()-2]->crossover((*offSpring)[offSpring->size()-1]);
		}
		
		// Mutation
		(*offSpring)[offSpring->size()- 2]->mutation(pm);
		(*offSpring)[offSpring->size()- 1]->mutation(pm);
		evaluate((*offSpring)[offSpring->size()-2]);
		evaluate((*offSpring)[offSpring->size()-1]);
	}
	
	// If population is odd one more must be inserted
	if (tamPop % 2) { 
		int ind1 = a1[a1.size()- 4 ];
		int ind2 = a1[a1.size() - 3];
		Individual *tmp;
		
		// Binary tournment
		if (ind1 < ind2) {
			tmp = (*population)[ind1]->internalClone();
		} else{
			tmp = (*population)[ind2]->internalClone();
		}
		int ind3 = a1[a1.size()-2];
		int ind4 = a1[a1.size()-1];
		if (ind3 < ind4) {
			offSpring->push_back((*population)[ind3]->internalClone());
		} else {
			offSpring->push_back((*population)[ind4]->internalClone());
		}
		
		// Crossover
		double vcross = rand() / (RAND_MAX + 1.0);
		if (vcross < pc) {  
			(*offSpring)[offSpring->size()-1]->crossover(tmp);
		}
		delete(tmp);
		
		// Mutation
		(*offSpring)[offSpring->size()- 1]->mutation(pm);
		evaluate((*offSpring)[offSpring->size()-1]);
	}
}

void NSGA2_MultiObjectivizedWithReference::fillExternalArchive(){
	for (int i = 0; i < offSpring->size(); i++){
		insertInArchive((*offSpring)[i]);
	}
}
void NSGA2_MultiObjectivizedWithReference::multiObjectivize(vector <Individual*> *p, vector<Individual*> *off, vector<Individual*> *reference){
	int start = getSampleInd()->getNumberOfObj() - getNumMultiObjectivizationPlugins();
	for (int i = start; i < getSampleInd()->getNumberOfObj(); i++){
		getMultiObjectivizationPlugin(i - start)->convertToMultiObjective(p, off, reference, i, getGeneration());
	}
}

void NSGA2_MultiObjectivizedWithReference::exchangePerformed(){
	multiObjectivize(population, population, population);	
}
