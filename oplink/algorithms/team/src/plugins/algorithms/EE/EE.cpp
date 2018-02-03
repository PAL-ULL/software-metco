/***********************************************************************************
 * AUTHORS
 *   - Alejandro Cervantes   
 *   - Sandra Garcia
 *	 - Cristobal Luque
 *   
 * 
 * DATE
 *   March 2008
 ***********************************************************************************/

#include "EE.h"
#include <float.h>


// Static variable for the current objective being ordered
int EE::objOrder = 0;


// Define una generación de búsqueda del algoritmo 
void EE::runGeneration() {

	// Evaluate the initial population
	if (getGeneration() == 0) {

		//for (int i = getPopEvaluatedIndividuals(); i < getPopulationSize(); i++)
		//{
		//	evaluate((*population)[i]);
		//}
		rankPopulation();
	} 
	// Push the children at the end of the population
	createChildPop();
	// Calculate the fitness as the rank of each element
	rankCrowdingPopulation();
}

// Inicializa los parámetros iniciales del algoritmo
bool EE::init(const vector<string> &params) {
	// Check for the algorithm parameters
	if (params.size() != 2) {
		cerr << "Numero de parametros incorrectos:(varianza, tamanio de poblacion)" << endl;
		return false;
	}
	this->varianza = atof(params[0].c_str());
	setPopulationSize(atoi(params[1].c_str()));

	getSampleInd()->setMigrationDataSize(1);
	getSampleInd()->setMigrationData(0, varianza); // valor inicial de la varianza
	return true;
}


// Rellena un frente con las soluciones actuales
void EE::getSolution(MOFront *p) {
	for (unsigned int i = 0;  i < population->size(); i++) {
		p->insert((*population)[i]);
	}
}


// Muestra la información relativa al algoritmo
void EE::printInfo(ostream &os) const {
	os << "Genetic Algorithm EE" << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Population Size = " << getPopulationSize() << endl;
}


/*************************************************************************
 * Create fronts using the non-domination rank until pSize elements have
 * been selected.
 * Each front is ordered using the local crowding distance.
 * In: population without any order with size = pSize * 2
 * Exit: population ordered using the crowded comparison operator with size pSize
 * ***********************************************************************/

void EE::rankCrowdingPopulation() {
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

void EE::rankPopulation() {
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

//Crea la poblacion sucesora
void EE::createChildPop() {
	int tamPop = population->size();
	for (int i=0; i<tamPop; i++){
		Individual *ind=muta((*population)[i]);
		population->push_back( ind );
		evaluate(ind);
		insertInArchive(ind);
	}
}


//Funcion gaussiana
double EE::gaussiana (double media,double vari){
		double u1, u2;
		do{
			u1=rand()*1./RAND_MAX;
		}while (u1==0.0);		
		u2=rand()*1./RAND_MAX;
     
		return media+sqrt(vari)*sqrt(abso(-2*log(u1)))*sin(2*PI*u2);
}


//Calcula el valor absoluto de un numero
double EE::abso(double zx){
	if (zx<0) zx=-zx;
		return zx;
}

//Mutacion modificada
Individual* EE::muta(Individual* ind){

	double varianza2;
	varianza2=ind->getMigrationData(0);
	Individual* ind2= ind->internalClone();
	varianza2=varianza2*exp(ALFA*gaussiana(0,1)); // Vamos a utilizar el vector de migración para controlar la varianza.
	if (varianza2 < .0000000001) 
		varianza2 =.0000000001;


	for (unsigned int i = 0; i < ind2->getNumberOfVar(); i++) 
	{
		ind2->setVar(i, gaussiana(ind2->getVar(i),varianza2));
		
		if (ind2->getVar(i) > ind2->getMaximum(i) ){
			ind2->setVar(i,ind2->getMaximum(i));
		}else if (ind2->getVar(i) < ind2->getMinimum(i) ){
			ind2->setVar(i, ind2->getMinimum(i) );
		}   
		
	}

	ind2->setMigrationData(0,varianza2);
	return ind2;
}


/************************************************************************************
 * Rellena la poblacion hasta llegar al tamaño de la población, con individuos que 
 * se crean llamando al metodo create del individuo
 * *********************************************************************************/
void EE::fillPopWithNewIndsAndEvaluate() {
	if (getPopulationSize() == -1) {
		cerr << "Warning: fillPopWithNewInds called but pSize has not been fixed. Using default value: 100" << endl;
		setPopulationSize(100);
	}

	for (int i = 0; i < population->size(); i++) {
		Individual *ind = ((*population)[i]);
		ind->setMigrationDataSize(1);
		ind->setMigrationData(0, this->varianza); // valor inicial de la varianza
	}

	for (int i = population->size(); i < getPopulationSize(); i++) {
		Individual *ind = getSampleInd()->internalClone();
		ind->restart();
		evaluate(ind);
		insertInArchive(ind);
		population->push_back (ind);
	}
}
