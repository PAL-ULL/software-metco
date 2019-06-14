/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * DATE 
 *    November 2007
 *
 * DESCRIPTION
 * 	It represents the mater island. The master island is in charge of:
 * 		- Execute the Hyper-heuristic principles, in order to decide which
 * 		configurations are executed.
 * 		- Maintain the global solution
 * 		- Detect the stopping criterion and finalize the execution
 * *********************************************************************************/

#ifndef __COORDINATORISLAND_H__
#define __COORDINATORISLAND_H__

#include <queue>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <time.h>
#include <sys/time.h>
#include <float.h>

#include "Configuration.h"
#include "Individual.h"
#include "MOFront.h"
#include "ExecutionIsland.h"
#include "Definitions.h"

using namespace std;

// Tipo tarea
typedef struct {
	int indexAlg; // identificador algoritmo
	int config;   // identificador configuracion
} TTask;

// Tipo tarea en ejecucion (tarea + tiempo de inicio de ejecucion)
typedef struct {
	int    indexAlg;                      
	int    config; 
	double initTime;
} TExecIsland;

// Tipo estadistica para una tarea
typedef struct {
	int insert; // numero de soluciones insertadas en frente global
	int remove; // numero de soluciones dominadas del frente global
	int nEvaluations; // numero de evaluaciones hechas
	double runTime; // tiempo de ejecucion total
} TStatistics;

// Tipo puntuacion de una tarea
typedef struct {
	int    indexAlg;
	int    config;
	double mark;
} TScore;


class CoordinatorIsland {
public:
	// Constructor
	CoordinatorIsland (const string &outputFileName, const int printPeriod, const string &baseDirectory); 

	// Destructor
	~CoordinatorIsland (void);

	// Arrancar la ejecución paralela
	void run  (const char *file);
	
	// Getters
	Individual *getEjInd()                  const { return configuration.getTeamAlgorithms()[0].ind; }
	int inline getEvaluations()             const { return evaluations;                              }
	ostream &getOutputFile()                      { return outputFile;                               }
	const Configuration &getConfiguration() const { return configuration;                            }
	void getSolution(MOFront *p)            const ;

	// Funciones auxiliares 
	void printInfo (void);
	void printCharacteristic (const int numSlaves);
	void printStatistics (void); 

private:

	// Atributos
	Configuration configuration;
	int numProcs;
	int notUsedIslands;
	unsigned int actualModel;
	int myId;
	MOFront *globalSolution;    // Frente optimo de MOFront
	double startTime;         // Inicio de ejecucion
	int numExecutions;     // Numero de ejecuciones de configuraciones
	int printPeriod;
	int nextPrint;
	int evaluations;
	int evaluationsFinished;
	double valueStopCrit;
	ofstream outputFile;
	string outputFileName;
	queue<int> clearStatisticEvaluation;
	long double lastScore;

	list<TTask> taskQueue;     	    // Cola de tareas iniciales con todas
	list<TTask> circularTaskQueue;  // las configuraciones
						 
	vector< vector<TStatistics> > realStatistics, usedStatistics; // Estadistica de cada una de las configuraciones
	vector<int> lastEvaluations; // Número de evaluaciones que cada isla informa que va a hacer
	vector<int> evaluationsPerformed; 
	vector<TExecIsland> execIsland; // Que algoritmo se ejecuta en cada isla

	InitPopIslandLoader *ipil;

	int numberOfProcessesPerExecution;
	vector<int> mySlaves;

	// Atributos auxiliares
  static const int PRINTPERIOD_DEFAULT = 5000;
	
	// Metodos privados
	void init (const char *confFile);
	void initIslands ();
	void initAlgorithm (const int actAlg, const int actConf, const int idIsland);
	void selectionIsland (int &indexAlg, int &config, const int idle);
	bool stop ();
	bool stopActual ();
	void bodyCoordinator (const int numSlaves);	
	int recvLocalSolution ();
	void finish (const int numProcs);
	long double scoreConfiguration (const int indexAlg, const int config, const double rndValue, const int islandId);
	void clearStatistics(vector< vector<TStatistics> > &statistics);
	void checkPrintPeriod();
};

#endif
