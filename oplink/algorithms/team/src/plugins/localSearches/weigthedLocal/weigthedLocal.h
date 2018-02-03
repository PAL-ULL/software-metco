#ifndef __WEIGTHEDLS_H__
#define __WEIGTHEDLS_H__

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>

#include "Individual.h"
#include "LocalSearch.h"

using namespace std;

class weigthedLocal : public LocalSearch {
	public:
		void localSearch(vector <Individual*> *poblacion, MOFront *localSolution);
		void initialization(vector <Individual*> *poblacion);
		void calculate_fmax(vector <Individual*> *poblacion, unsigned int i);
		void calculate_fmim(vector <Individual*> *poblacion, unsigned int i);
		double calculate_weigthedMim(Individual* ind, unsigned int i);
		double calculate_weigthedMim(unsigned int i,vector<double> &objectives);
		double calculate_weigthedMax(Individual* ind, unsigned int i);
		double calculate_weigthedMax(unsigned int i, vector<double> &objectives);
		double calculate_F(Individual* ind, vector<int> &Objs);
		double calculate_F(vector<double> &objectives, vector<int> &Objs);
		bool init(const vector<string> &params);
	private:
		//Variables
		vector <double> maximos;
		vector <double> minimos;
		int maxIterations;
		double pLS;
		bool impresion;
};
#endif


