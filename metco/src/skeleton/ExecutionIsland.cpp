/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 *
**********************************************************************************/

#include "mpi.h"
#include "MOFrontVector.h"
#include "Configuration.h"
#include "Individual.h"
#include "MigrationTopology.h"
#include "ExecutionIsland.h"
#include "LocalScoreAlgorithm.h"
#include "InitPopIslandLoader.h"
#include <string>
#include <time.h>
#include <sys/time.h>
#include <math.h>

// Constructor
ExecutionIsland::ExecutionIsland(const int slaveIslands, const Configuration &configuration){
	// Inicializacion buffers de envio y recepcion
	this->tamBuffer = DEF_SIZE_BUF;
	buffer = new long double[DEF_SIZE_BUF];

	// Datos de configuracion de la migracion

	this->pMigration = configuration.getMigrationProbability();
	this->maxIndToMigrate = configuration.getNumberIndMigrate();

	MPI_Comm_rank(MPI_COMM_WORLD, &myId);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

	this->slaveIslands = slaveIslands;
	numberOfProcessesPerExecution = numProcs / configuration.getNumberParallelExecutions();
	myMaster = (myId / numberOfProcessesPerExecution) * numberOfProcessesPerExecution;
	for (int i = myMaster + 1; i < myMaster + numberOfProcessesPerExecution; i++){
		if (i != myId){
			myPartners.push_back(i);
		}
	}

	//Crear grupo con islas de ejecucion
	MPI_Group all;
	MPI_Comm_group(MPI_COMM_WORLD, &all);
	int elementZero = 0;
	MPI_Group executionIslandGroup;
	MPI_Group_excl(all, 1, &elementZero, &executionIslandGroup);
	MPI_Comm_create(MPI_COMM_WORLD, executionIslandGroup, &executionIslandComm);

	//Testing synchronous
	completeGenerations = configuration.getExecutionModel()[0].valueStopCrit / configuration.getTeamAlgorithms()[0].valueSC / (numProcs - 1);

}

// Destructor
ExecutionIsland::~ExecutionIsland(){
	delete [] buffer;
}

