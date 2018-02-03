/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 * 
 * DESCRIPCION
 *    Analizador sintactico del fichero de configuración para las ejecuciones
 *    paralelas
 *
 **********************************************************************************/

// Conjunto de tokens
%token  TYPE_STOPPING_CRITERION VALUE_STOPPING_CRITERION EXECUTION_MODEL CORCHETE_ABIERTO CORCHETE_CERRADO INIT_PERCENT_OF_INDIVIDUALS MIGRATION_PROBABILITY NUMBER_OF_INDIVIDUALS_TO_MIGRATE NUMERO DOS_PUNTOS STRING MAX_GLOBAL_FRONT_SIZE MAX_LOCAL_FRONT_SIZE SHARING_GROUP MAX_FINAL_SOLUTION_SIZE INDIVIDUAL NEWLINE INDIVIDUALPARAMS ALGORITHM COMA SEND_RESULTS_PER_GENERATION CONVERSION CONVERSIONPARAMS SOLUTION_SOURCE SCORE SCOREPARAMS MIGRATION_SELECTOR EXCHANGE_SELECTOR MUTATION CROSSOVER OUTPUT_PATH PLUGIN_PATH MIGRATION_TOPOLOGY MIGRATION_TOPOLOGY_PARAMS INIT_POP_ISLAND_LOADER INIT_POP_ISLAND_LOADER_PARAMS LOCAL_SEARCH LOCAL_SEARCH_PARAMS MULTIOBJECTIVIZATION MULTIOBJECTIVIZATION_PARAMS NUMBER_PARALLEL_EXECUTIONS
%{
	#include <strings.h>
	#include <string>
	#include <sstream>
	#include "Conversion.h"
	#include "ScoreAlgorithm.h"
	#include "MigrationTopology.h"
	#include "InitPopIslandLoader.h"
	#include "MigrationSelector.h"
	#include "ExchangeSelector.h"
	#include "Individual.h"
	#include "Configuration.h"
	#include "EA.h"
	#include "utils.h"
	using namespace std;

	// Inicializamos los parámetros de configuración a valores no válidos, para detectar
	// errores en el fichero de configuración
	extern double realValueConf;
	extern string stringValueConf;
	void Conferror(Configuration &conf, int myId, char *);
	int Conflex();
	vector<double> realBufferConf; 
	vector<string> stringBufferConf;
	vector<TAlgorithm> teamAlgorithms;
	vector<TExecutionModel> executionModel;
	TExecutionModel nextModel;
	bool realExecutionModel = false;
	double initPercentOfIndividuals = -1;
	double migrationProbability = -1;
	int numberOfIndividualsToMigrate = -1;
	int maxGlobalFrontSize = -1;
	int maxFinalSolutionSize = -1;
	int parSize = -1;
	int sendResultsPerGeneration = -1;
	int numberParallelExecutions = -1;
	bool insertNextModel(const Configuration &configuration);
	vector<string> newConfig;
	vector<Individual*> allIndividuals;

//Deteccion de redefiniciones de parámetros
#define setCheckedValue(old, newValue, error, configuration)\
	if (old != -1){\
		configuration.setErrorParsingStr(string(error) + " redefined");\
	}\
	if (newValue == -1){\
		configuration.setErrorParsingStr(string(error) + ": not recognized value");\
	}\
	old = newValue;\

//Deteccion de parámetros obligatorios no definidos
#define checkDefined(value, error, configuration)\
	if (value == -1){\
		configuration.setErrorParsingStr(string(error) + " non-defined");\
	}\

	void reset();

%}

%parse-param{Configuration &configuration}
%parse-param{int myId}

%%

