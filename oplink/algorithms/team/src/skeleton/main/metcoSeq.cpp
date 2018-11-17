/* -----------------------------------------------------------------------------
 * Ejecuci�n aislada de un �nico EA.
 * Al programa se le pasa el EA que se quiere ejecutar, el problema que se
 * quiere resolver, el criterio de parada, la indicaci�n de si se debe crear
 * un frente de Pareto con los �ptimos encontrados durante toda la ejecuci�n
 * y los par�metros que se deber�n pasar al EA correspondiente.
 * Como salida proporciona el frente de Pareto obtenido durante la ejecuci�n,
 * y los puntos soluci�n (no tiene por qu� coincidir con el frente de Pareto
 * mostrado).
 * -------------------------------------------------------------------------- */

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <libgen.h>
#include <iostream>
#include <fstream>
#include <string.h>

#include "MOFrontVector.h"
#include "EA.h"
#include "Individual.h"
#include "ScoreAlgorithm.h"
#include "Mutation.h"
#include "Crossover.h"
#include "LocalSearch.h"
#include "OutputPrinter.h"
#include "MultiObjectivization.h"
#include "Decomposition.h"

#define MINIMUM_ARGS 10
#define ARG_OUTPUTPATH          1
#define ARG_PLUGINPATH          2
#define ARG_OUTPUTPRINTERMODULE 3
#define ARG_OUTPUTFILE          4
#define ARG_ALGORITHM           5
#define ARG_PROBLEM             6
#define ARG_CRITSTOP            7
#define ARG_CRITSTOPVALUE       8
#define ARG_PRINTPERIOD         9
#define ARG_EXTARC              10
#define ARG_MAXLOCFRONT         11

const char PROBLEM = '!';
const char SCORE = '_';
const char MUTATION_CROSSOVER = '-';
const char LOCAL_SEARCH = '$';
const char MULTIOBJECTIVIZATION = '+';
const char DECOMPOSITION = '%';

void argumentError (char *programName) {
	cout << "Correct usage: " << programName << " outputPath pluginPath outputPrinterModule outputFile algoritmo problema critStop critStopValue printPeriod useExternalArchive(0 | (1 maxLocalFrontSize)) [parametros_algoritmo] [! parametros_problema] [_ scoreModule paramsScoreModule] [- Mutation Crossover ] $ LocalSearch paramsLocalSearch [ + MultiObjectivizationPlugin paramsMultiObjectivization ] [ % Decomposition <direction>]" << endl;
 	exit (-1);
}

