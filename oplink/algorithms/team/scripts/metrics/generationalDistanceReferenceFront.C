/***********************************************************************************************************
 * Calcula la distancia generacional de un frente dado un frente referencia. Comprueba que el frente 
 * referencia sea realmente mejor que el frente que se está evaluando y que todos los puntos del frente 
 * referencia sean puntos no dominados entre si
 *
 * Uso:
 *    generationalDistanceReferenceFront Front ReferenceFront Individual IndParam1 IndParam2 ... IndParamN 
 * *********************************************************************************************************/

#include <stdlib.h>
#include <libgen.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>

using namespace std;

#include "Individual.h"
#include "MOFrontVector.h"

double euclideanDistance(vector<double> v1, vector<double> v2){
	double acum = 0;
	if (v1.size() != v2.size()){
		cout << "Error: calculating euclidenDistance of vector with diferent size" << endl;
		exit(-1);
	}
	for (unsigned int i = 0; i < v1.size(); i++){
		acum += (v1[i] - v2[i]) * (v1[i] - v2[i]);
	}
	return sqrt(acum);
}

int main(int argc, char *argv[]){
		if (argc < 4){
			cout << "Uso " << argv[0] << " Front ReferenceFront Individual [IndividualParam_1 IndividualParam_2 ... IndividualParam_N]" << endl;
			exit(-1);
		}
		vector<string> indParams;
		for (int i = 4; i < argc; i++){
			indParams.push_back(argv[i]);
		}
		string baseDirectory = string(dirname(argv[0])) + "/../../src/skeleton/main/";
		void *handle;
		Individual *ind = getInd(baseDirectory, argv[3], indParams, true);
		if (ind == NULL){
			cout << "Error creando el individuo" << endl;
			exit(-1);
		}

		//Create reference Front
		MOFront *referenceFront = new MOFrontVector(ind, true, true);
		ifstream referenceFrontFile(argv[2]);
		if (!referenceFrontFile.is_open()){
			cout << "Reference front could not be opened" << endl;
			exit(-1);
		}
		vector< vector <double> > referenceObjectives;
		while(!referenceFrontFile.eof()){
			vector<double> objectives;
			objectives.clear();
			for (unsigned int i = 0; i < ind->getNumberOfVar(); i++){
				double value;
				referenceFrontFile >> value;
				ind->setVar(i, value);
			}
			for (unsigned int i = 0; i < ind->getNumberOfObj(); i++){
				double value;
				referenceFrontFile >> value;
				ind->setObj(i, value);
				objectives.push_back(value);
			}
			if (!referenceFrontFile.eof()){
				int borrados;
				if (!referenceFront->insert(ind, borrados)){
					cout << "Incorrect Reference Front" << endl;
					exit(-1);
				}
				if (borrados != 0){
					cout << "Incorrect Reference Front" << endl;
					exit(-1);
				}
				referenceObjectives.push_back(objectives);
			}
		}
		referenceFrontFile.close();

		//Analyze front file
		double acumulatedGD = 0;
		int nInds = 0;
		ifstream frontFile(argv[1]);
		if (!frontFile.is_open()){
			cout << "front could not be opened" << endl;
			exit(-1);
		}
		while(!frontFile.eof()){
			vector<double> objectives;
			objectives.clear();
			for (unsigned int i = 0; i < ind->getNumberOfVar(); i++){
				double value;
				frontFile >> value;
				ind->setVar(i, value);
			}
			for (unsigned int i = 0; i < ind->getNumberOfObj(); i++){
				double value;
				frontFile >> value;
				ind->setObj(i, value);
				objectives.push_back(value);
			}
			if (!frontFile.eof()){
				int borrados;
				if (referenceFront->insert(ind, borrados)){
					cout << "Incorrect Reference Front. Element of the front inserted in the reference front" << endl;
					exit(-1);
				}
				if (borrados != 0){
					cout << "Incorrect Reference Front. Element of the reference front dominated by element in the front" << endl;
					exit(-1);
				}
				double minimumEucDis = DBL_MAX;
				for (unsigned int i = 0; i < referenceObjectives.size(); i++){
					double eucDistance = euclideanDistance(referenceObjectives[i], objectives);
					if (eucDistance < minimumEucDis)
						minimumEucDis = eucDistance;
				}
				acumulatedGD += minimumEucDis;
				nInds++;
			}
		}
		referenceFrontFile.close();
		cout << acumulatedGD / nInds << endl;
}
