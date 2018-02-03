/***********************************************************************************
 * AUTHORS
 *   - Alejandro Cervantes   
 *   - Sandra Garcia
 *	 - Cristobal Luque
 *   
 * 
 * DATE
 *   March 2008
 *
 * DESCRIPTION
 *
 ************************************************************************************/


#ifndef __EE_H__
#define __EE_H__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>

#include "MOFront.h"
#include "Individual.h"
#include "EA.h"

#define ALFA 0.6
#define PI 3.1415926535897


class EE : public EA {
		  
public:
	// Constructor
	EE() {};

	// Define una generación de búsqueda del algoritmo	
	void runGeneration();

	// Inicializa los parámetros iniciales del algoritmo
	bool init(const vector<string> &params);

	// Rellena un frente con las soluciones actuales
	void getSolution(MOFront *p) ;

	// Muestra la información relativa al algoritmo	
	void printInfo(ostream &os) const;
	
	static int objOrder;	// Variable indice para el objetivo actual

private:
	
	// Varianza del individuo
	float varianza; 

	static const int POPSIZE_DEFAULT = 100;	// Population size by default

	// Evaluation of the individuals in the population
	void evaluation (void);// Llama a EA::evaluate() para cada individuo

	// Sort and rank the crowding population
	void rankCrowdingPopulation();

	// Sort and rank the population
	void rankPopulation();

	// Generate the offsprings
	void createChildPop();

	//Mutacion propia del algoritmo
	Individual* muta(Individual* ind);

	//Funcion gaussiana
	double gaussiana (double media,double vari);

	//Calcula el valor absoluto
	double abso(double zx);

	//Redefinido de EA para incluir la varianza en los individuos
	void fillPopWithNewIndsAndEvaluate();
};

#endif
