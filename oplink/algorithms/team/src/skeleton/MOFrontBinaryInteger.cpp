/***********************************************************************************
 * AUTORES
 *   - Carlos Segura González
 * 
 * Sirve para mantener el frente cuando se tienen dos objetivos y uno de ellos es un
 8 valor entero, y se conocen sus valores minimo y maximo
 ************************************************************************************/

#include "MOFrontBinaryInteger.h"
#include "utils.h"
#include <math.h>

// Variable estática para ordenaciones por objetivos
int MOFrontBinaryInteger::objOrder;

// Destructor
MOFrontBinaryInteger::~MOFrontBinaryInteger (void) {
	clear();
}

void MOFrontBinaryInteger::getAllIndividuals(vector<Individual *> &frontSolution) {
	frontSolution.clear();
	int firstIndex = ((ejInd->getInternalOptDirection(objIndex)) == MINIMIZE)?0:(front.size()-1);
	int lastIndex = ((ejInd->getInternalOptDirection(objIndex)) == MAXIMIZE)?0:(front.size()-1);
	int incr = ((ejInd->getInternalOptDirection(objIndex)) == MINIMIZE)?1:-1;
	double currentValue = (ejInd->getInternalOptDirection(objNonIndex) == MINIMIZE)?DBL_MAX:-DBL_MAX;
	for (int i = firstIndex; i <= lastIndex; i+=incr){
		if (front[i] != NULL){
			double nextValue = front[i]->getObj(objNonIndex);
			if ((ejInd->getInternalOptDirection(objNonIndex) == MINIMIZE) && (front[i]->getObj(objNonIndex) < currentValue)){
				frontSolution.push_back(front[i]);
				currentValue = front[i]->getObj(objNonIndex);
			} else if ((ejInd->getInternalOptDirection(objNonIndex) == MAXIMIZE) && (front[i]->getObj(objNonIndex) > currentValue)){
				frontSolution.push_back(front[i]);
				currentValue = front[i]->getObj(objNonIndex);
			} else {
				if (deleteOnDestroy){
					delete front[i];
				}
				front[i] = NULL;
			}
		}
	}
}

// Devuelve un subconjunto aleatorio de individuos del frente
void MOFrontBinaryInteger::getInds (int nInd, vector<Individual*> &inds) {
	vector<Individual *> frontLocal;
	getAllIndividuals(frontLocal);
	vector<int> index;
	generateRandomValues(0, frontLocal.size()-1, nInd, index);
	inds.clear();
	for (int i = 0; i < index.size(); i++)
		inds.push_back(frontLocal[index[i]]);
}

// Inserción un nuevo individuo en el frente 
// 'erased' se corresponde con el numero de individuos eliminados del 
// frente en caso de que sea insertado
// 'igual' determina si ya hay una solución idéntica en el frentea
//Nota el calculo de erased, y el valor devuelto no son correctos, pero
//esto solo es usado para estadisticas, asi que da igual.
bool MOFrontBinaryInteger::insert (Individual *ind, int &erased, bool &igual) {
	erased = 0;
	igual = false;
	int result;
	unsigned int i = 0;
  	
	// Recorro el frente
	int objIndexedValue = (int)ind->getObj(objIndex);
	double objNoIndexedValue = ind->getObj(objNonIndex);
	if (front[objIndexedValue] != NULL){
		if (front[objIndexedValue]->getObj(objNonIndex) == objNoIndexedValue){
			igual = true;
			return false;
		} else if (((ind->getInternalOptDirection(objNonIndex) == MAXIMIZE) && (objNoIndexedValue < front[objIndexedValue]->getObj(objNonIndex))) ||
	((ind->getInternalOptDirection(objNonIndex) == MINIMIZE) && (objNoIndexedValue > front[objIndexedValue]->getObj(objNonIndex)))){
			return false;
		}
	}
	if (deleteOnDestroy){
		delete front[objIndexedValue];
	}
	if (copyOnInsert){
		front[objIndexedValue] = ind->internalClone();
	} else {
		front[objIndexedValue] = ind;
	}
	return true;
}

void MOFrontBinaryInteger::send(int dest) {
	cout << "Send no implementado en este tipo de frentes" << endl << flush;
	exit(-1);
}

// Operador de crowding del NSGA-II
void MOFrontBinaryInteger::crowOrder(const unsigned int finalSize) {
	double difference;

	vector<Individual*> frontLocal;
	getAllIndividuals(frontLocal);

	if (frontLocal.size() > finalSize) {
		for (unsigned int j = 0; j < frontLocal.size(); j++) {
			frontLocal[j]->setFitnessValue(0);
		}
		// calculate the distances
		for (unsigned int i = 0; i < frontLocal[0]->getNumberOfObj(); i++) {
			objOrder = i;
			sort(frontLocal.begin(), frontLocal.end(), orderByObjBinaryInteger);
			difference = frontLocal[frontLocal.size()-1]->getObj(i) - frontLocal[0]->getObj(i);
			if (difference == 0)
				difference = 1;
			frontLocal[0]->setFitnessValue(DBL_MAX);
			frontLocal[frontLocal.size()-1]->setFitnessValue(DBL_MAX);
			for (unsigned int j = 1; j < frontLocal.size()-1; j++) {
				if (frontLocal[j]->getFitnessValue() != DBL_MAX) {
					frontLocal[j]->setFitnessValue(((frontLocal[j+1]->getObj(i) - frontLocal[j-1]->getObj(i))/(difference)) + frontLocal[j]->getFitnessValue());
				}
			}
		}
		
		// ordenamos por el valor calculado
		sort(frontLocal.begin(), frontLocal.end(), ordByFitness);
		for (unsigned int i = finalSize; i < frontLocal.size(); i++) {
			front[(int)(frontLocal[i]->getObj(objIndex))] = NULL;
			if (deleteOnDestroy)
				delete(frontLocal[i]);
		}
		frontLocal.resize(finalSize);
	}
}

// Borrar el frente
void MOFrontBinaryInteger::clear(void) {
	if (deleteOnDestroy)
		for (unsigned int i = 0; i < front.size(); i++)
			delete (front[i]);
	front.clear();
}

// Funciones auxiliares para ordenación
bool orderByObjBinaryInteger(Individual *i1, Individual *i2) {
	return (i1->getObj(MOFrontBinaryInteger::objOrder) < i2->getObj(MOFrontBinaryInteger::objOrder));
}
