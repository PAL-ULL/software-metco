/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 ***********************************************************************************/

#include "mpi.h"
#include "MOFrontVector.h"
#include "CoordinatorIsland.h"
#include "Configuration.h"
#include "GlobalScoreAlgorithm.h"
#include "LocalScoreAlgorithm.h"
#include "utils.h"

#include <sstream>
#include <map>
#include <string>
#include <dlfcn.h> 
#include <sys/wait.h>

// Constructor
CoordinatorIsland::CoordinatorIsland (const string &outputFileName, const int printPeriod, const string &baseDirectory){
	//MPI Initialization
	MPI_Comm_rank(MPI_COMM_WORLD, &myId);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

	// Inicializacion de atributos internos
	this->globalSolution = NULL;
	this->printPeriod = nextPrint = printPeriod;  
	this->numExecutions = 0;
	this->evaluations = 0;
	this->evaluationsFinished = 0;
	this->actualModel = 0;
	this->outputFileName = outputFileName;
	// Fijar directorio de trabajo 
	configuration.setBaseDirectory(baseDirectory);

	// Inicializar semilla para la generacion de numeros aleatorios
	struct timeval t1;
	gettimeofday(&t1, NULL);
	unsigned int seed = (unsigned int)t1.tv_usec + t1.tv_sec;
	seed *= (myId + 1);
	srand(seed);
}

// Destructor
CoordinatorIsland::~CoordinatorIsland (){
	if (myId % numberOfProcessesPerExecution == 0){
		outputFile.close();
		delete (globalSolution);
		globalSolution = NULL;//por si se llama varias veces al run
	}
}

// Inicializacion
void CoordinatorIsland::init (const char *confFile) {
	// Leer fichero de configuracion
	configuration.load(confFile, myId);
	if ((numProcs % configuration.getNumberParallelExecutions()) != 0){
		if (myId == 0){
			cerr << "Error: numberOfParallelExecutions and numberOfProcesses are not multiples" << endl;
		}
		MPI_Finalize();
		exit(-1);
	}
	numberOfProcessesPerExecution = numProcs / configuration.getNumberParallelExecutions();

	//Call score (some plugins require an initial call to score)
	
	MOFront *moTemp = new MOFrontVector(getEjInd(), false, true);
	moTemp->insert(getEjInd()->internalClone());
	if (configuration.getScoreAlgorithm()->getType() == LOCAL_SCORE_ALGORITHM){
		((LocalScoreAlgorithm *)(configuration.getScoreAlgorithm()))->score(moTemp); 
	} else {
		((GlobalScoreAlgorithm *)(configuration.getScoreAlgorithm()))->score(moTemp);
	}
	delete moTemp;

	lastScore = 0;
	if (configuration.getErrorParsing()){ // finalizar ejecucion si la configuracion es incorrecta
		if (myId == 0){
			cout << "Error: " << configuration.getErrorParsingStr() << endl << flush;
		}
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Finalize();
		exit(-1);
	}

	vector<int> configInfo;
	for (int i = 0; i < configuration.getTeamAlgorithms().size(); i++){
		configInfo.push_back(configuration.getTeamAlgorithms()[i].configs.size());
	}
	configuration.getScoreAlgorithm()->setConfigurations(configInfo);

	if ((myId % numberOfProcessesPerExecution) != 0){ //retorna los esclavos 
		return;
	}
	for (int i = myId + 1; i < myId + numberOfProcessesPerExecution; i++){
		mySlaves.push_back(i);
	}


	ipil = configuration.getInitPopIslandLoader();
	// Inicializacion del frente global
	globalSolution = new MOFrontVector(getEjInd(), false, true);
	// Inicialización del valor del criterio de parada
	valueStopCrit = configuration.getExecutionModel()[0].valueStopCrit;
	// Rellenar la cola de tareas iniciales: ejecutar una vez cada configuracion
	/*Relegamos esto en los modulos de puntuacion
	TTask task;
	for (unsigned int i = 0; i < configuration.getTeamAlgorithms().size(); i++){
		for (unsigned int j = 0; j < configuration.getTeamAlgorithms()[i].configs.size(); j++){
			task.indexAlg = i;
			task.config = j;
			taskQueue.push_back(task);
			circularTaskQueue.push_back(task);
		}
	}*/

	// Inicialización de estadísticas de la ejecucion completa
	clearStatistics(realStatistics);

	// Inicializacion de estadísticas parciales usadas para la puntuacion	(desde el ultimo CLEAR_STATISTICS)
  clearStatistics(usedStatistics);
}

