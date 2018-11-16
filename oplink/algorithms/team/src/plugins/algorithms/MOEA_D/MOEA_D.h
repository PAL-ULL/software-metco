/***********************************************************************************
 * AUTHORS
 *   - Alejandro Marrero
 *   - Eduardo Segredo
 *
 * DATE
 *   November 2018
 *
 * DESCRIPTION
 *
 * Implementation of the Multi-objective Evolutionary Algorithm based on Decomposition
 * (MOEA/D) given in:
 * Qingfu Zhang, Hui Li (2007). "MOEA/D: A Multiobjective Evolutionary Algorithm
 * Based on Decomposition". IEEE Transactions on Evolutionary Computation.
 *
 * **********************************************************************************/
#ifndef __MOEA_D_H__
#define __MOEA_D_H__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <memory>
#include "MOFront.h"
#include "Individual.h"
#include "EA.h"


class MOEA_D : public EA {
public:
	static const int PARAMS;
	static const int RANDOM;
	static const int FILE;
	static int DIRECTION;
	// Constructor
	MOEA_D();
	virtual ~MOEA_D();
	// Define una generación de búsqueda del algoritmo
	void runGeneration();
	// Inicializa los parámetros iniciales del algoritmo
	bool init(const vector<string> &params);
	// Rellena un frente con las soluciones actuales
	void getSolution(MOFront *p) ;
	// Muestra la información relativa al algoritmo
	void printInfo(ostream &os) const;
	inline string getName() const { return "MOEA/D"; }
private:
	void initWeights(bool withFile);
	void initNeighborhood();
	void initReferencePoint();
	void updateReferencePoint(Individual*);
	void updateReferencePoint(vector<Individual*>&);
	void updateNeighbours(const int, Individual*);
	void updateExternalPopulation(Individual* ind);
	double getEuclideanDistanceFromCoeff(const vector<double>&,
		const vector<double>&) const;
	void minFastSort(vector<double>&, vector<int>&);
	double evaluateWithG(Individual*, vector<double>&);
	void improvement(Individual*);
	unique_ptr<Individual> refPoint; // Punto ideal
	vector<vector<double>> weights; // Vectores de coeficientes
	int neighSize; // Tamanio del vecindario
	vector<vector<int>> neighborhood; // Estructura del Vecindario
	vector<Individual*> exPopulation;
	// Operators Probability
	double mutationProb;
	double crossoverProb;
	string filename;
};


#endif
