/***********************************************************************************
 * AUTHORS
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * DATE
 *    November 2007
 ***********************************************************************************/

#include "Configuration.h"

//Selection
#define N_TYPE_SELECTION 4  
const string Configuration::TYPE_SELECTION[] = {"CYCLICAL", "ELITIST", "PROBABILITY", "NO_CHANGE"};

//Global Stop criterions and Actions
#define N_GLOBAL_CRIT_STOP      5 
const string Configuration::GLOBAL_CRIT_STOP[]      = {"TIME", "EXECUTIONS", "EVALUATIONS", "CLEAR_STATISTICS", "QUALITY"};

//Solution Sources
#define N_SOLUTION_SOURCES 2
const string Configuration::SOLUTION_SOURCES[] = {"ALGORITHM_INTERNAL", "ARCHIVE"};

// Bison 
extern FILE *Confin;
int Confparse(Configuration &configuration, int myId);

// Constructor 
Configuration::Configuration(){
	migrationProbability = -1;	
	numberOfIndividualsToMigrate = -1;
	maxGlobalFrontSize = -1;
	maxFinalSolutionSize = -1;
	initPercentOfIndividuals = -1;
	numberParallelExecutions = -1;
	errorParsing = 0;
	sendResultsPerGen = true;
	conversion = NULL;
}

// Parseo del fichero y almacenamiento de datos
void Configuration::load(const string &confFile, const int myId){
	Confin = fopen(confFile.c_str(), "r");
	if (Confin == NULL){
		setErrorParsingStr(string("Error: file") + confFile + "could not be opened");
		return;
	}
	Confparse(*this, myId);
	fclose(Confin);
}

// Obtencion del identificador de fuente de solucion
int Configuration::getSolutionSource(const string &typeSolutionSource){
	return getIndex(typeSolutionSource, SOLUTION_SOURCES, N_SOLUTION_SOURCES);
}

// Obtencion de identificador de tipo de seleccion
int Configuration::getTypeSelection (const string &typeSelect) {
	return getIndex(typeSelect, TYPE_SELECTION, N_TYPE_SELECTION);
}

// Obtencion de identificador de tipo de criterio de parada
int Configuration::getGlobalTypeStoppingCriterion (const string &crit){
	return getIndex(crit, GLOBAL_CRIT_STOP, N_GLOBAL_CRIT_STOP);
}

TAlgorithm::TAlgorithm(){
	critStop = -1; 
	valueSC = -1; 
	migrationSelector = NULL;
	exchangeSelector = NULL;
	mutation = NULL;
	crossover = NULL;
	solutionSource = (solution_sources_ids_t)-1;
	ind = NULL;
	maxLocalFrontSize = 0;
  sharingGroup = -1;
}
