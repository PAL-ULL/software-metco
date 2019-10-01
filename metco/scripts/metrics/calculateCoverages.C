#include "MOFrontVector.h"
#include "parseParDefinitions.h"
#include "parseSeqDefinitions.h"
#include "parseLineDefinitions.h"
#include "getIndividuals.h"
#include "Configuration.h"
#include "coverExperiments.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sys/wait.h>
#include <libgen.h>

using namespace std;

int globalRepetitions, actualRep;
vector<int> globalEvaluations;
map<string, int> setBases;
MOFront *referenceFront = NULL;
map<int, MOFront*> fronts;
map< pair<string, int >, int > originalSizes;
map< int, vector<long double> > values1, values2;
string baseDirectory;

//make union of fronts with the same number of evaluation and repetition
void joinFront(const string &file, const int nEvaluations, const int rep){
	MOFrontVector p(ind, true, true);
	stringstream fileName;
	fileName << file << "." << rep << "_" << nEvaluations << ".front";
	p.readAll(fileName.str());
	originalSizes[pair<string, int >(file, nEvaluations)] = p.getFrontSize();
	for (unsigned int i = 0; i < p.getFrontSize(); i++){
		if (fronts.count(nEvaluations) == 0)
			fronts[nEvaluations] = new MOFrontVector(ind, true, true);
		bool equals;
		p.getInd(i)->setOwner(pair<int, int>(setBases[file], 0));
		fronts[nEvaluations]->insert(p.getInd(i), equals);
		if (equals){
			cout << "Warning: caso no contemplado 2 individuos eran iguales" << endl;
			cout << file << "." << rep << "_" << nEvaluations << ".front" << endl;
			p.getInd(i)->print(cout);
			cout << endl;
		}
	}
}

void coverageFunction(const string &baseFile, const vector<int> &nEvaluations, const int repetitions, const int type){
	if (ind == NULL){
		globalEvaluations = nEvaluations;
		globalRepetitions = repetitions;
		if (type == PARALLEL_ANALYSIS)
			getIndividualParallelExperiment(baseDirectory);
		else if (type == SEQUENTIAL_ANALYSIS){
			getIndividualSequentialExperiment(baseDirectory);
		} else {
			cout << "Error: a Union can not be the first to analyze in the Coverage" << endl;
		}
	}
	if (setBases.count(baseFile) == 0){
		int id = setBases.size();
		setBases[baseFile] = id;
	}
	if (nEvaluations.size() != globalEvaluations.size()){
		cout << "Not all experiments have the same number of evaluations" << endl;
		exit(-1);
	}
	for (unsigned int i = 0; i < nEvaluations.size(); i++){
		if (nEvaluations[i] != globalEvaluations[i]){
			cout << "Not all experiments have the same printPeriod" << endl;
		}
	}
	if (repetitions != globalRepetitions){
		cout << "Not all experiments have the same number or repetitions" << endl;
		exit(-1);
	}
	for (unsigned int j = 0; j < nEvaluations.size(); j++)
		joinFront(baseFile, nEvaluations[j], actualRep);
}

int main(int argc, char *argv[]){
	if (argc != 5){
		cout << "Uso " << argv[0] << " experimentFile1 filter1 experimentFile2 filter2" << endl;
		return -1;
	}
	string expFile1, expFile2, filter1, filter2, baseFile1, baseFile2, baseNDFile1, baseNDFile2;
	expFile1 = argv[1];
	filter1 = argv[2];
	expFile2 = argv[3];
	filter2 = argv[4];
	baseDirectory = string(dirname(argv[0])) + "/../../src/skeleton/main";
	globalRepetitions = 1;
	for (actualRep = 0; actualRep < globalRepetitions; actualRep++){
		analyzeExperimentResults(expFile1, filter1, coverageFunction, false);
		analyzeExperimentResults(expFile2, filter2, coverageFunction, false);

		if (setBases.size() != 2){
			cout << "Error: exactamente 2 frentes deben ser considerados para el cubrimiento" << endl;
			exit(-1);
		}

		baseFile1 = setBases.begin()->first;
		baseFile2 = (++setBases.begin())->first;
		char *tmp;
		tmp = new char[baseFile1.size() + 1];
		strcpy(tmp, baseFile1.c_str());
		baseNDFile1 = basename(tmp);
		delete tmp;
		tmp = new char[baseFile2.size() + 1];
		strcpy(tmp, baseFile2.c_str());
		baseNDFile2 = basename(tmp);
		delete tmp;

		stringstream fileName;
		ofstream coverFile;
		int frontSize, eval;
		for (map<int, MOFront*>::iterator iter = fronts.begin(); iter != fronts.end(); iter++){
			frontSize = originalSizes[pair<string, int >(baseFile2,  eval)];

			int inserted = 0;
			MOFront *front = iter->second; 
			for (int i = 0; i < front->getFrontSize(); i++){
				Owner own = front->getInd(i)->getOwner();
				if (own.first == setBases[baseFile2]){
					inserted++;
				}
			}
			values1[iter->first].push_back((((double)(frontSize - inserted))/frontSize));
			frontSize = originalSizes[pair<string, int >(baseFile1,  eval)];
			
			inserted = 0;
			for (int i = 0; i < front->getFrontSize(); i++){
				Owner own = front->getInd(i)->getOwner();
				if (own.first == setBases[baseFile1]){
					inserted++;
				}
			}
			values2[iter->first].push_back((((double)(frontSize - inserted))/frontSize));
			delete(iter->second);
		}
		fronts.clear();
	}
	
	stringstream fileName;
	ofstream coverFile;
	for(map< int, vector<long double> >::iterator iter = values1.begin(); iter != values1.end(); iter++){
		fileName.str("");
		fileName << dirname(argv[0]) << "/" << baseFile1 << "_" << iter->first << ".allcover_" << baseNDFile2;
		coverFile.open(fileName.str().c_str());
		if (!coverFile.is_open()){
			cout << "File " << fileName.str() << " could not be opened" << endl;
			exit(-1);
		}
		for (unsigned int i = 0; i < iter->second.size(); i++)
			coverFile << iter->second[i] << endl;
		coverFile.close();
	}
	
	for(map< int, vector<long double> >::iterator iter = values2.begin(); iter != values2.end(); iter++){
		fileName.str("");
		fileName << dirname(argv[0]) << "/" << baseFile2 << "_" << iter->first << ".allcover_" << baseNDFile1;
		coverFile.open(fileName.str().c_str());
		if (!coverFile.is_open()){
			cout << "File " << fileName.str() << " could not be opened" << endl;
			exit(-1);
		}
		for (unsigned int i = 0; i < iter->second.size(); i++)
			coverFile << iter->second[i] << endl;
		coverFile.close();
	}

}
