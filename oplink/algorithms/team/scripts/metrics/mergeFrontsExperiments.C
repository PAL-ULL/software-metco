/************************************************************************************************************
 * Programa utilizado para realizar la mezcla de frentes obtenidos a partir de diferentes 
 * ejecuciones.
 * Todos los frentes deben tener el mismo número de parámetros y objetivos y el periodo de
 * impresión en todos ellos debe ser el mismo.
 * Paraámetros:
    - name: nombre de las uniones o del frente general
		- createUnionFronts(0|1): con 1 indicamos que se debe realizar la unión de los distintos frentes para
		                          cada uno de los periodos de impresión. 
		- unionFrontSize: tamaño máximo de los frentes que se van a crear
		- createGeneralFront(0|1): crear frente como unión de todos los frentes intermedios y sin limitar el
		                           tamaño máximo del frente
		- experiment_files: ficheros de experimentos (paralelos y secuenciales)
*************************************************************************************************************/

#include "MOFrontVector.h"
#include "parseParDefinitions.h"
#include "parseSeqDefinitions.h"
#include "parseLineDefinitions.h"
#include "getIndividuals.h"
#include "Configuration.h"
#include "coverExperiments.h"

#include <set>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sys/wait.h>
#include <libgen.h>

using namespace std;

MOFront *referenceFront = NULL;
map<int , MOFront*> fronts;
int	createUnionFronts, unionFrontSize, createGeneralFront;
vector<long double> maxObjs;
vector<long double> minObjs;
vector<int> globalEvaluations;
int globalRepetitions, actualRep;
set<string> setBases;
string baseDirectory;

//make union of fronts with the same number of evaluation and repetition
void joinFront(const string &file, const int nEvaluations){
	MOFrontVector p(ind, false, true);
	p.readAll(file);
	if (fronts.count(nEvaluations) == 0)
		fronts[nEvaluations] = new MOFrontVector(ind, true, true);
	for (unsigned int i = 0; i < p.getFrontSize(); i++)
		fronts[nEvaluations]->insert(p.getInd(i));
}

//make union of all fronts
//  file: front file
void joinGeneralFront(const string &file){
	MOFrontVector p(ind, false, true);
	p.readAll(file);
	for (unsigned int i = 0; i < p.getFrontSize(); i++){
		for (unsigned int j = 0; j < ind->getNumberOfObj(); j++){
			if (p.getInd(i)->getObj(j) > maxObjs[j]){
				maxObjs[j] = p.getInd(i)->getObj(j);
			}
			if (p.getInd(i)->getObj(j) < minObjs[j])
				minObjs[j] = p.getInd(i)->getObj(j); 
		}
		//cerr << "Va a hacer un insert" << endl;
		referenceFront->insert(p.getInd(i));//!!!!!!!!!!!
		//cerr << "Vuelve del insert" << endl;
		//if (referenceFront->getFrontSize() == 1){
			//cout << "Es 1 " << file << endl << flush;
			//referenceFront->getInd(0)->print(cout);
			//cout << endl;
		//}
	}
}
	
//join the intermediate fronts, that is with the
//same number of repetition and number of evaluation
//file: name of file with the number of repetition but 
//withouh number of evaluation
void joinIntermediateFronts(const string &file, const vector<int> &nEvaluations){
	stringstream fileName;
	for (unsigned int i = 0; i < nEvaluations.size(); i++){
		fileName.str("");
		fileName << file << "_" << nEvaluations[i] << ".front";
		if (createGeneralFront){
			//cerr << "Llama a joinGeneral" << endl << flush;
			joinGeneralFront(fileName.str());
			//cerr << "Vuelve de joinGeneral" << endl << flush;
		}
		if (createUnionFronts)
			joinFront(fileName.str(), nEvaluations[i]);
	}
}

