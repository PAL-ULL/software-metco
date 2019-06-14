/***********************************************************************************
 * AUTORES
 *   - Ofelia Gonz�lez P�rez
 *   - Gara Miranda Valladares
 *   - Carlos Segura Gonz�lez
 * 
 * FECHA
 *    Noviembre 2007
 *
 * DESCRIPCION
 *    Clase que representa a las islas esclavas. Su funci�n es ejecutar la 
 *    configuraci�n de EA que le indique la isla maestra.
 *    Mantiene una soluci�n local que ser� enviada a la isla maestra cuando
 *    la condici�n de parada local sea alcanzada.
 *    Se quedar� en espera hasta que la isla maestra le de ordenes para
 *    ejecutar una nueva configuraci�n o para finalizar la ejecuci�n.
 *    Se encarga adem�s de migrar individuos y recibir individuos migrados
 *    tras el paso de cada generaci�n del algoritmo (en funci�n de los 
 *    par�metros fijados por el usuario)
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

		// M�todo de ejecuci�n de las islas esclavas
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
