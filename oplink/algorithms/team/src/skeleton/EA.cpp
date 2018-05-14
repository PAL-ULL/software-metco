/***********************************************************************************
 * AUTORES
 *   - Ofelia Gonz�lez P�rez
 *   - Gara Miranda Valladares
 *   - Carlos Segura Gonz�lez
 * 
 * FECHA
 *    Noviembre 2007
 ************************************************************************************/

#include "EA.h"
#include "Configuration.h"
#include "MOFrontVector.h"
#include "MOFrontBinaryInteger.h"
#include <sys/time.h>
#include <sstream>
#include <dlfcn.h>

//Criterios de parada locales


// Constructor
EA::EA() {
	// init seed
	// initRandomSeed();
	// set initial values
	sampleInd = NULL;
	outputPrinter = NULL;
	localSolution = NULL;
	ls = NULL;
	population = new vector<Individual*>;
	this->pSize = -1;
	initiated = 0;
	this->printPeriod = this->nextPrint = 0;
	generateArchive = true;
	evaluations = 0;
}

/* ********************************************************************************
 * Elimina la soluci�n local y la poblaci�n
 * ********************************************************************************/
EA::~EA() {
	delete localSolution;
	for (unsigned int i = 0; i < population->size(); i++)
		delete (*population)[i];
	delete population;
}

void EA::setGenerateArchive(const bool generate, const int type){
	this->generateArchive = generate;
	if (generate){
		if (sampleInd == NULL){
			cerr << "Error interno: sampleInd es null" << endl;
		}
		switch(type){
			case ARCHIVE_VECTOR:
				localSolution = new MOFrontVector(sampleInd, true, true);
				break;
			case ARCHIVE_INSO1:
				localSolution = new MOFrontBinaryInteger(sampleInd, true, true);
				break;
			default:
				cerr << "Error: tipo de archivo no definido" << endl;
				exit(-1);
		}
	}
}
/* ********************************************************************************
 * Inicializa los atributos necesiarios para fijar un determinado criterio de
 * parada
 * *******************************************************************************/
void EA::setStoppingCriterion(const int critStop, const double critStopValue) {
	generationActual = 0;
	evaluationActual = 0;
	this->critStop = critStop;
	this->critStopValue = critStopValue;
	if (critStop == getTypeStoppingCriterion("TIME")) {
		struct timeval actualTime; 
		gettimeofday(&actualTime, NULL);
		time = ((double) (actualTime.tv_sec) * 1.0e6 + (double) (actualTime.tv_usec))/1.0e6 + critStopValue;
		evaluations = 0;
	} else if (critStop == getTypeStoppingCriterion("EVALUATIONS")) {
		evaluations = (int)critStopValue;
	} else if (critStop == getTypeStoppingCriterion("QUALITY")){
		quality = critStopValue;
	} else {
		cout << "\tError: Stopping criterion nondefined" << endl << flush; 
		exit(-1);
	}
}



/* ******************************************************************************
 * Este m�todo devuelve true cuando se ha alcanzado el criterio de parada local
 * del EA. Los criterios actuales pueden ser tiempo de ejecuci�n o n�mero de 
 * evaluaciones realizadas. 
 * ******************************************************************************/
bool EA::hasFinished() {
	if (critStop == getTypeStoppingCriterion("TIME")) {
		struct timeval actualTime; 
		gettimeofday(&actualTime, NULL);
		double at = ((double) (actualTime.tv_sec) * 1.0e6 + (double) (actualTime.tv_usec))/1.0e6;
		return (at >= time);
	} else if (critStop == getTypeStoppingCriterion("EVALUATIONS")) {
		return (evaluationActual >= evaluations);
	} else if (critStop == getTypeStoppingCriterion("QUALITY")){
		MOFront *p = new MOFrontVector(sampleInd, false, false);
		getSolution(p);
		bool reached = scoreAlgorithm->score(p) >= quality;
		delete p;
		return (reached);
	} else {
		cout << "\tError: Stopping criterion nondefined" << endl << flush; 
		return (true);
	}
}