// Ejecucion de las islas esclavas
void ExecutionIsland::run(const Configuration &configuration){
	double finalScore = 0;
	double migrationImprovement = 0;
	double *restartInfo = NULL;
	MPI_Status status;
	EA *algCode = NULL;
	vector<Individual *> lastPopulation, lastPopulationConverted;
	ScoreAlgorithm *scoreAlgorithm = configuration.getScoreAlgorithm();
	InitPopIslandLoader *ipil = configuration.getInitPopIslandLoader();
	MigrationTopology *migrationTopology = configuration.getMigrationTopology();
	const vector<TAlgorithm> &algorithms = configuration.getTeamAlgorithms();


	// Inicializaciones
	int islandInfo[2];
	int evaluations, lastEvaluationsSend;
	int maxLocalSolutionSize;

	int size;
	int lastAlg = -1;
	int lastConf = -1;
	MOFront *algSolution = NULL;
	initRandomSeed(myId);

	//Bucle principal: finaliza cuando el coordinador envia el comando de finalización
	MigrationSelector *migrationSelector = NULL;
	ExchangeSelector *exchangeSelector = NULL;
	vector < Individual *> *globalPop = NULL;
	vector < Individual *> *previousInitPop = NULL;
	vector < Individual *> *previousEndPop = NULL;
	if (ipil->usePreviousInit()){
		previousInitPop = new vector <Individual *>();
	}
	if (ipil->usePreviousEnd()){
		previousEndPop = new vector <Individual *>();
	}
	if (ipil->useGlobal()){
		globalPop = new vector <Individual *>();
	}
	bool generatingArchive = false;
	int numExecution = 0;

	while(1){
		double initScore = 0;
		//Receive the number of the algorithm-config to execute 
		//MPI_Probe(0, ALG_TAG, MPI_COMM_WORLD, &status);
		//MPI_Get_count(&status,MPI_DOUBLE, &size);
		if (tamBuffer < 2){
			delete [] buffer;
			buffer = new long double[2];
			tamBuffer = 2;
		}
		MPI_Recv(buffer, 2, MPI_LONG_DOUBLE, myMaster, ALG_TAG, MPI_COMM_WORLD, &status);
		numExecution++;
		// buffer[0] = Algoritmo || ALG_FINISH
		// buffer[1] = IdConf
		// buffer[2]...buffer[n] = Parametros de la configuración
		int newAlgId = (int)buffer[0];
		int newConfigId = (int)buffer[1];

		// Comprobar condicion de parada global
		if (newAlgId == ALG_FINISH) {
			delete algCode;
			break;
		}

		// Inicializa la configuracion a ejecutar
		if ((newAlgId != lastAlg) || (newConfigId != lastConf)) { //configuracion nueva
			delete algCode;
			algCode = getEA(configuration.getPluginPath(), algorithms[newAlgId].algorithmStr, algorithms[newAlgId].configs[newConfigId], true, algorithms[newAlgId].ind);
			if (algCode == NULL){
				cerr << "Error initiation EA" << endl;
				MPI_Abort(MPI_COMM_WORLD, 1);
				exit(-1);
			}
			algCode->setLocalSearch(algorithms[newAlgId].localSearch);
			if (algorithms[newAlgId].multiObjectivization != NULL) {
			  algCode->setMultiObjectivizationPlugins(vector<MultiObjectivization *>(1, algorithms[newAlgId].multiObjectivization));
		  }
			algCode->setIdConf(newAlgId);
			algCode->setStoppingCriterion(algorithms[newAlgId].critStop, algorithms[newAlgId].valueSC);
			algCode->setMaxLocalFrontSize(algorithms[newAlgId].maxLocalFrontSize);
			migrationSelector = algorithms[newAlgId].migrationSelector;
			exchangeSelector = algorithms[newAlgId].exchangeSelector;
			generatingArchive = (migrationSelector->getType() == SELECT_FROM_ARCHIVE) || (algorithms[newAlgId].solutionSource == SOURCE_ARCHIVE);
			algCode->setGenerateArchive(generatingArchive, ARCHIVE_VECTOR);
			delete algSolution;
			algSolution = new MOFrontVector(algCode->getSampleInd(), false, false);

      if (configsRestartInfo.count(algorithms[newAlgId].sharingGroup) > 0) {
        algCode->setRestartInfo(configsRestartInfo[algorithms[newAlgId].sharingGroup]);
        cout << "Información restaurada para la config " << newAlgId << " desde sharing group " << algorithms[newAlgId].sharingGroup << endl;
      }
    }

		//Limpia la solucion anterior
		if (generatingArchive){
			algCode->getLocalSolution()->clear();
		}
		//Resetea la condicion de parada
		algCode->resetStopConditions();
		lastEvaluationsSend = 0;
    
		//send info about the island
		islandInfo[0] = algCode->getPopulationSize();
		islandInfo[1] = algCode->getEvaluations();//number of evaluations
		MPI_Send(islandInfo, 2, MPI_INT, myMaster, ISLAND_INFO, MPI_COMM_WORLD); 
		Individual *ejInd = algorithms[newAlgId].ind;

		// Poblacion inicial
		
		if (ipil->useGlobal()){// Si se utiliza la global se recibe
			globalPop->clear();
			// Receive the initial individuals
			MPI_Probe(myMaster, INIT_IND, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status,MPI_LONG_DOUBLE, &size);
			if (size > tamBuffer){
				delete [] buffer;
				buffer = new long double[size];
				tamBuffer = size;
			}
			MPI_Recv(buffer, size, MPI_LONG_DOUBLE, myMaster, INIT_IND, MPI_COMM_WORLD, &status);

	
			// Fill the initial population
			int count = 4, leidos;
			int nInds = (int) buffer[0];
			int nVars = (int) buffer[1];
			int nObj = (int) buffer[2];
			int nMig = (int) buffer[3];
			for (int i = 0; i < nInds; i++){
				Individual *in = ejInd->deserialize(nVars, nObj, nMig, &buffer[count], leidos);
				count += leidos;
				globalPop->push_back(in);
			}
			if (size != count){
				cerr << "Size y count no se corresponden: " << size << ", " << count << endl << flush;
				exit(-1);
			}
		}

		vector <Individual *> newInitPopulation;
		vector<int> origin;//Para las conversiones
		newInitPopulation.clear();
		ipil->loadIslandInitPop(lastAlg, lastConf, newAlgId, newConfigId, previousInitPop, previousEndPop, globalPop, newInitPopulation, origin);

		vector <Individual *> newInitPopulationConverted;
		//Convertimos los que necesiten conversion	
		for (int i = 0; i < newInitPopulation.size(); i++){
			if ((origin[i] == GLOBAL) || (lastAlg == newAlgId)){//ya esta convertido
				newInitPopulationConverted.push_back(newInitPopulation[i]);
			} else {
				newInitPopulationConverted.push_back(configuration.getConversion()->convert(newInitPopulation[i], lastAlg, newAlgId));
				delete (newInitPopulation[i]);
			}
		}
    
		algCode->fillPopWithEvaluatedInds(newInitPopulationConverted);

		if (scoreAlgorithm->getType() == LOCAL_SCORE_ALGORITHM){
			for (int i = 0; i < algCode->population->size(); i++){
				algSolution->insert((*(algCode->population))[i]);
			}
			initScore = ((LocalScoreAlgorithm *)(scoreAlgorithm))->score(algSolution);
			algSolution->clear();
		}

		//update previousInitPop
		if (ipil->usePreviousInit()){
			previousInitPop->clear();
			for (int i = 0; i < algCode->population->size(); i++){
				previousInitPop->push_back((*algCode->population)[i]->internalClone());
			}
		}

		// Almacenar la configuracion que se esta ejecutando
		lastAlg = newAlgId;
		lastConf = newConfigId;

		// Ejecutar algoritmo hasta que se llegue al criterio de parada local
		double at2 = 0;
		double at = 0;
		while(!algCode->hasFinished()){
			if (numExecution <= completeGenerations){
				MPI_Barrier(executionIslandComm);//Testing synchronous (only with evaluations)
			}
			algCode->runGenerationInc();
			if (numExecution <= completeGenerations){
				MPI_Barrier(executionIslandComm);//Testing synchronous (only with evaluations)
			}
			if (configuration.getSendResultsPerGeneration()){
				if (algorithms[newAlgId].solutionSource == SOURCE_ARCHIVE){
					algCode->getLocalSolution()->send(myMaster);//send the local solution to the coordinator
				} else {
					algCode->getSolution(algSolution);
					algSolution->send(myMaster);
					algSolution->clear();
				}
				evaluations = algCode->getPerformedEvaluations();
				evaluations -= lastEvaluationsSend;
				MPI_Send(&evaluations, 1, MPI_INT, myMaster, NUM_EVALUATIONS, MPI_COMM_WORLD);//send the number of evaluations
				lastEvaluationsSend = algCode->getPerformedEvaluations();
				int finishLocal = 0;
				MPI_Send(&finishLocal, 1, MPI_INT, myMaster, LOCAL_STOP, MPI_COMM_WORLD);
			}
			//migrate individuals
			if (slaveIslands > 1){
				if ((((double)rand()) / RAND_MAX) < pMigration){ 
					//liberar request anteriores si han terminado
					int freed = 1;
					while(freed && pendingRequests.size()){
						MPI_Status status;
						MPI_Test(pendingRequests.front().first, &freed, &status);
						if (freed){
							delete(pendingRequests.front().first);
							delete [] pendingRequests.front().second;
							pendingRequests.pop();
						}
					}
					vector<Individual*> migrate;
					if (migrationSelector == NULL){//algorithm_internal
						cout << "Selector de individuos a migrar interno al algoritmo no implementado" << endl;
						exit(-1);
					} else {
						vector<int> selected;
						if (migrationSelector->getType() == SELECT_FROM_ARCHIVE){
							int nInd = (algCode->getLocalSolution()->getFrontSize() >= maxIndToMigrate)?maxIndToMigrate:algCode->getLocalSolution()->getFrontSize();
							vector<Individual *> inds;
							algCode->getLocalSolution()->getAllIndividuals(inds);
							migrationSelector->select(inds, nInd, selected);
							for (int i = 0; i < selected.size(); i++){
								migrate.push_back(algCode->getLocalSolution()->getInd(selected[i]));
							}
						} else {
							int nInd = (algCode->population->size() >= maxIndToMigrate)?maxIndToMigrate:algCode->population->size();
							migrationSelector->select(*algCode->population, nInd, selected);
							for (int i = 0; i < selected.size(); i++)
								migrate.push_back((*algCode->population)[selected[i]]);
						}
					}

					if (migrate.size() != 0){
						vector<int> destination;
						int myIdShifted = myId % numberOfProcessesPerExecution;
						int shiftedValue = myId - myIdShifted;
						migrationTopology->sendTo(myIdShifted, slaveIslands, destination);
						for (int i = 0; i < destination.size(); i++){
							destination[i] += shiftedValue;
						}
						for (int destIndex = 0; destIndex < destination.size(); destIndex++){
							int nVars = migrate[0]->getNumberOfVar();
							int nObj = migrate[0]->getNumberOfObj();
							int nMig = migrate[0]->getNumberOfMigData();
							int tamInd = nVars + nObj + nMig;
							long double *migrationBuffer = new long double[5 + migrate.size()*(tamInd)];
							migrationBuffer[0] = migrate.size();
							migrationBuffer[1] = nVars;
							migrationBuffer[2] = nObj;
							migrationBuffer[3] = nMig;
							migrationBuffer[4] = algCode->getIdConf();
							int count = 5, leidos;
							for (unsigned int i = 0; i < migrate.size(); i++){
								migrate[i]->serialize(&migrationBuffer[count], leidos);
								count += leidos;
								if (leidos != tamInd){
									cerr << "Error: enviando individuos de distinto tamaño" << endl << flush;
									exit(-1);
								}
							}
							if (count != 5 + migrate.size()*(nVars + nObj + nMig)){
								cerr << "Los tam. no se corresponden: " << count << " y " << (5 + migrate.size()*(nVars + nObj + nMig)) << endl;
								exit(-1);
							}
							//Migracion no bloqueante: si no puede haber interbloqueo
							MPI_Request *myRequest = new MPI_Request;
							if ((destination[destIndex] == 0) || (destination[destIndex] == myId)){
								cout << "Error envia a 0, o a si mismo" << endl;
							}
							MPI_Isend(migrationBuffer, 5 + migrate.size()*tamInd, MPI_LONG_DOUBLE, destination[destIndex], MIGRATION, MPI_COMM_WORLD, myRequest);
							pendingRequests.push(pair<MPI_Request*, long double*>(myRequest, migrationBuffer));
						}
					}
				}
				
				//receive migrated individuals
				bool received = true;
				bool receivedAny = false;
				int flag, size;
				while (received){
					MPI_Iprobe(MPI_ANY_SOURCE, MIGRATION, MPI_COMM_WORLD, &flag, &status);
					if (flag){
						receivedAny = true;
						vector<Individual *> convertedInds;
						convertedInds.clear();
						MPI_Get_count(&status, MPI_LONG_DOUBLE, &size);
						if (size > tamBuffer){
							delete [] buffer;
							buffer = new long double[size];
							tamBuffer = size;
						}
						struct timeval t1;
						gettimeofday(&t1, NULL);
						MPI_Recv(buffer, size, MPI_LONG_DOUBLE, status.MPI_SOURCE, MIGRATION, MPI_COMM_WORLD, &status);
						struct timeval t2;
						gettimeofday(&t2, NULL);

						int nInd = (int) buffer[0];
						int nVars = (int) buffer[1];
						int nObj = (int) buffer[2];
						int nMig = (int) buffer[3];
						int idConf = (int) buffer[4];
						int count = 5, leidos;
						for (int i = 0; i < nInd; i++){
							Individual *newInd = ejInd->deserialize(nVars, nObj, nMig, &buffer[count], leidos);
							count += (leidos);
							if (idConf != algCode->getIdConf()){
								convertedInds.push_back(configuration.getConversion()->convert(newInd, idConf, algCode->getIdConf()));
								delete newInd;
							} else {
								convertedInds.push_back(newInd);
							}
						}
						if (size != count){
							cerr << "Size y count no se corresponden: " << size << ", " << count << endl << flush;
							exit(-1);
						}

						//Calculo del migration improvement
						//Consideramos la mejora que se obtendria al usar todos los individuos migrados
						if (scoreAlgorithm->getType() == LOCAL_SCORE_ALGORITHM){
							double previous = 0;
							algCode->getSolution(algSolution);
							previous = ((LocalScoreAlgorithm *)(scoreAlgorithm))->score(algSolution);
							double after = 0;
							for (int i = 0; i < convertedInds.size(); i++){
								algSolution->insert(convertedInds[i]);
							}
							after = ((LocalScoreAlgorithm *)(scoreAlgorithm))->score(algSolution);
							algSolution->clear();
							//en multiobjetivo da mas problemas que los que quita
							if (ejInd->getNumberOfObj() == 1){
								if (scoreAlgorithm->getDirection() == MAXIMIZE){
									migrationImprovement += after - previous;
								} else {
									migrationImprovement += previous - after;
								}
							}
						}
						
						//Exchange Individuals
						if (exchangeSelector == NULL){//exchangeSelector interno al algoritmo
							algCode->received(convertedInds);
						} else {
							exchangeSelector->exchange(convertedInds, *algCode->population);
						}
					} else{
						received = false;
					}
				}
				if (receivedAny){
					algCode->exchangePerformed();
				} 
			}
		}

		//Recuperacion del estado del algoritmo
		restartInfo = algCode->getRestartInfo();

    // Stores the state of the algorithm
    if (restartInfo) {
      if (configsRestartInfo.count(algorithms[newAlgId].sharingGroup) > 0) {
        delete [] configsRestartInfo[algorithms[newAlgId].sharingGroup];
        configsRestartInfo.erase(algorithms[newAlgId].sharingGroup);
        cout << "Informacion antigua borrada para la config " << newAlgId << " en sharing group " << algorithms[newAlgId].sharingGroup << endl;
      }
      configsRestartInfo[algorithms[newAlgId].sharingGroup] = restartInfo;
      cout << "Informacion nueva almacenada para la config " << newAlgId << " en sharing group " << algorithms[newAlgId].sharingGroup << endl;
    }
    
		if (algorithms[newAlgId].solutionSource == SOURCE_ARCHIVE){
			if (scoreAlgorithm->getType() == LOCAL_SCORE_ALGORITHM)
				finalScore = ((LocalScoreAlgorithm *)(scoreAlgorithm))->score(algCode->getLocalSolution());
			algCode->getLocalSolution()->send(myMaster);//send the local solution to the coordinator
		} else {
			algCode->getSolution(algSolution);
			if (scoreAlgorithm->getType() == LOCAL_SCORE_ALGORITHM)
				finalScore = ((LocalScoreAlgorithm *)(scoreAlgorithm))->score(algSolution);
			algSolution->send(myMaster);
			algSolution->clear();
		}

		// Si puntuacion local, enviamos la puntuacion (ineficiente, deberiamos unir envios)
		if (scoreAlgorithm->getType() == LOCAL_SCORE_ALGORITHM){
			long double newscores[2];
			newscores[0] = initScore;
			if (scoreAlgorithm->getDirection() == MAXIMIZE){
				newscores[1] = finalScore - migrationImprovement;
			} else {
				newscores[1] = finalScore + migrationImprovement;
			}
			migrationImprovement = 0;
			MPI_Send(&newscores, 2, MPI_LONG_DOUBLE, myMaster, LOCAL_SCORE, MPI_COMM_WORLD);
		}
		evaluations = algCode->getPerformedEvaluations();
		evaluations -= lastEvaluationsSend;
		MPI_Send(&evaluations, 1, MPI_INT, myMaster, NUM_EVALUATIONS, MPI_COMM_WORLD);//send the number of evaluations
		lastEvaluationsSend = algCode->getPerformedEvaluations();
    
		if (configuration.getSendResultsPerGeneration()){
			int finishLocal = 1;
			MPI_Send(&finishLocal, 1, MPI_INT, myMaster, LOCAL_STOP, MPI_COMM_WORLD);
		}

		//update previousEndPop
		if (ipil->usePreviousEnd()){
			previousEndPop->clear();
			for (int i = 0; i < algCode->population->size(); i++){
				previousEndPop->push_back((*algCode->population)[i]);
			}
		} else {
			for (int i = 0; i < algCode->population->size(); i++){
				delete((*algCode->population)[i]);
			}
		}
		algCode->population->clear();
	}

  // Free memory from the map configsRestartInfo
  for (std::map<int, double*>::iterator it = configsRestartInfo.begin(); it != configsRestartInfo.end(); it++) {
    delete [] it->second;
  }
  configsRestartInfo.clear();

	// Terminar los envios pendientes: ojo esto a veces falla con openmpi, creo que es pq
	// en openmpi si el q tiene q recibir no hace el receive, se queda esperando infinitamente
	//while(pendingRequests.size()){
	//	MPI_Status status;
	//	MPI_Wait(pendingRequests.front().first, &status);
	//	delete(pendingRequests.front().first);
	//	delete [] pendingRequests.front().second;
	//	pendingRequests.pop();
	//}
}