// Fichero de configuracion completo
confFile:
	{ reset(); }
	generalSection algorithmSection
		{
			if (!realExecutionModel){
				configuration.setErrorParsingStr("Execution Model non-defined");
			}
			// Chequear si todo esta definido
			checkDefined(initPercentOfIndividuals, "Init_Percent_Of_Individuals", configuration);
			checkDefined(migrationProbability, "Migration_probability", configuration);
			checkDefined(numberOfIndividualsToMigrate, "Number_Of_Individuals_To_Migrate", configuration);
			checkDefined(maxGlobalFrontSize, "Max_front_size", configuration);
			checkDefined(maxFinalSolutionSize, "Max_solution_size", configuration);
			checkDefined(sendResultsPerGeneration, "Send_results_per_generation", configuration);
			checkDefined(numberParallelExecutions, "Number_parallel_executions", configuration);

			// Chequear que al menos hay un algoritmo
			if (teamAlgorithms.size() == 0){
				configuration.setErrorParsingStr("0 configurations defined");
			}
			
			vector <pair <string, string> > algIndPairs;
			// Para cada configuración chequear que todo está bien definido y que se pueden
			// cargar los individuos y EAs
			for (unsigned int i = 0; i < teamAlgorithms.size(); i++){
				checkDefined(teamAlgorithms[i].critStop, "Type_Stopping_Criterion (Configuration)", configuration);
				checkDefined(teamAlgorithms[i].valueSC, "Value_Stopping_Criterion (Configuration)", configuration);
				checkDefined(teamAlgorithms[i].solutionSource, "Solution Source (Configuration)", configuration);
				checkDefined(teamAlgorithms[i].sharingGroup, "Sharing_Group (Configuration)", configuration);
				if (teamAlgorithms[i].individualName.size() == 0){
					configuration.setErrorParsingStr("Individual (Configuration) non-defined");
					YYABORT;
				}
				if (teamAlgorithms[i].migrationSelectorName.size() == 0){
					configuration.setErrorParsingStr("Migration Selector non-defined");
					YYABORT;
				}

				if (teamAlgorithms[i].exchangeSelectorName.size() == 0){
					configuration.setErrorParsingStr("Exchange Selector non-defined");
					YYABORT;
				}

				if (teamAlgorithms[i].mutationName.size() == 0){
					configuration.setErrorParsingStr("Mutation non-defined");
					YYABORT;
				}

				if (teamAlgorithms[i].crossoverName.size() == 0){
					configuration.setErrorParsingStr("Crossover non-defined");
					YYABORT;
				}

				if (teamAlgorithms[i].localSearchName.size() == 0){
					configuration.setErrorParsingStr("Local Search non-defined");
					YYABORT;
				}

				if (teamAlgorithms[i].configs.size() == 0){
					configuration.setErrorParsingStr("Algoritm without any configuration");
					YYABORT;
				}
				for (unsigned int j = 0; j < teamAlgorithms[i].configs.size(); j++){
					if (teamAlgorithms[i].configs[j].size() != teamAlgorithms[i].configs[0].size()){
						configuration.setErrorParsingStr("Not all configuration in the same algorithms have the same number of parameters");
						YYABORT;
					}
				}

				// Carga del operador de mutacion
				if (strcasecmp(teamAlgorithms[i].mutationName.c_str(), "individual_internal") != 0){
					vector<string> empty;
					Mutation *mutation = getMutation(configuration.getPluginPath(), teamAlgorithms[i].mutationName, empty, (myId == 0));
					if (mutation == NULL){
						configuration.setErrorParsingStr("Loading mutation operator");
						YYABORT;
					}
					teamAlgorithms[i].mutation = mutation;
				} else {
					teamAlgorithms[i].mutation = NULL;
				}

				// Carga del operador de crossover
				if (strcasecmp(teamAlgorithms[i].crossoverName.c_str(), "individual_internal") != 0){
					vector<string> empty;
					Crossover *crossover = getCrossover(configuration.getPluginPath(), teamAlgorithms[i].crossoverName, empty, (myId == 0));
					if (crossover == NULL){
						configuration.setErrorParsingStr("Loading Crossover operator");
						YYABORT;
					}
					teamAlgorithms[i].crossover = crossover;
				} else {
					teamAlgorithms[i].crossover = NULL;
				}

				// Carga del operador de busqueda local
				LocalSearch *ls = getLocalSearch(configuration.getPluginPath(), teamAlgorithms[i].localSearchName, teamAlgorithms[i].localSearchParams, (myId == 0));
				if (ls == NULL){
					configuration.setErrorParsingStr("LocalSearch module could not be loaded");
				} else {
					teamAlgorithms[i].localSearch = ls;
				}
				
        // Carga del plugin con la multiobjetivizacion
        if (teamAlgorithms[i].multiObjectivizationName.size() != 0) {
				  MultiObjectivization *mo = getMultiObjectivization(configuration.getPluginPath(), teamAlgorithms[i].multiObjectivizationName, teamAlgorithms[i].multiObjectivizationParams, (myId == 0));
				  if (mo == NULL){
            teamAlgorithms[i].multiObjectivization = NULL;
					  configuration.setErrorParsingStr("MultiObjectivization module could not be loaded");
				  } else {
					  teamAlgorithms[i].multiObjectivization = mo;
				  }
        }
        else {
          teamAlgorithms[i].multiObjectivization = NULL;
        }

				//Carga del individuo
				vector<string> params = splitString(teamAlgorithms[i].individualParams);
				void *hndl;
				Individual *ind = getInd(configuration.getPluginPath(), teamAlgorithms[i].individualName, params, (myId == 0));
				if (ind == NULL){
					configuration.setErrorParsingStr("Loading individual");
					YYABORT;
				}

				teamAlgorithms[i].ind = ind;

        if (teamAlgorithms[i].multiObjectivization != NULL) {
          teamAlgorithms[i].multiObjectivization->initiallization(teamAlgorithms[i].ind);
          teamAlgorithms[i].ind->setNumberOfObj(teamAlgorithms[i].ind->getNumberOfObj() + 1);
        }
				teamAlgorithms[i].ind->setMutOperator(teamAlgorithms[i].mutation);
				teamAlgorithms[i].ind->setCrossOperator(teamAlgorithms[i].crossover);
        if (teamAlgorithms[i].multiObjectivization != NULL) {
          teamAlgorithms[i].ind->setMultiObjectivizationPlugins(vector<MultiObjectivization *>(1, teamAlgorithms[i].multiObjectivization));
        }
				allIndividuals.push_back(teamAlgorithms[i].ind);

				if (strcasecmp(teamAlgorithms[i].migrationSelectorName.c_str(), "algorithm_internal") != 0){
					vector<string> empty;
					MigrationSelector *migSelector = getMigrationSelector(configuration.getPluginPath(), teamAlgorithms[i].migrationSelectorName, empty, (myId == 0));
					if (migSelector == NULL){
						configuration.setErrorParsingStr("Loading migration Selector");
						YYABORT;
					}
					teamAlgorithms[i].migrationSelector = migSelector;
				} else {
					teamAlgorithms[i].migrationSelector = NULL;
				}


				if (strcasecmp(teamAlgorithms[i].exchangeSelectorName.c_str(), "algorithm_internal") != 0){
					vector<string> empty;
					ExchangeSelector *migSelector = getExchangeSelector(configuration.getPluginPath(), teamAlgorithms[i].exchangeSelectorName, empty, (myId == 0));
					if (migSelector == NULL){
						configuration.setErrorParsingStr("Loading exchange Selector");
						YYABORT;
					}
					teamAlgorithms[i].exchangeSelector = migSelector;
				} else {
					teamAlgorithms[i].exchangeSelector = NULL;
				}

				algIndPairs.push_back(pair<string, string>(teamAlgorithms[i].algorithmStr, teamAlgorithms[i].individualName));
			}

			// Chequear que se fijo un nombre para el módulo de conversión y se puede cargar
			if (configuration.getConversionStr().size() == 0){
				configuration.setErrorParsingStr("conversion module not specified");
			} else {
				Conversion *conv = getConversion(configuration.getBaseDirectory() + "/" + configuration.getConversionStr(), configuration.getConversionParams(), algIndPairs, (myId == 0));
				if (conv == NULL){
					configuration.setErrorParsingStr("Conversion module could not be loaded");
				} else{
					conv->setExampleIndividuals(allIndividuals);
					configuration.setConversion(conv);
				}
			}

			// Chequear que se fijo el módulo de Score y se puede cargar
			if (configuration.getScoreStr().size() == 0){
				configuration.setErrorParsingStr("Score module not specified");
			} else {
				ScoreAlgorithm *score = getScoreAlgorithm(configuration.getPluginPath(), configuration.getScoreStr(), configuration.getScoreParams(), (myId == 0));
				if (score == NULL){
					configuration.setErrorParsingStr("Score module could not be loaded");
				} else {
					configuration.setScore(score);
				}
			}

			// Chequear que se fijo el modelo de MigrationTopology y se puede cargar
			if (configuration.getMigrationTopologyStr().size() == 0){
				configuration.setErrorParsingStr("MigrationTopology module not specified");
			} else {
				MigrationTopology *migTop = getMigrationTopology(configuration.getPluginPath(), configuration.getMigrationTopologyStr(), configuration.getMigrationTopologyParams(), (myId == 0));
				if (migTop == NULL){
					configuration.setErrorParsingStr("MigrationTopology module could not be loaded");
				} else {
					configuration.setMigrationTopology(migTop);
				}
			}

			// Chequear que se fijo el initPopIslandLoader 
			if (configuration.getInitPopIslandLoaderStr().size() == 0){
				configuration.setErrorParsingStr("InitPopIslandLoader module not specified");
			} else {
				InitPopIslandLoader *ipil = getInitPopIslandLoader(configuration.getPluginPath(), configuration.getInitPopIslandLoaderStr(), configuration.getInitPopIslandLoaderParams(), (myId == 0));
				if (ipil == NULL){
					configuration.setErrorParsingStr("InitPopIslandLoader module could not be loaded");
				} else {
					configuration.setInitPopIslandLoader(ipil);
				}
			}

			// Guardar los parámetros "globales"
			configuration.setExecutionModel(executionModel);
			configuration.setInitPercentOfIndividuals(initPercentOfIndividuals);
			configuration.setMigrationProbability(migrationProbability); 
			configuration.setNumberOfIndividualsToMigrate(numberOfIndividualsToMigrate);
			configuration.setMaxGlobalFrontSize(maxGlobalFrontSize);
			configuration.setMaxFinalSolutionSize(maxFinalSolutionSize);
			configuration.setSendResultsPerGeneration(sendResultsPerGeneration == 1);
			configuration.setTeamAlgorithms(teamAlgorithms);
			configuration.setNumberParallelExecutions(numberParallelExecutions);
		}
			
	;

