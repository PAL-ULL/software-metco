/* -----------------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------------- */
#include "mpi.h"

#include <time.h>
#include <sys/time.h>
#include <string>
#include <libgen.h>
#include <sys/resource.h>

#include "MOFrontVector.h"
#include "CoordinatorIsland.h"
#include "Individual.h"
#include "Definitions.h"
#include "Configuration.h"


int main (int argc, char *argv[]) {
	struct timeval t1, t2;

	// Inicialización de MPI
	MPI_Init(&argc, &argv);
	//dejamos que la pila crezca todo lo que el administrador nos deje
	struct rlimit value;
	getrlimit(RLIMIT_STACK, &value);
	value.rlim_cur = value.rlim_max;
	setrlimit(RLIMIT_STACK, &value);
	getrlimit(RLIMIT_STACK, &value);

	// Comprobación del paso correcto de parámetros	
	int myId;
	MPI_Comm_rank(MPI_COMM_WORLD, &myId);
	if ((argc != 3) && (argc != 4)){
		if (myId == 0)
			cout << "Uso correcto: " << argv[0] << " outputFile configFile [printPeriod]" << endl;
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Finalize();
		return (-1);
	}
	
	string baseDirectory = string(dirname(argv[0])) + "/";
	// Creación del coordinador
	CoordinatorIsland coordinator(argv[1], ((argc==4)?atoi(argv[3]):0), baseDirectory);

	// Tiempo inicial
	gettimeofday(&t1, NULL);

	// Ejecución del Team Algorithm
	coordinator.run (argv[2]);

	Individual *ind = coordinator.getEjInd();
	// End Time
	gettimeofday(&t2, NULL);
	coordinator.printInfo();
	double start = (double) (t1.tv_sec) * 1.0e6 + (double) (t1.tv_usec);
	double end = (double) (t2.tv_sec) * 1.0e6 + (double) (t2.tv_usec);
	float dt = (float) (end - start) / 1.0e6;
	coordinator.getOutputFile() << endl;
	coordinator.getOutputFile() << "Time =  " << dt << endl << endl;

	// Print results
	coordinator.getOutputFile() << "Current Evaluation = " << coordinator.getEvaluations() << endl;
	MOFront *p = new MOFrontVector(ind, false, false);
	coordinator.getSolution(p);
	coordinator.getOutputFile() << p;
	coordinator.getOutputFile() << endl;
	delete (p);
	//MPI_Barrier(MPI_COMM_WORLD);
	//MPI_Abort(MPI_COMM_WORLD, -1);
	exit(EXIT_SUCCESS);
}
