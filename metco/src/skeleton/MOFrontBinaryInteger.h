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
 *   Clase para la representacion de un frente de soluciones en problemas de 
 *   optimización multi-objetivo.
 *   
 *   La clase proporciona distintos métodos para insertar soluciones en el frente,
 *   para imprimirlas a fichero o para leerlas de fichero.
 *
 ***********************************************************************************/

#ifndef _MO_FRONT_BINARY_INTEGER_H_
#define _MO_FRONT_BINARY_INTEGER_H_

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <float.h>
#include <stdlib.h>

#include "MOFront.h"
#include "Definitions.h"
#include "Individual.h"

using namespace std;

// Clase que define un frente de soluciones
class MOFrontBinaryInteger : public MOFront {

public:
	// Constructor
	MOFrontBinaryInteger (const Individual *ejInd, bool copyOnInsert, bool deleteOnDestroy) : MOFront(ejInd, copyOnInsert, deleteOnDestroy) {
		if (ejInd->getNumberOfObj() != 2){
			cout << "MOFrontBinaryInteger con numero de objetivos distinto de 2" << endl << flush;
			exit(-1);
		}
		objIndex = ejInd->getIndexObj();
		if (objIndex == 0){
			objNonIndex = 1;
		} else if (objIndex == 1){
			objNonIndex = 0;
		} else {
			cout << "MOFrontBinaryInteger usado con individuo que no fija objIndex" << endl << flush;
			exit(-1);
		}
		minObjIndex = ejInd->getIndexObjMin();
		maxObjIndex = ejInd->getIndexObjMax();
		front.insert(front.begin(), maxObjIndex - minObjIndex + 1, (Individual *)NULL);
	}

	// Destructor
	~MOFrontBinaryInteger (void);

	// Getters
	inline unsigned int getFrontSize (void) { 
		vector <Individual *> frontLocal; 
		getAllIndividuals(frontLocal); 
		return frontLocal.size(); 
	}

	inline Individual* getInd (int indexInd) const { return front[indexInd]; }
	void getAllIndividuals(vector<Individual *> &front); 
	void getInds (int nInd, vector <Individual*> &inds) ;

	// Insercion de soluciones en el frente
	bool insert (Individual *ind, int &erased, bool &iguales);
	
	void clear();

	// Envio de soluciones del frente al procesador con id = dest
	void send (int dest);

	// Operador de crowding del NSGA-II
	void crowOrder (const unsigned int finalSize);

	static int objOrder;
	
private:
	vector<Individual *> front;
	int objIndex;
	int objNonIndex;
	int minObjIndex;
	int maxObjIndex;
};

bool orderByObjBinaryInteger(Individual *i1, Individual *i2);

#endif