// Conjunto de parametros globales
generalSection:
	generalSection lineaGeneralSection 
	|
	;

// Conjunto de algoritmos junto con sus configuraciones
algorithmSection: 
	algorithmSection defAlg
	| defAlg
	;

// Parametro global
lineaGeneralSection:
	EXECUTION_MODEL DOS_PUNTOS model_execution_list
	| CONVERSION DOS_PUNTOS STRING NEWLINE 
		{
			if (configuration.getConversionStr().size() != 0){
				configuration.setErrorParsingStr("Conversion redefined");
			} else {
				configuration.setConversionStr(stringValueConf);
			}
		}
	| SCORE DOS_PUNTOS STRING NEWLINE
		{
			if (configuration.getScoreStr().size() != 0){
				configuration.setErrorParsingStr("Score redefined");
			} else {
				configuration.setScoreStr(stringValueConf);
			}
		}
	| MIGRATION_TOPOLOGY DOS_PUNTOS STRING NEWLINE
		{
			if (configuration.getMigrationTopologyStr().size() != 0){
				configuration.setErrorParsingStr("Migration_Topology redefined");
			} else {
				configuration.setMigrationTopologyStr(stringValueConf);
			}
		}
	| INIT_POP_ISLAND_LOADER DOS_PUNTOS STRING NEWLINE
		{
			if (configuration.getInitPopIslandLoaderStr().size() != 0){
				configuration.setErrorParsingStr("InitPopIslandLoader redefined");
			} else {
				configuration.setInitPopIslandLoaderStr(stringValueConf);
			}
		}
	| OUTPUT_PATH DOS_PUNTOS STRING NEWLINE
		{
			if (configuration.getOutputPath().size() != 0){
				configuration.setErrorParsingStr("Output_path redefined");
			} else {
				configuration.setOutputPath(stringValueConf);
			}
		}
	| PLUGIN_PATH DOS_PUNTOS STRING NEWLINE
		{
			if (configuration.getPluginPath().size() != 0){
				configuration.setErrorParsingStr("Plugin_path redefined");
			} else {
				configuration.setPluginPath(stringValueConf);
			}
		}
	| CONVERSIONPARAMS DOS_PUNTOS STRING 
		{
			if (configuration.getConversionParams().size() != 0){
				configuration.setErrorParsingStr("Conversion params redefined");
			} else {
				configuration.setConversionParams(splitString(stringValueConf));
				if (configuration.getConversionParams().size() == 0){
					configuration.setErrorParsingStr("Conversion params bad syntax");
				}
			}
		}
	| SCOREPARAMS DOS_PUNTOS STRING 
		{
			if (configuration.getScoreParams().size() != 0){
				configuration.setErrorParsingStr("Score params redefined");
			} else {
				configuration.setScoreParams(splitString(stringValueConf));
				if (configuration.getScoreParams().size() == 0){
					configuration.setErrorParsingStr("Score params bad syntax");
				}
			}
		}
	| MIGRATION_TOPOLOGY_PARAMS DOS_PUNTOS STRING 
		{
			if (configuration.getMigrationTopologyParams().size() != 0){
				configuration.setErrorParsingStr("Migration_Topology params redefined");
			} else {
				configuration.setMigrationTopologyParams(splitString(stringValueConf));
				if (configuration.getMigrationTopologyParams().size() == 0){
					configuration.setErrorParsingStr("Migration_Topology params bad syntax");
				}
			}
		}
	| INIT_POP_ISLAND_LOADER_PARAMS DOS_PUNTOS STRING 
		{
			if (configuration.getInitPopIslandLoaderParams().size() != 0){
				configuration.setErrorParsingStr("Init_Pop_Island_Loader params redefined");
			} else {
				configuration.setInitPopIslandLoaderParams(splitString(stringValueConf));
				if (configuration.getInitPopIslandLoaderParams().size() == 0){
					configuration.setErrorParsingStr("Init_Pop_Island_Loader params bad syntax");
				}
			}
		}
	| INIT_PERCENT_OF_INDIVIDUALS DOS_PUNTOS NUMERO  NEWLINE { setCheckedValue(initPercentOfIndividuals, (realValueConf/100), "Init_Percent_Of_Individuals", configuration);  }
	| MIGRATION_PROBABILITY DOS_PUNTOS NUMERO NEWLINE { setCheckedValue(migrationProbability, realValueConf, "Migration_probability", configuration); }
	| NUMBER_OF_INDIVIDUALS_TO_MIGRATE DOS_PUNTOS NUMERO NEWLINE { setCheckedValue(numberOfIndividualsToMigrate, (int)realValueConf, "Number_of_individuals_to_migrate", configuration);  }
	| MAX_GLOBAL_FRONT_SIZE DOS_PUNTOS NUMERO NEWLINE { setCheckedValue(maxGlobalFrontSize, (int)realValueConf, "Max_front_size", configuration); }
	| MAX_FINAL_SOLUTION_SIZE DOS_PUNTOS NUMERO NEWLINE { setCheckedValue(maxFinalSolutionSize, (int)realValueConf, "Solution_size", configuration); }
	| SEND_RESULTS_PER_GENERATION DOS_PUNTOS STRING NEWLINE 
			{ 
				if (strcasecmp("yes", stringValueConf.c_str()) == 0){
					setCheckedValue(sendResultsPerGeneration, 1, "Send_results_per_generation", configuration); 
				} 
				else if (strcasecmp("no", stringValueConf.c_str()) == 0){
					setCheckedValue(sendResultsPerGeneration, 0, "Send_results_per_generation", configuration);
				} else {
					configuration.setErrorParsingStr("Send_results_per_generation: not recognized value");
				}
			}
	| NUMBER_PARALLEL_EXECUTIONS DOS_PUNTOS NUMERO NEWLINE { setCheckedValue(numberParallelExecutions, (int)realValueConf, "Number_parallel_executions", configuration); } 
	;