// Reseteo de vectores de estadisticas
void CoordinatorIsland::clearStatistics(vector< vector<TStatistics> > &statistics){
	TStatistics stat;
	stat.insert       = 0;
	stat.remove       = 0;
	stat.nEvaluations = 0;
	stat.runTime      = 0.0;

	statistics.clear();
	for (unsigned int i = 0; i < configuration.getTeamAlgorithms().size(); i++) {
		statistics.push_back(vector<TStatistics>(configuration.getTeamAlgorithms()[i].configs.size(), stat));
	}

	//!!if (globalSolution)
	//	!!globalSolution->clearOwnerIds();
}

// Muestra la información de las características de la ejecución y de las estadísticas obtenidas
void CoordinatorIsland::printInfo (void) { 
		printCharacteristic (numberOfProcessesPerExecution- 1);
		printStatistics ();
}

// Inicializar la ejecucion de un algoritmo en una isla
void CoordinatorIsland::initAlgorithm (const int indexAlg, const int actConf, const int idIsland){
	configuration.getScoreAlgorithm()->begins(indexAlg, actConf, idIsland);
  // Se almacena que algoritmo es ejecutado por la isla idIsland
	if ((indexAlg != execIsland[idIsland%numberOfProcessesPerExecution-1].indexAlg) || (actConf != execIsland[idIsland%numberOfProcessesPerExecution-1].config)){
	 	execIsland[idIsland%numberOfProcessesPerExecution-1].indexAlg = indexAlg;
  	execIsland[idIsland%numberOfProcessesPerExecution-1].config   = actConf;
	}
                             
	// Envio de identificador de algoritmo y configuracion
	long double *buffer = new long double[2];
	buffer[0] = indexAlg;
	buffer[1] = actConf;
	MPI_Send(buffer, 2, MPI_LONG_DOUBLE, idIsland, ALG_TAG, MPI_COMM_WORLD);
	delete buffer;

	// Informacion que recibe de la isla
	//     islandInfo[0] = tamanio de la poblacion
	//     islandInfo[1] = numero de iteraciones
	MPI_Status status;
	int islandInfo[2];
	MPI_Recv(islandInfo, 2, MPI_INT, idIsland, ISLAND_INFO, MPI_COMM_WORLD, &status);
	
	if (ipil->useGlobal()){// Send the initial inidividuals if required
		int size = globalSolution->getFrontSize()>(islandInfo[0]*configuration.getInitPercentOfIndividuals())?((int)(islandInfo[0]*configuration.getInitPercentOfIndividuals())):globalSolution->getFrontSize();
		vector< Individual* > v;
		globalSolution->getInds(size, v);

		// Convertir individuos al formato de la isla receptora (si no es el que tiene actualmente)
		vector<Individual* > vConverted, indsToDelete;
		for (int i = 0; i < size; i++){
			if (v[i]->getOwner().first != indexAlg){
				vConverted.push_back(configuration.getConversion()->convert(v[i], v[i]->getOwner().first, indexAlg));
				indsToDelete.push_back(vConverted[vConverted.size()-1]);
			} else {
				vConverted.push_back(v[i]);
			}
		}
		int nVar = 0, nObj = 0 , nMig = 0, sizeInd = 0;
		if (size != 0){
			nVar = vConverted[0]->getNumberOfVar();
			nObj = vConverted[0]->getNumberOfObj();
			nMig = vConverted[0]->getNumberOfMigData();
			sizeInd = nVar + nObj + nMig;
			buffer = new long double[4 + size*(nVar + nObj + nMig)];
			buffer[0] = size;
			buffer[1] = nVar;
			buffer[2] = nObj; 
			buffer[3] = nMig; 
		} else {
			buffer = new long double[4];
			buffer[0] = 0;
		}
			
		int count = 4, leidos;
		for (int i = 0; i < size; i++){
			vConverted[i]->serialize(&buffer[count], leidos);
			if (leidos != sizeInd){
				cout << "Error: tamaños no coinciden" << endl << flush;
				exit(-1);
			}
			count += leidos;
		}
		MPI_Send(buffer, 4 + size*(nVar + nObj + nMig), MPI_LONG_DOUBLE, idIsland, INIT_IND, MPI_COMM_WORLD);
		for (int i = 0; i < indsToDelete.size(); i++)
			delete indsToDelete[i];
		delete [] buffer;
	}

	// Almacenar el tiempo de comienzo de la ejecución
	struct timeval initialTime; 
	gettimeofday(&initialTime, NULL);
	execIsland[idIsland%numberOfProcessesPerExecution-1].initTime = (double) (initialTime.tv_sec) * 1.0e6 + (double) (initialTime.tv_usec);

	//Actualizamos el número de evaluaciones. Las estadísticas se actualizan al finalizar ejecución
	lastEvaluations[idIsland%numberOfProcessesPerExecution-1] = islandInfo[1];	
	evaluations += islandInfo[1];

}

