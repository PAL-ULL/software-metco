
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
private:
	unique_ptr<Individual> zVector; // Punto ideal
	vector<vector<double>> lambdaV; // Vectores de coeficientes
	int neighSize; // Tamanio del vecindario
	vector<vector<int>> neighborhood; // Estructura del Vecindario
	// Operators Probability
	double mutationProb;
	double crossoverProb;
public:
	// Constructor
	MOEA_D() {};
	~MOEA_D();
	// Define una generación de búsqueda del algoritmo
	void runGeneration();
	// Inicializa los parámetros iniciales del algoritmo
	bool init(const vector<string> &params);
	// Rellena un frente con las soluciones actuales
	void getSolution(MOFront *p) ;
	// Muestra la información relativa al algoritmo
	void printInfo(ostream &os) const;
	string getName() const { return "MOEA_D"; }
private:
	void initWeights();
	void initNeighborhood();
	void initZVector();
	void updateReferencePoint(vector<Individual*>*);
	double getEuclideanDistanceFromCoeff(const vector<double>&,
		const vector<double>&) const;
	void minFastSort(vector<double>&, vector<int>&);
	double evaluateWithG(Individual*, vector<double>&);
};


#endif
