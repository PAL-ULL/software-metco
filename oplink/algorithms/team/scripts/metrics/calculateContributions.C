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

int globalRepetitions;
vector<int> globalEvaluations;
map<string, int> setBases;
MOFront *referenceFront = NULL;
map< int, MOFront*> fronts;
int numInserted = 0;
int actualRep;
map< pair<string,int>, vector<long double> > values;
string baseDirectory;

//make union of fronts with the same number of evaluation and repetition
void joinFront(const string &file, const int nEvaluations, const int rep){
	MOFrontVector p(ind, true, true);
	stringstream fileName;
	fileName << file << "." << rep << "_" << nEvaluations << ".front";
	cout << "    Joining " << fileName.str() << endl;
	p.readAll(fileName.str());
	for (unsigned int i = 0; i < p.getFrontSize(); i++){
		if (fronts.count( nEvaluations) == 0)
			fronts[nEvaluations] = new MOFrontVector(ind, true, true);
		bool equals;
		p.getInd(i)->setOwner(pair<int, int> (setBases[file], 0));
		if (fronts[nEvaluations]->insert(p.getInd(i), equals)){
			fronts[nEvaluations]->getInd(fronts[nEvaluations]->getFrontSize()-1)->deleteVars();//avoid memory problems
			numInserted++;
		}
		if (equals){
			cout << "Warning: caso no contemplado 2 individuos eran iguales" << endl;
			cout << file << "." << rep << "_" << nEvaluations << ".front" << endl;
			p.getInd(i)->print(cout);
			cout << endl;
		}
	}
}

void contributionFunction(const string &baseFile, const vector<int> &nEvaluations, const int repetitions, const int type){
	cout << "Joining " << baseFile << endl;
	if (ind == NULL){
		globalEvaluations = nEvaluations;
		globalRepetitions = repetitions;
		if (type == PARALLEL_ANALYSIS){
			cout << "Tipo Parallel" << endl;
			getIndividualParallelExperiment(baseDirectory);
		}
		else if (type == SEQUENTIAL_ANALYSIS){
			cout << "Tipo sequencial" << endl;
			getIndividualSequentialExperiment(baseDirectory);
		} else{
			cout << "A UNION can not be the first to analyze in the contribution" << endl;
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
	if ((argc < 4) || ((argc %2) == 1)){
		cout << "Uso " << argv[0] << " contributionName experimentFile1 filter1 experimentFile2 filter2 experimentFile3 filter3..." << endl;
		return -1;
	}
	baseDirectory = string(dirname(argv[0])) + "/../../src/skeleton/main";
	
	globalRepetitions = 1;
	for (actualRep = 0; actualRep < globalRepetitions; actualRep++){//one repetition each time to avoid memory problems
		for (int i = 2; i < argc; i+=2)
			analyzeExperimentResults(argv[i], argv[i+1], contributionFunction, false);//suponemos que los frentes intermedios ya están calculados
	
		for (map<int, MOFront*>::iterator iter = fronts.begin(); iter != fronts.end(); iter++){
			for(map<string, int>::iterator iter2 = setBases.begin(); iter2 != setBases.end(); iter2++){
				
				int insertedByMOEA = 0;
				MOFront *front = iter->second; 
				for (int i = 0; i < front->getFrontSize(); i++){
					Owner own = front->getInd(i)->getOwner();
					if ((own.first == iter2->second) && (own.second == 0)){
						insertedByMOEA++;
					}
				}

				values[pair<string,int>(iter2->first, iter->first)].push_back(((double)(insertedByMOEA)/ iter->second->getFrontSize()));
			}
			delete(iter->second);
		}
		fronts.clear();
	}
	
	
	stringstream fileName;
	ofstream contribFile;
	for(map< pair<string,int>, vector<long double> >::iterator iter = values.begin(); iter != values.end(); iter++){
		fileName.str("");
		fileName << dirname(argv[0]) << "/" << iter->first.first << "_" << iter->first.second << ".allcontribution_" << argv[1];
		contribFile.open(fileName.str().c_str());
		if (!contribFile.is_open()){
			cout << "File " << fileName.str() << " could not be opened" << endl;
			exit(-1);
		}
		for (unsigned int i = 0; i < iter->second.size(); i++)
			contribFile << iter->second[i] << endl;
		contribFile.close();
	}
}