// Inicializo cada una de las islas tomando las primeras 
// configuraciones de cada uno de los algoritmos hasta 
// completar 'numSlaves'
void CoordinatorIsland::initIslands (){
	int idle = numberOfProcessesPerExecution - 1;
	int numSlaves = idle;

	lastEvaluations.insert(lastEvaluations.begin(), numSlaves, 0);
	evaluationsPerformed.insert(evaluationsPerformed.begin(), numSlaves, 0);
	// Inicializo el vector de que algoritmo ejecuta cada isla
	TExecIsland aux;
	aux.indexAlg = -1;
	aux.config   = -1;
	execIsland.insert(execIsland.begin(), numSlaves, aux);
	
	// Insertamos cada configuracion al menos una vez (a no ser que se pase del criterio de parada global)
	TTask task;
	while (idle && (!stop()) && taskQueue.size()) {
		task = taskQueue.front();
		taskQueue.pop_front();
		initAlgorithm (task.indexAlg, task.config, mySlaves[idle-1]);
		idle--;	
	}
	
	// Asignamos el resto de islas
	int indexAlg = -1000;
	int config = -1000;
	while ((idle) && (!stop())) {
		selectionIsland(indexAlg, config, mySlaves[idle-1]);
		if ((indexAlg == -1000) || (config == -1000)){
			cerr << "Error interno algoritmo no fijado" << endl;
		}
		initAlgorithm (indexAlg, config, mySlaves[idle-1]);
		idle--;
	}
	notUsedIslands = idle;
}

// Comprueba si el criterio de parada actual se ha alcanzado
bool CoordinatorIsland::stopActual() {
	if (configuration.getExecutionModel()[actualModel].stopCrit == GLOBAL_TIME) {
		struct timeval actualTime; 
		gettimeofday(&actualTime, NULL);
		double at = (double) (actualTime.tv_sec) * 1.0e6 + (double) (actualTime.tv_usec);
		float t = (float) (at - startTime) / 1.0e6;
		return (t >= (float) configuration.getExecutionModel()[actualModel].valueStopCrit);
	} else if (configuration.getExecutionModel()[actualModel].stopCrit == GLOBAL_EXECUTIONS) {
		return (numExecutions >= valueStopCrit);
	} else if (configuration.getExecutionModel()[actualModel].stopCrit == GLOBAL_EVALUATIONS){
		return (evaluations >= valueStopCrit);
	} else if (configuration.getExecutionModel()[actualModel].stopCrit == GLOBAL_QUALITY){
		if (configuration.getScoreAlgorithm()->getType() == LOCAL_SCORE_ALGORITHM){
			return (((LocalScoreAlgorithm *)(configuration.getScoreAlgorithm()))->score(globalSolution) >= valueStopCrit);
		} else {
			return (((GlobalScoreAlgorithm *)(configuration.getScoreAlgorithm()))->score(globalSolution) >= valueStopCrit);
		}
	} else {
		cout << "\tError: Stopping criterion nondefined" << endl ; 
		return (true);
	}
}

