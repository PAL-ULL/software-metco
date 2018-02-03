/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 ************************************************************************************/

#ifndef NO_MPI
#include "mpi.h"
#endif
#include "MOFrontVector.h"
#include "utils.h"
#include <math.h>

// Variable estática para ordenaciones por objetivos
int MOFrontVector::objOrder;

// Destructor
MOFrontVector::~MOFrontVector (void) {
	clear();
}

// Devuelve un subconjunto aleatorio de individuos del frente
void MOFrontVector::getInds (int nInd, vector<Individual*> &inds) {
	vector<int> index;
	generateRandomValues(0, front.size()-1, nInd, index);
	inds.clear();
	for (int i = 0; i < index.size(); i++)
		inds.push_back(front[index[i]]);
}

// Inserción un nuevo individuo en el frente 
// 'erased' se corresponde con el numero de individuos eliminados del 
// frente en caso de que sea insertado
// 'igual' determina si ya hay una solución idéntica en el frente
bool MOFrontVector::insert (Individual *ind, int &erased, bool &igual) {
	erased = 0;
	igual = false;
	int result;
	unsigned int i = 0;
  
  // Esto esta comentado para poder visualizar todos los individuos de la poblacion en el caso mono-objetivo	
	// Recorro el frente
	while (i < front.size()) {
		result = dominanceTest(front[i], ind);
		// Si alguno me domina no pertenecere al frente
   	if ((result == FIRST_DOMINATES) || (result == NON_DOMINANCES_EQUALS)) {	
			igual = (result == NON_DOMINANCES_EQUALS);
     	return false;
		}
		if (result == SECOND_DOMINATES) { 	// Elimino de la lista a los individuos
			if (deleteOnDestroy){
				delete (front[i]);
			} 
			front[i] = front[front.size()-1];
     	front.pop_back();
			erased++;
		} else 
      	i++;
	}
	if (copyOnInsert){
		front.push_back(ind->internalClone());
	}
	else {
		front.push_back(ind);
	}
	return true;
}

#ifndef NO_MPI
void MOFrontVector::send(int dest) {
	if (front.size() == 0){
		long double n = 0;
		MPI_Send(&n, 1, MPI_LONG_DOUBLE, 0, PARETO, MPI_COMM_WORLD);
		return;
	} 
	else {
		int nInd = front.size();
		int nVar = (*front.begin())->getNumberOfVar();
		int nObj = (*front.begin())->getNumberOfObj();
		int nMig = (*front.begin())->getNumberOfMigData();
		int tamBuffer = 4 + nInd * (nVar + nObj + nMig);
		long double *sendBuffer = new long double[tamBuffer];
		if (sendBuffer == NULL){
			cout << "ERROR " << endl << flush;
		}
		sendBuffer[0] = nInd;
		sendBuffer[1] = nVar;
		sendBuffer[2] = nObj;
		sendBuffer[3] = nMig;
		int count = 4, leidos;
		for (unsigned int i = 0; i < nInd; i++) {
			front[i]->serialize(&sendBuffer[count], leidos);
			count += leidos;
		}
		MPI_Send(sendBuffer, tamBuffer, MPI_LONG_DOUBLE, dest, PARETO, MPI_COMM_WORLD); 
		delete [] sendBuffer;
	}
}
#endif

// Operador de crowding del NSGA-II
void MOFrontVector::crowOrder(const unsigned int finalSize) {
	double difference;
	
	if (front.size() > finalSize) {	
		for (unsigned int j = 0; j < front.size(); j++) {
			front[j]->setFitnessValue(0);
		}
		// calculate the distances
		for (unsigned int i = 0; i < front[0]->getNumberOfObj(); i++) {
			objOrder = i;
			sort(front.begin(), front.end(), orderByObj);
			difference = front[front.size()-1]->getObj(i) - front[0]->getObj(i);
			if (difference == 0)
				difference = 1;
			front[0]->setFitnessValue(DBL_MAX);
			front[front.size()-1]->setFitnessValue(DBL_MAX);
			for (unsigned int j = 1; j < front.size()-1; j++) {
				if (front[j]->getFitnessValue() != DBL_MAX) {
					front[j]->setFitnessValue(((front[j+1]->getObj(i) - front[j-1]->getObj(i))/(difference)) + front[j]->getFitnessValue());
				}
			}
		}
		
		// ordenamos por el valor calculado
		sort(front.begin(), front.end(), ordByFitness);
		for (unsigned int i = finalSize; i < front.size(); i++) {
			if (deleteOnDestroy)
				delete(front[i]);
		}
		front.resize(finalSize);
	}
}

// Borrar el frente
void MOFrontVector::clear(void) {
	if (deleteOnDestroy){
		for (unsigned int i = 0; i < front.size(); i++)
			delete (front[i]);
	}
	front.clear();
}

// Funciones auxiliares para ordenación
bool orderByObj(Individual *i1, Individual *i2) {
	return (i1->getObj(MOFrontVector::objOrder) < i2->getObj(MOFrontVector::objOrder));
}