/* *********************************************************************************
 * Realiza las impresiones intermedias de la soluci�n encontrada en funci�n del 
 * periodo de impresi�n fijado por el usuario
 * *********************************************************************************/
void EA::checkPrint() {
	if (printPeriod == 0)
		return;
	if ((critStop == getTypeStoppingCriterion("TIME")) && (getElapsedTime() >= nextPrint)) {
		outputPrinter->printSolution(this, false);
		nextPrint += printPeriod;
	} else if ((critStop == getTypeStoppingCriterion("EVALUATIONS")) && (getPerformedEvaluations() >= nextPrint)){
		outputPrinter->printSolution(this, false);
		nextPrint += printPeriod;
	}
}


/*************************************************************************************
 * Este m�todo lleva a cabo la ejecuci�n secuencial e independiente del algoritmo.
 * Primero genera la poblaci�n inicial y a continuaci�n va ejecutando generaci�n
 * tras generaci�n hasta que se cumpla el criterio de parada.
 * Cada cierto numero de evaluaciones muestra por pantalla la soluci�n actual.
 * ***********************************************************************************/
void EA::run() {
	struct timeval actualTime; 
	gettimeofday(&actualTime, NULL);
	startTime = (double) (actualTime.tv_sec) * 1.0e6 + (double) (actualTime.tv_usec);
	fillPopWithNewIndsAndEvaluate();
	do {
		checkPrint();
		runGenerationInc();
	} while (!hasFinished());
	checkPrint();
}


/*************************************************************************************
 * Este m�todo es invocado cuando se reciben individuos migrados desde otra isla. 
 * El algoritmo debe sacrificar a algunos de los individuos actuales, 
 * intercambi�ndolos por los nuevos individuos. La elecci�n de que individuos se 
 * intercambian es dependiente del algoritmo.
 * El conjunto de individuos que sean sacrificados (ya no se vayan a usar m�s)
 * y los individuos del vector migrados que no se utilicen deben ser borrados
 * mediante el uso del operador delete.
 * Por defecto se intercambian los primeros individuos de la poblaci�n por
 * los individuos que se han recibido
 * ***********************************************************************************/
void EA::received(vector<Individual*> &mig){
	unsigned int indCopy = (mig.size() < population->size())?mig.size():population->size();
	for (unsigned int i = 0; i < indCopy; i++){
		delete (*population)[i];
		(*population)[i] = mig[i];
	}
	for (unsigned int i = indCopy; i < mig.size(); i++){
		delete(mig[i]);
	}
}

/*************************************************************************************
 * Ejecuci�n de generaciones
 * - Se ejecuta la generaci�n
 * - Se llama al codigo generationCode del individuo 
 * - Se incrementa el n�mero de generaciones hechas
 * ***********************************************************************************/
void EA::runGenerationInc() {
	sampleInd->generationCode();
	runGeneration();
	ls->localSearch(population, generateArchive?localSolution:NULL);
	
	if (localFrontSize != 0){
		localSolution->crowOrder(localFrontSize);
	}
	generationActual++;
}


/************************************************************************************
 * Inicializaci�n de la poblaci�n con un conjunto de individuos que ya estan
 * evaluados
 * El resto se generan de forma aleatoria y se evaluan 
 * *********************************************************************************/
void EA::fillPopWithEvaluatedInds(const vector<Individual *> &newInds) {
	for (unsigned int i = 0; i < newInds.size(); i++) {
		population->push_back(newInds[i]);
	}
	fillPopWithNewIndsAndEvaluate();
}


/************************************************************************************
 * Rellena la poblacion hasta llegar al tama�o de la poblaci�n, con individuos que 
 * se crean llamando al metodo create del individuo
 * *********************************************************************************/
void EA::fillPopWithNewIndsAndEvaluate() {
	if (pSize == -1) {
		cerr << "Warning: fillPopWithNewInds called but pSize has not been fixed. Using default value: 100" << endl;
		pSize = 100;
	}

	for (int i = population->size(); i < getPopulationSize(); i++) {
		Individual *ind = sampleInd->internalClone();
		ind->restart();
		evaluate(ind);
		population->push_back (ind);
	}
}