// Comprueba si el criterio de parada global se ha alcanzado
bool CoordinatorIsland::stop(){
	if (stopActual()){//next stop criterion
		while(actualModel+1 != configuration.getExecutionModel().size()){
			actualModel++;
			if (configuration.getExecutionModel()[actualModel].stopCrit == GLOBAL_TIME){
				struct timeval actualTime; 
				gettimeofday(&actualTime, NULL);
				startTime = (double) (actualTime.tv_sec) * 1.0e6 + (double) (actualTime.tv_usec);
				return false;
			} else if (configuration.getExecutionModel()[actualModel].stopCrit == GLOBAL_EXECUTIONS){
				valueStopCrit =  (int)configuration.getExecutionModel()[actualModel].valueStopCrit  + numExecutions;
			} else if (configuration.getExecutionModel()[actualModel].stopCrit  == GLOBAL_EVALUATIONS){
				//valueStopCrit = (int)configuration.getExecutionModel()[actualModel].valueStopCrit  + evaluations;
				valueStopCrit = (int)configuration.getExecutionModel()[actualModel].valueStopCrit  + valueStopCrit;
				return false;
			} else if (configuration.getExecutionModel()[actualModel].stopCrit == GLOBAL_CLEAR_STATISTICS){
				clearStatisticEvaluation.push(evaluations);
			} else if (configuration.getExecutionModel()[actualModel].stopCrit == GLOBAL_QUALITY){
				valueStopCrit = configuration.getExecutionModel()[actualModel].valueStopCrit;
				return false;
			}
		}
		return true;
		
	} else {
		return false;
	}
}

// Puntuación de una configuración segun sus estadisticas
long double CoordinatorIsland::scoreConfiguration (const int indexAlg, const int config, const double rndValue, const int idle){
	if (configuration.getScoreAlgorithm()->getType() == LOCAL_SCORE_ALGORITHM){
		return ((LocalScoreAlgorithm *)configuration.getScoreAlgorithm())->estimateScore(lastScore, indexAlg, config, idle, rndValue);
	} else if (configuration.getScoreAlgorithm()->getType() == GLOBAL_SCORE_ALGORITHM) {
		return ((GlobalScoreAlgorithm *)configuration.getScoreAlgorithm())->estimateScore(indexAlg, config, rndValue);
	}
}

// Seleccion de la nueva islas 
void CoordinatorIsland::selectionIsland (int &indexAlg, int &config, const int idle){
	// Si no se han asignado todas las tareas iniciales
	if (taskQueue.size()) {
		TTask task = taskQueue.front();
		indexAlg        = task.indexAlg;
		config          = task.config;
		taskQueue.pop_front();
		return;
	}

	// Metodo de selección CIRCULAR
	if (configuration.getExecutionModel()[actualModel].typeSelection == Configuration::getTypeSelection("CYCLICAL")) {
		TTask task;
		task      = circularTaskQueue.front();
		indexAlg  = task.indexAlg;
		config    = task.config;
		circularTaskQueue.pop_front();
		circularTaskQueue.push_back (task);
		return;
 	}
	
	// Metodo de selecion ELITISTA
	if (configuration.getExecutionModel()[actualModel].typeSelection == Configuration::getTypeSelection("ELITIST")) {
		// Selecciono la configuracion con mejores estadisticas
		// de entre todas las posibilidades
		long double maxValue = -1; 
		long double value;
		vector< pair <int,int> > best;
		double rndValue = (double)(rand()) / RAND_MAX;
		for (unsigned int i = 0; i < configuration.getTeamAlgorithms().size(); i++) {
			for (unsigned int j = 0; j < configuration.getTeamAlgorithms()[i].configs.size(); j++) {
				value  = scoreConfiguration (i, j, rndValue, idle);
				if (value <= 0){
					cerr << "Error interno: scoreConfiguration devuelve valor <= 0" << endl;
					exit(-1);
				}
				if (value > maxValue) {
					best.clear();
					best.push_back(pair<int,int>(i, j));
					maxValue = value;
				} 
				if (value == maxValue){
					best.push_back(pair<int,int>(i, j));
				}
			}
		}
		//seleccionamos uno de los que tenían mejor valor aleatoriamente
		int selec = (int) ((long double)(best.size())*rand()/(RAND_MAX+1.0));
		indexAlg = best[selec].first;
		config   = best[selec].second;
		return;
	}
	
	// Metodo de seleccion PROBABILISTA
	if (configuration.getExecutionModel()[actualModel].typeSelection == Configuration::getTypeSelection("PROBABILITY")) {
		vector<TScore> p;  // p[0] = prob island 1
		                           // p[1] = prob island 1 + prob island 2
					   // ...
					   // p[n] = p[0] + p[1] + ... + p[n-1]
		TScore tas;
		long double value,
		       sum = 0.0;
		double rndValue = (double)(rand()) / RAND_MAX;
		//cerr << "Va a seleccionar algoritmo" << endl;
		for (unsigned int i = 0; i < configuration.getTeamAlgorithms().size(); i++) { 
			tas.indexAlg = i;
			for (unsigned int j = 0; j < configuration.getTeamAlgorithms()[i].configs.size(); j++) {
				value      = scoreConfiguration (i, j, rndValue, idle);	
				if (value <= 0){
					cerr << "Error interno: scoreConfiguration devuelve valor <= 0" << endl;
					exit(-1);
				}
				//cout << "Valor(" << i << ") = " << value << endl;
				sum       += value;
				tas.mark   = sum;
				tas.config = j;
				p.push_back (tas);
			}
		}
		for (int i = 0; i < p.size(); i++){
			double prob = (p[i].mark - ((i != 0)?p[i-1].mark:0)) / sum * 100;
		}
		double sel = ((double)rand() / (double)RAND_MAX) * sum;   // sel = [0, sum]
		for (unsigned int i = 0; i < p.size(); i++) {
			if (p[i].mark >= sel) {
				indexAlg = p[i].indexAlg;
				config   = p[i].config;
				return;
			}
		}
		return;
	} 

	//Méthod NO_CHANGE: continuar ejecutando la misma configuracion. Fue necesario incluirla para hacer ejecuciones
	//heterogéneas en las que se quiere realizar impresiones de resultados intermedios.
	if (configuration.getExecutionModel()[actualModel].typeSelection == Configuration::getTypeSelection("NO_CHANGE")) {
		if (execIsland[idle-1].indexAlg != -1){
			indexAlg = execIsland[idle-1].indexAlg;
			config   = execIsland[idle-1].config;
		} else {//circular
			TTask task;
			task      = circularTaskQueue.front();
			indexAlg  = task.indexAlg;
			config    = task.config;
			circularTaskQueue.pop_front();
			circularTaskQueue.push_back (task);
		}
		return;
	}

	cout << "\tError: Selection method of island nondefined" << endl ; 
	exit(-1);
	return;
}

