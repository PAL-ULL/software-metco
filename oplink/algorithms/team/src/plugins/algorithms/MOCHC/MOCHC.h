
#ifndef __MOCHC_H__
#define __MOCHC_H__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <set>

#include "MOFront.h"
#include "Individual.h"
#include "EA.h"


class MOCHC : public EA {
		  
public:
	// Constructor
	MOCHC() {};

	// Define una generación de búsqueda del algoritmo	
	void runGeneration();

	// Inicializa los parámetros iniciales del algoritmo
	bool init(const vector<string> &params);

	// Rellena un frente con las soluciones actuales
	void getSolution(MOFront *p) ;

	// Muestra la información relativa al algoritmo	
	void printInfo(ostream &os) const;
	
	string getName() const { return "MOCHC"; }
	
private:
	float pm; // Mutation probability
	float icc; // Umbral Inicial
	float cv; // Valor de convergencia k
	float pp; // Poblacion preservada al reiniciar
	double minimunDistance;
	set<Individual*> populationOld;
	bool changePopulation;	//Indicador que almacena si la poblacion ha cambiado en la ultima generacion
	
	static const int POPSIZE_DEFAULT = 100;	// Population size by default

	// Evaluation of the individuals in the population
	void evaluation (void);	

	void initialize();
	
	// Sort and rank the crowding population
	void rankCrowdingPopulation();

	// Sort and rank the population
	void rankPopulation();
	
	void mutation();

	// Generate the offsprings
	void createChildPop();	
	
	void selectInPopulation(vector<Individual*> pop, int numberIndividual, vector<Individual*> &selected);

	bool compareIndividual (Individual *ind);
	
};


#endif