/************************************************************************************
 * Reinicia los atributos relacionados con los criterios de parada
 * **********************************************************************************/
void EA::resetStopConditions() {
	evaluationActual = 0;
	generationActual = 0;
	if (critStop == getTypeStoppingCriterion("TIME")) {
		struct timeval actualTime; 
		gettimeofday(&actualTime, NULL);
		time = ((double) (actualTime.tv_sec) * 1.0e6 + (double) (actualTime.tv_usec))/1.0e6 + critStopValue;	
	}
}

double EA::getElapsedTime() const {
	struct timeval actualTime; 
	gettimeofday(&actualTime, NULL);
	double at = (double) (actualTime.tv_sec) * 1.0e6 + (double) (actualTime.tv_usec);
	return ((at - startTime) / 1.0e6);
}


/************************************************************************************
 * - Evalua el individuo
  * - Incrementa evaluationActual
 * **********************************************************************************/
void EA::evaluate(Individual *ind) {
	ind->evaluate();
	if (generateArchive) {
		localSolution->insert(ind);
	} 
	evaluationActual++;
}

/****************************************************************************************
 *   Guarda el individuo en el frente de soluciones locales en las ejecuciones paralelas
 *   o secuenciales en las que se indique que se quiere guardar. Para indicar que
 *   se almacene el frente local en las ejecuciones secuenciales habr� que indicarlo
 *   en la l�nea de comandos de la ejecuci�n
 *   Supone que ya esta evaluado
 ***************************************************************************************/

void EA::insertInArchive(Individual *ind){
	if (generateArchive) {
		localSolution->insert(ind);
	} 
}



/***********************************************************************************
 * Obtiene un EA* a partir del nombre del EA. El fichero de la librer�a debe
 * tener el mismo nombre que el EA y estar situado en el mismo directorio que
 * el programa
 * *********************************************************************************/
EA *getEA(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError, Individual *ind){
	EA *tmp = (EA*)loadPlugin("EA", pluginPath, pluginName, args, printError, false);
	tmp->setSampleInd(ind);
	if (!tmp->init(args)){
		if (printError)
			cerr << "Plugin \"" << "EA::" << pluginName << "\" initialization failed" << endl;
		return NULL;
	}
	return tmp;
}

// Creado por Antonio J. Nebro
// Torneo binario
// Returns the index of the winning individual in the population
int EA::binaryTournament(vector<Individual *>* pop) {
	//int dominates = MOFront::dominate(newIndividual, aux) 
	int individual1 ;
	int individual2 ;
	int winner      ;
	
	if (pop->size() == 1)
		return 0 ; // There is only one individual
        
	individual1 = (int)((pop->size()-1) * ((double)rand()/RAND_MAX)) ;
	individual2 = individual1 ;
	while (individual2 == individual1) {
		double rnd = (double)rand()/RAND_MAX ;
		individual2 = (int)((pop->size()) * rnd) ;
 	}
 	
	if ((individual1 >= pop->size()) || (individual1 < 0)) {
  		cerr << "Invalid individual1 in binary tournament. Index: " << individual1 << endl ;
		exit(-1) ;
	} // if
	if ((individual2 >= pop->size()) || (individual2 < 0)) {
  		cerr << "Invalid individual2 in binary tournament. Index: " << individual2 << endl ;
		exit(-1) ;
	} // if

	int dominates = dominanceTest((*pop)[individual1], (*pop)[individual2]) ;
	if (dominates == SECOND_DOMINATES) { // individual2 dominates Individual1
		winner = individual2 ;
	} // if
	else if (dominates == FIRST_DOMINATES) { // individual1 dominates individual2
		winner = individual1 ;
	}
	else {
		if ((rand()/(RAND_MAX) < 0.5))
			winner = individual1 ;
		else
			winner = individual2 ; 	
	}
	
	return winner ;
} // Torneo binario