// Recibe solucion de una islas y actualiza globalSolution
int CoordinatorIsland::recvLocalSolution (){
	// Recibo solucion de una isla
	MPI_Status status;	
	int sizePareto, src, evaluationsIsla;
	MPI_Probe(MPI_ANY_SOURCE, PARETO, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status,MPI_LONG_DOUBLE, &sizePareto);
	
	src = status.MPI_SOURCE;

	long double *pareto = new long double[sizePareto];
	MPI_Recv(pareto, sizePareto, MPI_LONG_DOUBLE, src, PARETO, MPI_COMM_WORLD, &status);
		
	// Actualizo las estadisticas

	int indexAlg = execIsland[src%numberOfProcessesPerExecution-1].indexAlg;
	int config = execIsland[src%numberOfProcessesPerExecution-1].config;


	// Actualizo optimal pareto
	int nInd = (int)pareto[0];
	int nVar = (int)pareto[1];
	int nObj = (int)pareto[2];
	int nMig = (int)pareto[3];

	int count = 4;
	int ins = 0;
	int rem = 0;
	int remAux;
	for (int i = 0; i < nInd; i++){
		int leidos;
		Individual *ind = getEjInd()->deserialize(nVar, nObj, nMig, &pareto[count], leidos);
		count += leidos;
		ind->setOwner(Owner(indexAlg, config));
		if (globalSolution->insert(ind, remAux)) {
			ins++;
			rem += remAux; 
		} else {
			delete(ind);
		}
	}

	if (configuration.getMaxGlobalFrontSize() != 0){
		globalSolution->crowOrder1by1(configuration.getMaxGlobalFrontSize());
	}

	//Update Score
	if (configuration.getScoreAlgorithm()->getType() == GLOBAL_SCORE_ALGORITHM) {
		((GlobalScoreAlgorithm *)configuration.getScoreAlgorithm())->updateScore(indexAlg, config, globalSolution, ins);
	}
	
	delete [] pareto;
	// Tiempo de ejecucion empleado por la isla
	struct timeval finishTime; 
	gettimeofday(&finishTime, NULL);
	double fTime = (double) (finishTime.tv_sec) * 1.0e6 + (double) (finishTime.tv_usec);
	double rTime = (fTime - execIsland[src%numberOfProcessesPerExecution-1].initTime) / 1.0e6;
	execIsland[src%numberOfProcessesPerExecution-1].initTime = fTime;

	usedStatistics[indexAlg][config].insert  += ins;
	usedStatistics[indexAlg][config].remove  += rem;
	usedStatistics[indexAlg][config].runTime += rTime;
	realStatistics[indexAlg][config].insert  += ins;
	realStatistics[indexAlg][config].remove  += rem;
	realStatistics[indexAlg][config].runTime += rTime;

	//receive the number of evaluations
	MPI_Recv(&evaluationsIsla, 1, MPI_INT, src, NUM_EVALUATIONS, MPI_COMM_WORLD, &status);
	usedStatistics[indexAlg][config].nEvaluations += evaluationsIsla; 
	realStatistics[indexAlg][config].nEvaluations += evaluationsIsla;
	//update the performed evaluations
	if (lastEvaluations[src%numberOfProcessesPerExecution-1] == 0)
		evaluations += evaluationsIsla;
	evaluationsPerformed[src%numberOfProcessesPerExecution-1] += evaluationsIsla;
	evaluationsFinished += evaluationsIsla;

	if ((clearStatisticEvaluation.size() != 0) && (evaluationsFinished >= clearStatisticEvaluation.front())){
		clearStatistics(usedStatistics);
		clearStatisticEvaluation.pop();
	}
	checkPrintPeriod();
	
	// Aumento el numero de ejecuciones 
	numExecutions++;

	// Recibimos si fue una terminación local
	bool localFinish = true;
	if (configuration.getSendResultsPerGeneration()){
		int finish;
		MPI_Status status;
		MPI_Recv(&finish, 1, MPI_INT, src, LOCAL_STOP, MPI_COMM_WORLD, &status);
		localFinish = (finish == 1);
	}
	if (localFinish){
		configuration.getScoreAlgorithm()->ends(indexAlg, config, src % numberOfProcessesPerExecution - 1);
		// Recibir puntuacion local
		if (configuration.getScoreAlgorithm()->getType() == LOCAL_SCORE_ALGORITHM){
			long double newScoreValue;
			long double newscores[2];
			MPI_Status status;
			MPI_Recv(newscores, 2, MPI_LONG_DOUBLE, src, LOCAL_SCORE, MPI_COMM_WORLD, &status);
			((LocalScoreAlgorithm *)configuration.getScoreAlgorithm())->updateScore(newscores[0], newscores[1], indexAlg, config, src % numberOfProcessesPerExecution);
			lastScore = newscores[1];
		} 
		if (lastEvaluations[src%numberOfProcessesPerExecution-1] != 0){
			evaluations += (evaluationsPerformed[src%numberOfProcessesPerExecution-1] - lastEvaluations[src%numberOfProcessesPerExecution-1]);
			evaluationsPerformed[src%numberOfProcessesPerExecution-1] = 0;
		}
	}
	return localFinish?src:-1;
}