// Modelo de ejecucion
// comprueba que el modelo esté bien definido
model_execution_list:
	model_execution_list CORCHETE_ABIERTO STRING COMA NUMERO COMA 
		{
			nextModel.stopCrit = Configuration::getGlobalTypeStoppingCriterion(stringValueConf); 
			nextModel.valueStopCrit = realValueConf; 
		}  
	STRING CORCHETE_CERRADO
		{ 
			nextModel.typeSelection = Configuration::getTypeSelection(stringValueConf);
			if (!insertNextModel(configuration)){
				configuration.setErrorParsingStr("Execution model incorrect");
			}
		}
	| model_execution_list CORCHETE_ABIERTO STRING CORCHETE_CERRADO 
			{ 
				nextModel.stopCrit = Configuration::getGlobalTypeStoppingCriterion(stringValueConf);
				if (!insertNextModel(configuration)){
					configuration.setErrorParsingStr("Execution model incorrect");
				}
			}
  | CORCHETE_ABIERTO STRING COMA NUMERO COMA 
			{ 
				nextModel.stopCrit = Configuration::getGlobalTypeStoppingCriterion(stringValueConf); 
				nextModel.valueStopCrit = realValueConf; 
			} 
		STRING CORCHETE_CERRADO	
			{ 
				nextModel.typeSelection = Configuration::getTypeSelection(stringValueConf); 
				if (!insertNextModel(configuration)) { 
					configuration.setErrorParsingStr("Execution model incorrect");
				} 
			}
  | CORCHETE_ABIERTO STRING CORCHETE_CERRADO
			{ 
				nextModel.stopCrit = Configuration::getGlobalTypeStoppingCriterion(stringValueConf);
				if (!insertNextModel(configuration)){
					configuration.setErrorParsingStr("Execution model incorrect");
				}
			}

