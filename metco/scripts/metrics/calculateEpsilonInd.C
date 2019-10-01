#include <stdlib.h>
#include <libgen.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "MinimizationGenericIndividual.h"
#include "Individual.h"
#include "MOFrontVector.h"

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
		Individual *ind = getInd(baseDirectory, argv[3], indParams, true);
		if (ind == NULL){
			cout << "Error creando el individuo" << endl;
			exit(-1);
		}
		//The fronts are normalized and minimization is considered
		Individual *genericInd = new MinimizationGenericIndividual();
		genericInd->setNumberOfObj(ind->getNumberOfObj());
		genericInd->setNumberOfVar(ind->getNumberOfVar());
		//Read front
		MOFrontVector front(ind, true, true);
		front.read(argv[1]);
		//Read reference front
		MOFrontVector referenceFront(ind, true, true);
		referenceFront.read(argv[2]);

		cout << front.getEpsilonIndicator(referenceFront);
}