// Envio mensajes de finalizacion
void CoordinatorIsland::finish (const int numProcs){
	long double buffer;
	buffer = ALG_FINISH;
	
	for (int i = 1; i <= numProcs; i++) 
		MPI_Send(&buffer, 1, MPI_LONG_DOUBLE, myId + i, ALG_TAG, MPI_COMM_WORLD);
}


// Diagrama de flujo:
//
//                                       <--|
//   |--> Recibir soluciones no dominadas   |
//   |                 \/                   |
//(1)|    Eliminar elementos dominados      | Criterio de seleccion        
//   |                 \/                   | 
//   si ---- ¿ Esclavos corriendo ?         |
//                     \/ no                |
//                  ¿ Stop ? ---------------
//                     \/  si
//                  Terminar 
//
// NOTAS:
//  (1) Recibir pareto de la isla
///////////////////////////////////////////////////////////////////
void CoordinatorIsland::bodyCoordinator (const int numSlaves) {
	int newAlg, newConf, idle;
	
	while (1) {
		// Recibo soluciones no dominadas
		do {
			idle = recvLocalSolution();
		} while(idle == -1);

		// ¿Stop?
		if (stop()) {
			// Recibimos los frentes de los otros
			for (int i = 0; i < numSlaves-1-notUsedIslands; i++){
				do {
					idle = recvLocalSolution();
				} while (idle == -1);
			}
		
			// Envio mensajes de finalizacion
			#ifdef DEBUG
				cout << "FINALIZING ISLANDS ... " << endl << flush;
			#endif 
			finish(numSlaves);
			return;
		}
		
		// Criterio de seleccion
		selectionIsland(newAlg, newConf, idle);
		initAlgorithm (newAlg, newConf, idle);
	}
}

