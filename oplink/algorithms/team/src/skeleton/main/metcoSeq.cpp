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

#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sstream>

#include <time.h>
#include <algorithm>
#include <fstream>
#include <future>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include "Crossover.h"
#include "Decomposition.h"
#include "EA.h"
#include "Individual.h"
#include "LocalSearch.h"
#include "MOFrontVector.h"
#include "MultiObjectivization.h"
#include "Mutation.h"
#include "OutputPrinter.h"
#include "ScoreAlgorithm.h"
#include "matplotlibcpp.h"

#define MINIMUM_ARGS 11
#define ARG_OUTPUTPATH 1
#define ARG_PLUGINPATH 2
#define ARG_OUTPUTPRINTERMODULE 3
#define ARG_OUTPUTFILE 4
#define ARG_ALGORITHM 5
#define ARG_PROBLEM 6
#define ARG_CRITSTOP 7
#define ARG_CRITSTOPVALUE 8
#define ARG_PRINTPERIOD 9
#define ARG_EXTARC 10
#define ARG_MAXLOCFRONT 11
#define ARG_REPETITIONS 11

namespace plt = matplotlibcpp;

const char PROBLEM = '!';
const char SCORE = '_';
const char MUTATION_CROSSOVER = '-';
const char LOCAL_SEARCH = '$';
const char MULTIOBJECTIVIZATION = '+';
const char DECOMPOSITION = '%';
const char SEPARATOR = '-';
const char COMMA = ',';

const string EXTENSION = ".mrf";
const string CSV_HEADER = "Algorithm,Avg,Min,Max,Std\n";
const int PLOT_WIDTH = 1200;
const int PLOT_HEIGHT = 700;

void argumentError(char *programName) {
    cout << "Correct usage: " << programName
         << " outputPath pluginPath outputPrinterModule outputFile algoritmo "
            "problema critStop critStopValue printPeriod useExternalArchive(0 "
            "| (1 maxLocalFrontSize)) repetitions [parametros_algoritmo] [! "
            "parametros_problema] [_ scoreModule paramsScoreModule] [- "
            "Mutation Crossover ] $ LocalSearch paramsLocalSearch [ + "
            "MultiObjectivizationPlugin paramsMultiObjectivization ] [ % "
            "Decomposition ]"
         << endl;
    exit(-1);
}

