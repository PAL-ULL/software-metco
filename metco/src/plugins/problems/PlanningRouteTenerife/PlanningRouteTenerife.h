/* -----------------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------------- */


#ifndef __PlanningRouteTenerife_H__
#define __PlanningRouteTenerife_H__


#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>

#include "utils.h"
#include "Individual.h"

#define CROSSOVER_UNIFORM 0
//#define CROSSOVER_HUX 1
#define NOBJ = 2

class PlanningRouteTenerife : public Individual {

	private:
	// Genetic Operators
	void dependentCrossover(Individual* ind);
	void dependentMutation(double pm);

	// Porcentaje de actividades, coste de estas y tiempoTotal
	// static double coste;
	// static float porcentajeActividades;
	static int tiempoTotal;
	
	// Actividades, costes, duracion, distancias, individuos
	static vector<string> actividades; 
	static vector<double> costes;
	static vector<int> duracion;
	static vector< vector<int> > distancias;
	// static vector<int>individuos;
	
	public:

	// Leemos la instancia del fichero
	int readFile(const string filename);

	// Setters
	
	
	// Getters
	

	// Inicializamos la función
	bool init(const vector<string> &params);
	

	// genera a un individuo aleatorio
	void restart(void);

	// Repara la solucion si se pasa del tiempo
	void repair(void);

	// Calculates the max profit/weight ratio for each item in increasing order
	//void ratios(void);

	// Quicksort algorithm to order the max profit/weight ratio vector
	//void quicksort(int *deleteOrder, float *maxRatio, int begin, int end);

	// Comprueba la restriccion de tiempo
	bool checkTiempo(void);

	// evalua y escoge un individuo
	void evaluate(void);
	
	Individual* clone(void) const;

	// Ranges
	double inline getMaximum(const int i) const { return 1; }

	double inline getMinimum(const int i) const { return 0; }

	unsigned int inline getOptDirection(const int i) const { 
    if (i == 0) 
      return MINIMIZE;
    else if (i == 1)
      return MAXIMIZE;    
  }
	
	unsigned int type_crossover;
	
//void crossover_HUX (Individual *ind);

};

#endif