/****************************************************************************************************
 * Chequear si se debe imprimir según el periodo de impresión
 * **************************************************************************************************/
void CoordinatorIsland::checkPrintPeriod(){
	if (printPeriod == 0)
		return;
	if ( (configuration.getExecutionModel()[actualModel].stopCrit == GLOBAL_EVALUATIONS) && (evaluationsFinished < nextPrint) ){
		return;
	}
	if (configuration.getExecutionModel()[actualModel].stopCrit == GLOBAL_TIME) {
		struct timeval actualTime; 
		gettimeofday(&actualTime, NULL);
		double at = (double) (actualTime.tv_sec) * 1.0e6 + (double) (actualTime.tv_usec);
		float t = (float) (at - startTime) / 1.0e6;
		if (t < nextPrint){
			return;
		}
	}
	outputFile << "Current Evaluation = " << nextPrint << endl;
	nextPrint += printPeriod;
	if (configuration.getMaxFinalSolutionSize() != 0){
		MOFront *p = new MOFrontVector(getEjInd(), false, false);
		for (unsigned int i = 0; i < globalSolution->getFrontSize(); i++) {
			p->insert (globalSolution->getInd(i));		
		}
		p->crowOrder1by1(configuration.getMaxFinalSolutionSize());
		outputFile << p;
		//if (configuration.getScoreAlgorithm()->getType() == LOCAL_SCORE_ALGORITHM){
		//	outputFile << "Score = " << ((LocalScoreAlgorithm *)(configuration.getScoreAlgorithm()))->score(p) << endl;
		//} else {
		//	outputFile << "Score = " << ((GlobalScoreAlgorithm *)(configuration.getScoreAlgorithm()))->score(p) << endl;
		//}
		delete(p);
	} else {
		outputFile << globalSolution << endl;
	}
}
			