int main(int argc, char *argv[]) {
    // Llamada correcta al programa
    if (argc < MINIMUM_ARGS + 1) {
        argumentError(argv[0]);
    }

    initRandomSeed();

    // Initialize random generator
    string outputPath = argv[ARG_OUTPUTPATH];
    string pluginPath = argv[ARG_PLUGINPATH];
    string outputFilename = argv[ARG_OUTPUTFILE];
    string printerModule = argv[ARG_OUTPUTPRINTERMODULE];

    // Initialize parameters
    char *critStop = argv[ARG_CRITSTOP];
    int critStopId = EA::getTypeStoppingCriterion(critStop);
    if (critStopId == -1) {
        cerr << "Incorrect stopping criterion: " << critStop << endl;
        exit(-1);
    }

    double critStopValue = atof(argv[ARG_CRITSTOPVALUE]);
    int printPeriod = atoi(argv[ARG_PRINTPERIOD]);
    bool useExternalArchive = (atoi(argv[ARG_EXTARC]) == 1);
    int repetitions = atoi(argv[ARG_REPETITIONS]);

    int algParamIndex;
    int maxLocalFrontSize;

    if (useExternalArchive) {
        if (argc == MINIMUM_ARGS + 1) argumentError(argv[0]);
        maxLocalFrontSize = atoi(argv[ARG_MAXLOCFRONT]);
        algParamIndex = MINIMUM_ARGS + 2;
    } else {
        maxLocalFrontSize = 0;
        algParamIndex = MINIMUM_ARGS + 1;
    }

    // B�squeda de los par�metros del algoritmo
    vector<string> algorithmArgs;
    stringstream algArgsStream;
    int actualArg;
    for (actualArg = algParamIndex; actualArg < argc; actualArg++) {
        if ((argv[actualArg][0] == PROBLEM) || (argv[actualArg][0] == SCORE) ||
            (argv[actualArg][0] == MUTATION_CROSSOVER) ||
            (argv[actualArg][0] == LOCAL_SEARCH) ||
            (argv[actualArg][0] == DECOMPOSITION)) {
            break;
        }
        algorithmArgs.push_back(argv[actualArg]);
        algArgsStream << argv[actualArg] << SEPARATOR;
    }

    // B�squeda de los par�metros del problema
    vector<string> problemArgs;
    stringstream probArgsStream;
    if (argv[actualArg][0] == PROBLEM) {
        actualArg++;
        for (; actualArg < argc; actualArg++) {
            if ((argv[actualArg][0] != SCORE) &&
                (argv[actualArg][0] != MUTATION_CROSSOVER) &&
                (argv[actualArg][0] != LOCAL_SEARCH)) {
                problemArgs.push_back(argv[actualArg]);
                probArgsStream << argv[actualArg] << SEPARATOR;
            } else {
                break;
            }
        }
    }

    // Modulo score
    ScoreAlgorithm *score = NULL;
    vector<string> scoreArgs;
    if (argv[actualArg][0] == SCORE) {
        actualArg++;
        if (actualArg < argc) {
            string scoreModuleName = argv[actualArg++];
            for (; actualArg < argc; actualArg++) {
                if (argv[actualArg][0] != MUTATION_CROSSOVER) {
                    scoreArgs.push_back(argv[actualArg]);
                } else {
                    break;
                }
            }
            score =
                getScoreAlgorithm(pluginPath, scoreModuleName, scoreArgs, true);
            if (score == NULL) {
                cerr << "Error loading Score plugin" << endl;
                exit(-1);
            }
        }
    }

    Mutation *mut = NULL;
    Crossover *cross = NULL;
    vector<string> mutationArgs, crossoverArgs;
    if (argv[actualArg][0] == '-') {
        actualArg++;
        // Cargamos mutation
        if (actualArg < argc) {
            if (strcmp(argv[actualArg], "internal") != 0) {
                mut = getMutation(pluginPath, argv[actualArg], mutationArgs,
                                  true);
            }
        }
        actualArg++;
        // Cargamos crossover
        if (actualArg < argc) {
            if (strcmp(argv[actualArg], "internal") != 0) {
                cross = getCrossover(pluginPath, argv[actualArg], crossoverArgs,
                                     true);
            }
        }
        actualArg++;
    }

    // Modulo LocalSearch
    LocalSearch *ls = NULL;
    vector<string> localSearchArgs;
    if (argv[actualArg][0] == LOCAL_SEARCH) {
        actualArg++;
        // Cargamos local Search
        if (actualArg < argc) {
            string localSearchModuleName = argv[actualArg++];
            for (; actualArg < argc; actualArg++) {
                if (argv[actualArg][0] == MULTIOBJECTIVIZATION ||
                    argv[actualArg][0] == DECOMPOSITION) {
                    break;
                }
                localSearchArgs.push_back(argv[actualArg]);
            }
            ls = getLocalSearch(pluginPath, localSearchModuleName,
                                localSearchArgs, true);
            if (ls == NULL) {
                cerr << "Error loading LocalSearch plugin" << endl;
                exit(-1);
            }
        }
    }

    // Plugins de MultiObjetivizacion
    MultiObjectivization *mo = NULL;
    vector<string> multiObjectivizationArgs;
    if ((actualArg < argc) && (argv[actualArg][0] == MULTIOBJECTIVIZATION)) {
        actualArg++;
        if (actualArg < argc) {
            string multiObjectivizationName = argv[actualArg++];
            for (; actualArg < argc; actualArg++) {
                multiObjectivizationArgs.push_back(argv[actualArg]);
                if (argv[actualArg][0] == DECOMPOSITION) {
                    break;
                }
            }
            mo = getMultiObjectivization(pluginPath, multiObjectivizationName,
                                         multiObjectivizationArgs, true);
            if (mo == NULL) {
                cerr << "Error loading MultiObjectivization plugin" << endl;
                exit(-1);
            }
        }
    }

    Decomposition *decomposition = nullptr;
    vector<string> decompositionArgs;
    if ((actualArg < argc) && (argv[actualArg][0] == DECOMPOSITION)) {
        actualArg++;
        if (actualArg < argc) {
            string decompositionName = argv[actualArg++];
            for (; actualArg < argc; actualArg++) {
                decompositionArgs.push_back(argv[actualArg]);
            }
            decomposition = getDecomposition(pluginPath, decompositionName,
                                             decompositionArgs, true);
            if (decomposition == nullptr) {
                cerr << "Error loading Decomposition plugin" << endl;
                exit(-1);
            }
        }
    }
    // Cargamos individuo
    string problemName = argv[ARG_PROBLEM];
    Individual *ind = getInd(pluginPath, argv[ARG_PROBLEM], problemArgs, true);
    if (ind == NULL) {
        cerr << "Error loading individual" << endl;
        exit(-1);
    }

    // MultiObjetivizar individuo
    if (mo != NULL) {
        mo->initiallization(ind);
        ind->setNumberOfObj(ind->getNumberOfObj() + 1);
        ind->setMultiObjectivizationPlugins(
            vector<MultiObjectivization *>(1, mo));
    }

    ind->setMutOperator(mut);
    ind->setCrossOperator(cross);

    // Initiate the corresponding EA
    string algorithmName = argv[ARG_ALGORITHM];
    EA *ga = getEA(pluginPath, argv[ARG_ALGORITHM], algorithmArgs, true, ind);
    if (ga == NULL) {
        cerr << "Error loading EA" << endl;
        exit(-1);
    }
    if ((mo != NULL) && (!ga->supportsMultiObjectivization())) {
        cerr << "Error: selected GA does not support multiObjectivization"
             << endl
             << flush;
        exit(-1);
    }
    ga->setStoppingCriterion(critStopId, critStopValue);
    ga->setPrintPeriod(printPeriod);
    ga->setMultiObjectivizationPlugins(vector<MultiObjectivization *>(1, mo));
    ga->setDecomposition(decomposition);
    if (ind->getIndexObj() == -1) {
        ga->setGenerateArchive(useExternalArchive, ARCHIVE_VECTOR);
    } else {
        ga->setGenerateArchive(useExternalArchive, ARCHIVE_INSO1);
    }
    ga->setMaxLocalFrontSize(maxLocalFrontSize);
    ga->setScoreAlgorithm(score);
    ga->setLocalSearch(ls);

    outputFile += "_" + to_string(rep) + EXTENSION;
    vector<string> outputPrinterParams(1, (outputPath + "/" + outputFile));
    OutputPrinter *outputPrinter =
        getOutputPrinter(pluginPath, printerModule, outputPrinterParams, true);

    if (outputPrinter == NULL) {
        cout << "OutputPrinter could not be loaded" << endl;
        exit(-1);
    }

    ga->setOutputPrinter(outputPrinter);
    outputPrinter->printInit(ga);

    // Runs the evolurionary process
    ga->run();
    MOFront *p = new MOFrontVector(ga->getSampleInd(), false, false);

    // Program Output
    outputPrinter->printSolution(ga, true);
    outputPrinter->finish();

    // Finishing
    delete (ga);
    delete (ind);
    exit(EXIT_SUCCESS);
}
