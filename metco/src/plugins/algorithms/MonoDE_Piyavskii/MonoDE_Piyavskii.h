/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 04/01/13
* Descripcion: clase que implementa la funcionalidad de una 
* evolucion diferencial, y que incorpora las ideas del teorema de Piyavskii
* Codigo basado en 
* http://sci2s.ugr.es/eamhco/SourceCodeExcel/P03-WangWuRahnamayan/Code_GODE.rar
************************************************************************/

#ifndef _MONO_DE_H_
#define _MONO_DE_H_

#include <map>
#include "Individual.h"
#include "EA.h"

using namespace std;

class MonoDE_Piyavskii : public EA {
	public:
		virtual ~MonoDE_Piyavskii(){}
		bool init(const vector<string> &params);
		void getSolution(MOFront *p);
		void printInfo(ostream &os) const ;
		void fillPopWithNewIndsAndEvaluate();
	private:
		void eval(Individual *ind);
		void Rand1Exp(int candidate);
		void variation();
		void checkSanity() const;//Check if the parameters are correct
		void printBestIndividual();
		void runGeneration();
		void printDiversity();
		void updateKValuesWithLast();

		map<string, string> parameters;
		int ps;//population size
		double F;
		double CR;
		double bestObjective;
		int optDirection;
		double k_slope;

		vector<Individual*> evaluated;
		vector<double> minDistances;
		vector<double> estimatedKs;
		vector<int> nearests;
};

#endif
