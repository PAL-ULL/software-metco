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
 *    Clase que representa a las islas esclavas. Su función es ejecutar la 
 *    configuración de EA que le indique la isla maestra.
 *    Mantiene una solución local que será enviada a la isla maestra cuando
 *    la condición de parada local sea alcanzada.
 *    Se quedará en espera hasta que la isla maestra le de ordenes para
 *    ejecutar una nueva configuración o para finalizar la ejecución.
 *    Se encarga además de migrar individuos y recibir individuos migrados
 *    tras el paso de cada generación del algoritmo (en función de los 
 *    parámetros fijados por el usuario)
***********************************************************************************/

#ifndef __EXECUTIONISLAND_H__
#define __EXECUTIONISLAND_H__

#include "mpi.h"
#include "Definitions.h"
#include "CoordinatorIsland.h"
#include "EA.h"
#include "Configuration.h"
#include <iostream>
#include <vector>
#include <map>
#include <queue>

using namespace std;

class ExecutionIsland {
	public:
		// Constructor
		ExecutionIsland(const int slaveIslands, const Configuration &configuration);

		// Destructor
		~ExecutionIsland();

		// Método de ejecución de las islas esclavas
		void run(const Configuration &configuration);
	private:
		MPI_Comm executionIslandComm;
		int completeGenerations;
		// Buffers de envios y recepcion
		long double *buffer;
		int tamBuffer, tamBuffer2;
		static const int DEF_SIZE_BUF = 10;

		// Parametros de migracion
		double pMigration;
		unsigned int maxIndToMigrate;
		
		unsigned int slaveIslands;

		int	numberOfProcessesPerExecution;
		int myMaster;
		int myId, numProcs;
		vector<int> myPartners;

    // Data structure to store data of each algorithm and configuration for the restart process
    map<int, double*> configsRestartInfo;

		// Request de las comunicaciones asincronas
		queue<pair<MPI_Request *, long double *> > pendingRequests;

		// Population recovery //FIXME esto se deberia hacer desde los algoritmos o con un modulo o algo asi
		vector<Individual*> originalPopulation;
};

#endif