int main (int argc, char *argv[]) {

	//Llamada correcta al programa
	if (argc < MINIMUM_ARGS + 1){
		argumentError(argv[0]);
  }

  // IMPORTANTE!!!!!!!!!!!!!! Cuando se quieren hacer experimentos con diferentes inicializadores de poblacion
  // y pasando una semilla fija por parametro desde linea de comandos a MonoDE2 no debe hacerse uso de esta parte sino
  // de la parte de codigo de abajo
	initRandomSeed();

	// Initialize random generator
	string outputPath = argv[ARG_OUTPUTPATH];
	string pluginPath = argv[ARG_PLUGINPATH];

	vector<string> outputPrinterParams(1, (outputPath + "/" + argv[ARG_OUTPUTFILE]));
	OutputPrinter *outputPrinter = getOutputPrinter(pluginPath, argv[ARG_OUTPUTPRINTERMODULE], outputPrinterParams, true);
	if (outputPrinter == NULL){
		cout << "OutputPrinter could not be loaded" << endl;
		exit(-1);
	}

	// Initialize parameters
	char *critStop = argv[ARG_CRITSTOP];
	int critStopId = EA::getTypeStoppingCriterion(critStop);
	if (critStopId == -1){
		cerr << "Incorrect stopping criterion: " << critStop << endl;
		exit(-1);
	}
	double critStopValue = atof(argv[ARG_CRITSTOPVALUE]);
	int printPeriod      = atoi(argv[ARG_PRINTPERIOD]);
	bool useExternalArchive = (atoi(argv[ARG_EXTARC]) == 1);
	int algParamIndex;
	int maxLocalFrontSize;

	if (useExternalArchive){
		if (argc == MINIMUM_ARGS + 1)
			argumentError(argv[0]);
		maxLocalFrontSize = atoi(argv[ARG_MAXLOCFRONT]);
		algParamIndex = MINIMUM_ARGS + 2;
	} else{
		maxLocalFrontSize = 0;
		algParamIndex = MINIMUM_ARGS + 1;
	}

	//B�squeda de los par�metros del algoritmo
	vector<string> algorithmArgs;
	int actualArg;
	for (actualArg = algParamIndex; actualArg < argc; actualArg++){
		if ((argv[actualArg][0] == PROBLEM)
		|| (argv[actualArg][0] == SCORE)
		|| (argv[actualArg][0] == MUTATION_CROSSOVER)
		|| (argv[actualArg][0] == LOCAL_SEARCH)
		|| (argv[actualArg][0] == DECOMPOSITION)){
			break;
		}
		algorithmArgs.push_back(argv[actualArg]);
	}

  // IMPORTANTE!!!!!!!!!!!!!! Cuando se quieren hacer experimentos con diferentes inicializadores de poblacion
  // y pasando una semilla fija por parametro desde linea de comandos a MonoDE2 debe hacerse uso de esta parte y no
  // del initRandomSeed() de mas arriba

  // Sets a fixed seed
  /*string str1("Seed");
  for (int i = 0; i < algorithmArgs.size(); i++) {
      if (str1.compare(algorithmArgs[i].c_str()) == 0) {
        srand(atoi(algorithmArgs[i+2].c_str()));
      }
  }*/

	//B�squeda de los par�metros del problema
	vector<string> problemArgs;
	if (argv[actualArg][0] == PROBLEM){
		actualArg++;
		for (; actualArg < argc; actualArg++){
			if ((argv[actualArg][0] != SCORE)
			&& (argv[actualArg][0] != MUTATION_CROSSOVER)
			&& (argv[actualArg][0] != LOCAL_SEARCH)){
				problemArgs.push_back(argv[actualArg]);
			} else {
				break;
			}
		}
	}

	//Modulo score
	ScoreAlgorithm *score = NULL;
	vector<string> scoreArgs;
	if (argv[actualArg][0] == SCORE){
		actualArg++;
		if (actualArg < argc){
			string scoreModuleName = argv[actualArg++];
			for (; actualArg < argc; actualArg++){
				if (argv[actualArg][0] != MUTATION_CROSSOVER){
					scoreArgs.push_back(argv[actualArg]);
				} else {
					break;
				}
			}
			score = getScoreAlgorithm(pluginPath, scoreModuleName, scoreArgs, true);
			if (score == NULL){
				cerr << "Error loading Score plugin" << endl;
				exit(-1);
			}
		}
	}

	Mutation *mut = NULL;
	Crossover *cross = NULL;
	vector<string> mutationArgs, crossoverArgs;
	if (argv[actualArg][0] == '-'){
		actualArg++;
		//Cargamos mutation
		if (actualArg < argc){
			if (strcmp(argv[actualArg], "internal") != 0){
				mut = getMutation(pluginPath, argv[actualArg], mutationArgs, true);
			}
		}
		actualArg++;
		//Cargamos crossover
		if (actualArg < argc){
			if (strcmp(argv[actualArg], "internal") != 0){
				cross = getCrossover(pluginPath, argv[actualArg], crossoverArgs, true);
			}
		}
		actualArg++;
	}

	//Modulo LocalSearch
	LocalSearch *ls = NULL;
	vector<string> localSearchArgs;
	if (argv[actualArg][0] == LOCAL_SEARCH){
		actualArg++;
		//Cargamos local Search
		if (actualArg < argc){
			string localSearchModuleName = argv[actualArg++];
			for (; actualArg < argc; actualArg++){
				if (argv[actualArg][0] == MULTIOBJECTIVIZATION
				|| argv[actualArg][0] == DECOMPOSITION){
					break;
				}
				localSearchArgs.push_back(argv[actualArg]);
			}
			ls = getLocalSearch(pluginPath, localSearchModuleName, localSearchArgs, true);
			if (ls == NULL){
				cerr << "Error loading LocalSearch plugin" << endl;
				exit(-1);
			}
		}
	}

	//Plugins de MultiObjetivizacion
  MultiObjectivization *mo = NULL;
  vector<string> multiObjectivizationArgs;
  if ((actualArg < argc) && (argv[actualArg][0] == MULTIOBJECTIVIZATION)) {
    actualArg++;
    if (actualArg < argc){
      string multiObjectivizationName = argv[actualArg++];
      for (; actualArg < argc; actualArg++){
        multiObjectivizationArgs.push_back(argv[actualArg]);
		if(argv[actualArg][0] == DECOMPOSITION) {
			break;
		}
      }
      mo = getMultiObjectivization(pluginPath, multiObjectivizationName, multiObjectivizationArgs, true);
      if (mo == NULL){
        cerr << "Error loading MultiObjectivization plugin" << endl;
        exit(-1);
      }
    }
  }

  Decomposition* decomposition = nullptr;
  vector<string> decompositionArgs;
  if ((actualArg < argc) && (argv[actualArg][0] == DECOMPOSITION)) {
	actualArg++;
	if (actualArg < argc){
	  string decompositionName = argv[actualArg++];
	  for (; actualArg < argc; actualArg++){
		decompositionArgs.push_back(argv[actualArg]);
	  }
	  decomposition = getDecomposition(pluginPath, decompositionName, decompositionArgs, true);
	  if (decomposition == nullptr){
		cerr << "Error loading Decomposition plugin" << endl;
		exit(-1);
	  }
	}
  }
	//cargamos individuo
	Individual *ind = getInd(pluginPath, argv[ARG_PROBLEM], problemArgs, true);
	if (ind == NULL){
		cerr << "Error loading individual" << endl;
		exit(-1);
	}

	//MultiObjetivizar individuo
  if (mo != NULL) {
	  mo->initiallization(ind);
	  ind->setNumberOfObj(ind->getNumberOfObj() + 1);
		ind->setMultiObjectivizationPlugins(vector<MultiObjectivization *>(1, mo));
  }

	ind->setMutOperator(mut);
	ind->setCrossOperator(cross);

	/*for (int i = 0; i < ind->getNumberOfObj(); i++){
		cerr << ind->getInternalOptDirection(i) << endl;
	}*/

	// Initiate the corresponding EA
	EA *ga = getEA(pluginPath, argv[ARG_ALGORITHM], algorithmArgs, true, ind);
	if (ga == NULL){
		cerr << "Error loading EA" << endl;
		exit(-1);
	}
	if ((mo != NULL) && (!ga->supportsMultiObjectivization())){
		cerr << "Error: selected GA does not support multiObjectivization" << endl << flush;
		exit(-1);
	}
	ga->setStoppingCriterion(critStopId, critStopValue);
	ga->setPrintPeriod(printPeriod);
	ga->setMultiObjectivizationPlugins(vector<MultiObjectivization *>(1, mo));
	ga->setDecomposition(decomposition);
	if (ind->getIndexObj() == -1){
		ga->setGenerateArchive(useExternalArchive, ARCHIVE_VECTOR);
	} else {
		ga->setGenerateArchive(useExternalArchive, ARCHIVE_INSO1);
	}
	ga->setMaxLocalFrontSize(maxLocalFrontSize);
	ga->setScoreAlgorithm(score);
	ga->setLocalSearch(ls);
	ga->setOutputPrinter(outputPrinter);

	outputPrinter->printInit(ga);
	// Runs the evolurionary process
	ga->run();

	//cerr << "Termina run" << endl << flush;
	// Program Output
	outputPrinter->printSolution(ga, true);
	outputPrinter->finish();

	//cerr << "Termina de imprimir" << endl << flush;
	//Imprimimos el frente al final pero segun el ga
	//cout << "Solucion del ga" << endl;
	/*MOFront *solBorrar = new MOFrontVector(ind, true, true);
	ga->getSolution(solBorrar);
	cout << solBorrar;*/

	//Finishing
	delete (ga);
	delete (ind);
	exit(EXIT_SUCCESS);
}