void unionFunction(const string &baseFile, const vector<int> &nEvaluations, const int repetitions, const int type){
	//cerr << "Funcion de union llamada" << endl;
	if (ind == NULL){
		globalEvaluations = nEvaluations;
		globalRepetitions = repetitions;
		if (type == PARALLEL_ANALYSIS){
			getIndividualParallelExperiment(baseDirectory);
		} else
			getIndividualSequentialExperiment(baseDirectory);
		maxObjs.insert(maxObjs.begin(), ind->getNumberOfObj(), DBL_MIN);
		minObjs.insert(minObjs.begin(), ind->getNumberOfObj(), DBL_MAX);
	}
	if (createUnionFronts){
		if (nEvaluations.size() != globalEvaluations.size()){
			cout << "Not all experiments have the same number of evaluations" << endl;
			exit(-1);
		}
		for (unsigned int i = 0; i < nEvaluations.size(); i++){
			if (nEvaluations[i] != globalEvaluations[i]){
				cout << "Not all experiments have the same printPeriod" << endl;
			}
		}
		setBases.insert(baseFile);
	}
	//cerr << "Antes de la creacion de frente referencia" << endl;
	if ((createGeneralFront) && (referenceFront == NULL)){
		//cerr << "Va a crear frente de referencia" << endl; 
		referenceFront = new MOFrontVector(ind, true, true);
	}
	stringstream completeFile;
	completeFile << baseFile << "." << actualRep; 
	//cerr << "Va a llamar a join" << endl;
	joinIntermediateFronts(completeFile.str(), nEvaluations);
	//cerr << "Funcion de union termina" << endl;
}

int main(int argc, char *argv[]){
	if ((argc < 7) || ((argc %2) == 0)){
		cout << "Uso " << argv[0] << " unionName createUnionFronts(0|1) unionFrontSize createGeneralFront(0|1) experimentFile1 filter1 experimentFile2 filter2 experimentFile3 filter3..." << endl;
		return -1;
	}
	baseDirectory = string(dirname(argv[0])) + "/../../src/skeleton/main/";
	stringstream unionName;
	unionName << dirname(argv[0]) << "/" << argv[1];
	createUnionFronts = atoi(argv[2]);
	unionFrontSize = atoi(argv[3]);
	createGeneralFront = atoi(argv[4]);
	globalRepetitions = 1;
	for (actualRep = 0; actualRep < globalRepetitions; actualRep++){
		for (int i = 5; i < argc; i+=2){
			analyzeExperimentResults(argv[i], argv[i+1], unionFunction, ((createGeneralFront) && (actualRep == 0)));//suponemos que si no va a crear el frente Global, los frentes intermedios ya están calculados
			//analyzeExperimentResults(argv[i], argv[i+1], unionFunction, ((actualRep == 0)));//no suponemos que si no va a crear el frente Global, los frentes intermedios ya están calculados
		}
		stringstream fileName;
		stringstream fileEvaluationsName, fileRepetitionsName;
		ofstream evaluationsFile, repetitionsFile;
		if (createUnionFronts){
			for (map<int, MOFront*>::iterator iter = fronts.begin(); iter != fronts.end(); iter++){
				fileName.str("");
				fileName << unionName.str() << "." << actualRep << "_" << (iter->first * setBases.size()) << ".front";
				if (unionFrontSize != 0){
					iter->second->crowOrder(unionFrontSize);
				}
				iter->second->writeAll(fileName.str());
				delete(iter->second);
			}
			fronts.clear();
			//create .evaluations file
			fileEvaluationsName.str("");
			fileEvaluationsName << unionName.str() <<  "." << actualRep << ".evaluations";
			evaluationsFile.open(fileEvaluationsName.str().c_str(), ios::out|ios::trunc);
			evaluationsFile << globalEvaluations.size() << endl;
			for (unsigned int j = 0; j < globalEvaluations.size(); j++){
				evaluationsFile << (globalEvaluations[j]*setBases.size()) << endl;
			}		
			evaluationsFile.close();
			
			fileRepetitionsName.str("");
			fileRepetitionsName << unionName.str() << ".repetitions";
			repetitionsFile.open(fileRepetitionsName.str().c_str(), ios::out|ios::trunc);
			repetitionsFile << repetitions << endl;
			repetitionsFile.close();
		}
		//cerr << "Termina rep: " << actualRep << endl;
	}
	if (createGeneralFront){
		//cerr << "Va a crear frente general" << endl;
		referenceFront->writeAll(unionName.str());
		ofstream maxminValues((unionName.str() + ".maxmin").c_str());
		for (unsigned int i = 0; i < minObjs.size(); i++){
			maxminValues << minObjs[i] << " " << maxObjs[i] << endl;
		}
		maxminValues.close();
	}
}
