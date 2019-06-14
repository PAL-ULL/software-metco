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

#ifndef __MO_FRONT_H__
#define __MO_FRONT_H__

#include <vector>
#include <iostream>
#include <fstream>
#include <float.h>
#include <stdlib.h>

#include "Definitions.h"
#include "Individual.h"

using namespace std;

// Clase que define un frente de soluciones
class MOFront {

public:
	// Constructor
	MOFront (const Individual *ejInd, bool copyOnInsert, bool deleteOnDestroy);

	// Destructor
	virtual ~MOFront (void) {};

 	// Deben ser reimplementados
	// - Acceso a elementos
	virtual unsigned int getFrontSize (void) = 0; 
	virtual Individual* getInd (int indexInd) const = 0;
	virtual void getInds (int nInd, vector <Individual*> &inds) = 0;
	virtual void getAllIndividuals(vector<Individual*> &inds) = 0; 
	// - Limitacion del tam.
	virtual void crowOrder (const unsigned int finalSize) = 0;
	// - Insercion de soluciones en el frente
	virtual bool insert (Individual *ind, int &erased, bool &iguales) = 0;
	// - Eliminar elementos del frente
	virtual void clear() = 0;

	// Ya implementados
	// - Insercion de elementos
	bool insert (Individual *ind);
	bool insert (Individual *ind, int &erased);
	bool insert (Individual *ind, bool &iguales);

	// Eliminar elementos
	// Operador de crowding del NSGA-II
	void crowOrder1by1(const unsigned int finalSize);
	// Lectura/Escritura de frentes
	void read (const string &inputFile);
	void readAll (const string &inputFile);
	void write (const string &outputFile);
	void writeAll (const string &outputFile);
	// - Mezcla de frente de soluciones
	void merge (const char *namefile1, const char *namefile2, const int numObj);
	// - Calculo de metricas
	long double getEpsilonIndicator (MOFront &p) ;
	double getHV(vector<long double> &refPoint) ;
	double getHVNorm(vector<long double> &refPoint, vector<long double> &minValues, vector<long double> &maxValues) ;
	

	friend ostream &operator<<(ostream &os, MOFront *p);
	// Envio de soluciones del frente al procesador con id = dest
	virtual void send (int dest) { cout << "Ejecutando un send que no hace nada!" << endl; exit(-1); }

protected:
	const Individual *ejInd;     // Individuo tipo usado
	bool copyOnInsert;
	bool deleteOnDestroy;
};

#endif