// Arranca la ejecución del coordinador y de las islas
void CoordinatorIsland::run (const char *file) {
	init (file);
	if ((myId % numberOfProcessesPerExecution) == 0) { // Coordinador
		MPI_Group all;
		MPI_Comm_group(MPI_COMM_WORLD, &all);
		int elementZero = 0;
		MPI_Group executionIslandGroup;
		MPI_Group_excl(all, 1, &elementZero, &executionIslandGroup);
		MPI_Comm executionIslandComm;
		MPI_Comm_create(MPI_COMM_WORLD, executionIslandGroup, &executionIslandComm);

		//string newOutputFileName;
		//changeFirst(outputFileName, myId / numberOfProcessesPerExecution, newOutputFileName);
		//Modify file name depending on the number of master
		string newOutputFileName; 
		changeFirst(outputFileName, myId / numberOfProcessesPerExecution, newOutputFileName);
		stringstream fileName;
		if (newOutputFileName[0] == '/'){
			fileName << newOutputFileName;
		} else if (configuration.getOutputPath().size() != 0){
			fileName << configuration.getOutputPath() << "/" << newOutputFileName;
		} else {
			//fileName << configuration.getBaseDirectory() << "/" << newOutputFileName;
			fileName <<  "./" << newOutputFileName;
		}
		this->outputFile.open(fileName.str().c_str(), ios::in|ios::out|ios::trunc);

    // !!!! Para obtener una precision mayor al imprimir !!!!
    this->outputFile.precision(18);

		if (!outputFile.is_open()){
			cout << "File " << fileName.str() << " could not be opened" << endl;
			MPI_Abort(MPI_COMM_WORLD, -1);
			exit(-1);
		}

		struct timeval actualTime;
		gettimeofday(&actualTime, NULL);
		startTime = (double) (actualTime.tv_sec) * 1.0e6 + (double) (actualTime.tv_usec);

		#ifdef DEBUG
			cout << "INITIALIZING ISLANDS ... " << endl << flush;
		#endif
		initIslands();
		bodyCoordinator(numberOfProcessesPerExecution-1);
	} else { // Islas
		int numSlaves = numberOfProcessesPerExecution - 1;
		ExecutionIsland island(numSlaves, configuration);
		island.run(configuration);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	if (myId % numberOfProcessesPerExecution != 0)
		exit(0);

	return;
}	

// Obtener solución encontrada por la ejecución, rellenando el frente
// pasado y limitándolo al numero de individuos especificado por el 
// usuario
void CoordinatorIsland::getSolution (MOFront *p) const {
	// Eliminar los elementos duplicados
	for (unsigned int i = 0; i < globalSolution->getFrontSize(); i++) {
		p->insert (globalSolution->getInd(i));		
	}
	if (configuration.getMaxFinalSolutionSize() != 0){
		p->crowOrder1by1(configuration.getMaxFinalSolutionSize());
	}
}

// Imprimo por pantalla las estadisticas de cada algoritmo
void CoordinatorIsland::printStatistics () {
	outputFile << endl << "STATISTICS" << endl;
	outputFile << "----------" << endl;
	
	for (unsigned int i = 0; i < configuration.getTeamAlgorithms().size(); i++) {
		outputFile << "Algorithm: " << configuration.getTeamAlgorithms()[i].algorithmStr << endl;
		for (unsigned int j = 0; j < configuration.getTeamAlgorithms()[i].configs.size(); j++) {
			outputFile << " ·Configuration " << j << ": ";
			for (unsigned int z = 0; z < configuration.getTeamAlgorithms()[i].configs[j].size(); z++) 
				outputFile << configuration.getTeamAlgorithms()[i].configs[j][z] << " "; 
			outputFile << endl ;
			outputFile << "         Inserted: " << realStatistics[i][j].insert << endl;
			outputFile << "          Removed: " << realStatistics[i][j].remove << endl;
			//outputFile << "   Final Inserted: " << globalSolution->getRealInsertedByMoea (i, j) << endl;
			outputFile << "      Evaluations: " << realStatistics[i][j].nEvaluations << endl;
			outputFile << "   Run time (sec): " << realStatistics[i][j].runTime << endl;
		}
		outputFile << endl;
	}
}

// Imprimo por pantalla las caracteristica del team algorithms
void CoordinatorIsland::printCharacteristic (const int numSlaves) {
	outputFile << endl << "TEAM ALGORITHMS" << endl;
	outputFile << "---------------" << endl ;
	outputFile << "Number of islands      : " << numSlaves << endl;
	outputFile << "Execution Model        : " << endl;
	for (unsigned int i = 0; i < configuration.getExecutionModel().size(); i++){
		if (configuration.getExecutionModel()[i].stopCrit == Configuration::getGlobalTypeStoppingCriterion("CLEARSTATISTICS"))
			outputFile << "								[ ClearStatistics ]  " << endl;
		else 
			outputFile << "								[" << Configuration::getGlobalTypeStoppingCriterion(configuration.getExecutionModel()[i].stopCrit) << "  " << configuration.getExecutionModel()[i].valueStopCrit << "  " << Configuration::getTypeSelection(configuration.getExecutionModel()[i].typeSelection) << " ]  "<< endl; 
	}
	outputFile << "Init percent of ind    : " << configuration.getInitPercentOfIndividuals() * 100 << endl;
	outputFile << "Migration probability  : " << configuration.getMigrationProbability() << endl;
	outputFile << "Number ind to migrate  : " << configuration.getNumberIndMigrate() << endl;
	outputFile << "Max global front size  : " << configuration.getMaxGlobalFrontSize() << endl;
	outputFile << "Max final solution size: " << configuration.getMaxGlobalFrontSize() << endl << endl;

	for (unsigned int i = 0; i < configuration.getTeamAlgorithms().size(); i++) {
		outputFile << "Algorithm: " << configuration.getTeamAlgorithms()[i].algorithmStr << endl;
		if (configuration.getTeamAlgorithms()[i].maxLocalFrontSize == 0){
			outputFile << "		Max local front size   : not limited" << endl;
		} else{
			outputFile << "		Max local front size   : " << configuration.getTeamAlgorithms()[i].maxLocalFrontSize << endl;
		}

		for (unsigned int j = 0; j < configuration.getTeamAlgorithms()[i].configs.size(); j++) {
			outputFile << " ·Configuration " << j << ": " << endl;
			outputFile << "		";
			for (unsigned int z = 0; z < configuration.getTeamAlgorithms()[i].configs[j].size(); z++)
				outputFile << configuration.getTeamAlgorithms()[i].configs[j][z] << " " ;
			outputFile << endl ;
		}
		outputFile << endl ;
	}
}
