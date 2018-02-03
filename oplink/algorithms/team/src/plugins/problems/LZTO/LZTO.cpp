/***********************************************************************************
 * AUTORES
 *   - Carlos Segura González
 * 
 * FECHA
 *    Mayo 2010
 ************************************************************************************/

#include "LZTO.h"
#include <vector>
#include <time.h>
#include <sys/time.h>

// Initialization: N (string size)
//   
bool LZTO::init (const vector<string> &params){
	if (params.size() != 1){
		return false;
	}
	int nVar = atoi(params[0].c_str());
	if (nVar <= 0){
		return false;
	}
	setNumberOfVar(nVar);
	setNumberOfObj(NOBJ);
	return true;
}

// Evaluacion
void LZTO::evaluate (void){
	int lz = 0;
	for (int i = 0; i < getNumberOfVar(); i++){
		if (getVar(i) <= 0.5){
			lz++;
		} else {
			break;
		}
	}
	setObj(0, lz);
	int to = 0;
	for (int i = getNumberOfVar() - 1; i >= 0; i--){
		if (getVar(i) > 0.5){
			to++;
		} else {
			break;
		}
	}
	setObj(1, to);
}

void LZTO::restart(){
	//Inicializacion del gen inicial y del vector de solucion
	for (unsigned int i = 0; i < getNumberOfVar(); i++) {
		int valor = int(rand()%2);
		setVar(i,valor);
	}
}

unsigned int LZTO::getNeighbourhoodSize() const {
	return getNumberOfVar();
}
	
Individual* LZTO::getNeighbour(const int i) const {
	Individual *newInd = internalClone();
	if (newInd->getVar(i) == 0){
		newInd->setVar(i, 1);
	} else {
		newInd->setVar(i, 0);
	}
	newInd->evaluate();
	return newInd;
}

void LZTO::getNeighbourObjectives(const int neigh, vector<double> &objectives) const {
	objectives[0] = getObj(0);
	objectives[1] = getObj(1);
	if (getVar(neigh) <= 0.5){
		if (getObj(1) == getNumberOfVar() - neigh - 1){//Mejora de objetivo1
			objectives[1]++;
			for (int i = neigh - 1; i >= 0; i--){
				if (getVar(i) > 0.5){
					objectives[1]++;
				} else {
					break;
				}
			}
		} 
		if (objectives[0] > neigh){//Empeoramiento de obj0
			objectives[0] = neigh;
		}
	} else {//Actualmente es 1
		if (getObj(0) == neigh){//Mejora de objetivo
			objectives[0]++;
			for (int i = neigh + 1; i < getNumberOfVar(); i++){
				if (getVar(i) <= 0.5){
					objectives[0]++;
				} else {
					break;
				}
			}
		} 
		if (objectives[1] > getNumberOfVar() - neigh - 1){//Empeoramiento de objetivo
			objectives[1] = getNumberOfVar() - neigh - 1;
		}
	}
}
	
// Clonacion
Individual* LZTO::clone (void) const{
	return new LZTO();
}