// Configuracion
defAlg:
	HeaderConfiguration RestConfiguration 
	;

// Nombre del algoritmo
HeaderConfiguration:
	ALGORITHM DOS_PUNTOS STRING NEWLINE 
		{ 
			TAlgorithm nuevo; 
			nuevo.algorithmStr = stringValueConf;
			teamAlgorithms.push_back(nuevo);
		}
	;

// Individuo y parametros del algoritmo
RestConfiguration:
	RestConfiguration RestConfigurationLine
	| RestConfigurationLine
	;

// Linea con el individuo o parámetros del algoritmo
RestConfigurationLine:
	TYPE_STOPPING_CRITERION DOS_PUNTOS STRING NEWLINE 
		{ 
			setCheckedValue(teamAlgorithms[teamAlgorithms.size() - 1].critStop, EA::getTypeStoppingCriterion(stringValueConf), "Type_Stopping_Criterion (Algorithm)", configuration); 
		}
	| VALUE_STOPPING_CRITERION DOS_PUNTOS NUMERO NEWLINE { setCheckedValue(teamAlgorithms[teamAlgorithms.size() - 1].valueSC, realValueConf, "Value_Stopping_Criterion (Algorithm)", configuration); }
	| SOLUTION_SOURCE DOS_PUNTOS STRING NEWLINE // algorithm_internal o archive
		{
			setCheckedValue(teamAlgorithms[teamAlgorithms.size() -1].solutionSource, Configuration::getSolutionSource(stringValueConf), "Solution_Source (Algorithm)", configuration); 
		}
	| INDIVIDUAL DOS_PUNTOS STRING NEWLINE 
			{
				if (teamAlgorithms[teamAlgorithms.size()-1].individualName.length() != 0){
					configuration.setErrorParsingStr("Individual redefinition");
				}
				teamAlgorithms[teamAlgorithms.size()-1].individualName = stringValueConf;
			}
	| INDIVIDUALPARAMS DOS_PUNTOS STRING 
			{
				if (teamAlgorithms[teamAlgorithms.size()-1].individualParams.length() != 0){
					configuration.setErrorParsingStr("IndividualParams redefinition");
				}
				teamAlgorithms[teamAlgorithms.size()-1].individualParams = stringValueConf;
			}
	| MAX_LOCAL_FRONT_SIZE DOS_PUNTOS NUMERO NEWLINE 
			{
				if (teamAlgorithms[teamAlgorithms.size()-1].maxLocalFrontSize != 0){
					configuration.setErrorParsingStr("MaxLocalFrontSize redefined");
				}
				teamAlgorithms[teamAlgorithms.size()-1].maxLocalFrontSize = (int)realValueConf;
			}
	| SHARING_GROUP DOS_PUNTOS NUMERO NEWLINE 
			{
				if (teamAlgorithms[teamAlgorithms.size()-1].sharingGroup != -1){
					configuration.setErrorParsingStr("Sharing_Group redefined");
				}
				teamAlgorithms[teamAlgorithms.size()-1].sharingGroup = (int)realValueConf;
			}
	| MIGRATION_SELECTOR DOS_PUNTOS STRING NEWLINE
		{
			if (teamAlgorithms[teamAlgorithms.size()-1].migrationSelectorName.size() != 0){
				configuration.setErrorParsingStr("Migration_Selector redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].migrationSelectorName = stringValueConf;
		}
	| MUTATION DOS_PUNTOS STRING NEWLINE
		{
			if (teamAlgorithms[teamAlgorithms.size()-1].mutationName.size() != 0){
				configuration.setErrorParsingStr("Mutation redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].mutationName = stringValueConf;
		}
	| CROSSOVER DOS_PUNTOS STRING NEWLINE
		{
			if (teamAlgorithms[teamAlgorithms.size()-1].crossoverName.size() != 0){
				configuration.setErrorParsingStr("Crossover redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].crossoverName = stringValueConf;
		}
	| LOCAL_SEARCH DOS_PUNTOS STRING NEWLINE
		{
			if (teamAlgorithms[teamAlgorithms.size()-1].localSearchName.size() != 0){
				configuration.setErrorParsingStr("Local Search redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].localSearchName = stringValueConf;
		}
	| LOCAL_SEARCH_PARAMS DOS_PUNTOS STRING 
			{
				if (teamAlgorithms[teamAlgorithms.size()-1].localSearchParams.size() != 0){
					configuration.setErrorParsingStr("Local_SearchParams redefined");
				}
				teamAlgorithms[teamAlgorithms.size()-1].localSearchParams = splitString(stringValueConf);
			}
	| MULTIOBJECTIVIZATION DOS_PUNTOS STRING NEWLINE
		{
			if (teamAlgorithms[teamAlgorithms.size()-1].multiObjectivizationName.size() != 0){
				configuration.setErrorParsingStr("MultiObjectivization redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].multiObjectivizationName = stringValueConf;
		}
	| MULTIOBJECTIVIZATION_PARAMS DOS_PUNTOS STRING
			{
				if (teamAlgorithms[teamAlgorithms.size()-1].multiObjectivizationParams.size() != 0){
					configuration.setErrorParsingStr("MultiObjectivizationParams redefined");
				}
				teamAlgorithms[teamAlgorithms.size()-1].multiObjectivizationParams = splitString(stringValueConf);
			}
	| EXCHANGE_SELECTOR DOS_PUNTOS STRING NEWLINE
		{
			if (teamAlgorithms[teamAlgorithms.size()-1].exchangeSelectorName.size() != 0){
				configuration.setErrorParsingStr("Exchange_Selector redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].exchangeSelectorName = stringValueConf;
		}

	|	set_string_numeros NEWLINE 
			{
				teamAlgorithms[teamAlgorithms.size()-1].configs.push_back(stringBufferConf);
				stringBufferConf.clear();
			}
  ;

set_string_numeros:
	set_string_numeros STRING { stringBufferConf.push_back(stringValueConf); }
	| set_string_numeros NUMERO { stringstream tmp; tmp << realValueConf; stringBufferConf.push_back(tmp.str()); }
	| STRING { stringBufferConf.push_back(stringValueConf); }
	| NUMERO { stringstream tmp; tmp << realValueConf; stringBufferConf.push_back(tmp.str()); }
	;

//set_numeros:
//	set_numeros NUMERO { realBufferConf.push_back(realValueConf); }
//	| NUMERO { realBufferConf.push_back(realValueConf); } 
//	;

%%

void Conferror(Configuration &configuration, int myId, char *s) {
	configuration.setErrorParsingStr("Unrecognized format in Configuration File");
}

// Añadir modelo de ejecucion
bool insertNextModel(const Configuration &configuration){
	if (nextModel.stopCrit == Configuration::getGlobalTypeStoppingCriterion("CLEARSTATISTICS")){
 		executionModel.push_back(nextModel);
		return true;
	}
	if ((nextModel.stopCrit == -1) || (nextModel.typeSelection == -1) || (nextModel.valueStopCrit <= 0)){
		return false;
	}
	executionModel.push_back(nextModel);
	realExecutionModel = true;
	return true;
}

// Resetear a valores no válidos
void reset(){
	teamAlgorithms.clear();
	executionModel.clear();
	realExecutionModel = false;
	initPercentOfIndividuals = -1;
	migrationProbability = -1;
	numberOfIndividualsToMigrate = -1;
	maxGlobalFrontSize = -1;
	maxFinalSolutionSize = -1;
	parSize = -1;
}
