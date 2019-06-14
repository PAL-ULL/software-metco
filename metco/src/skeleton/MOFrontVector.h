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

#ifndef _MO_FRONT_VECTOR_H_
#define _MO_FRONT_VECTOR_H_

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
class MOFrontVector : public MOFront {

public:
	// Constructor
	MOFrontVector (const Individual *ejInd, bool copyOnInsert, bool deleteOnDestroy) : MOFront(ejInd, copyOnInsert, deleteOnDestroy) {}

	// Destructor
	~MOFrontVector (void);

	// Getters
	inline unsigned int getFrontSize (void) { return front.size(); }
	inline Individual* getInd (int indexInd) const { return front[indexInd]; }
	inline void getAllIndividuals(vector<Individual *> &f) { f = front; }
	void getInds (int nInd, vector <Individual*> &inds) ;

	// Insercion de soluciones en el frente
	bool insert (Individual *ind, int &erased, bool &iguales);
	
	void clear();

	// Envio de soluciones del frente al procesador con id = dest
#ifndef NO_MPI
	void send (int dest);
#endif

	// Operador de crowding del NSGA-II
	void crowOrder (const unsigned int finalSize);

	static int objOrder;
	
private:
	// Variable estatica para ordenaciones por objetivos
	vector<Individual *> front;      // Frente de soluciones
};

bool orderByObj(Individual *i1, Individual *i2);

#endif
