#include "metricDefinitions.h"
#include "Configuration.h"
#include "parseSeqDefinitions.h"
#include "parseParDefinitions.h"
#include "parseLineDefinitions.h"
#include "getIndividuals.h"
#include "MOFrontVector.h"
#include "coverExperiments.h"

#include <algorithm>
#include <sys/wait.h>
#include <string.h>
#include <numeric>
#include <set>
#include <map>
#include <iostream>
#include <sstream>
#include <libgen.h>
#include <fstream>


using namespace std;

string baseDirectory;
string exeDir, sufixName;
vector<long double> maxObjs;
vector<long double> minObjs;

void normalizeReferenceFront(MOFront *p1, MOFront *pnorm, const vector<long double> &maxObjs, const vector<long double> &minObjs){
	pnorm->clear();
	Individual *newInd = p1->getInd(0);
	newInd->setNumberOfObj(2);//!!!!!!!!!!!!!!!!!!!!!!!!!
	if (maxObjs.size() != newInd->getNumberOfObj()){
		cout << "Error: number of objectives do not match" << endl;
		exit(-1);
	}
	for (unsigned int i = 0; i < p1->getFrontSize(); i++){
		if (!pnorm->insert(p1->getInd(i))){
			cout << "Error: non-dominated individual in front" << endl;
			exit(-1);
		}
	}
	for (unsigned int i = 0; i < pnorm->getFrontSize(); i++){
		Individual *actualInd = pnorm->getInd(i);
		for (unsigned int j = 0; j < ind->getNumberOfObj(); j++){
			if (actualInd->getInternalOptDirection(j) == MINIMIZE)
				actualInd->setObj(j, ((actualInd->getObj(j) - minObjs[j])/(maxObjs[j] - minObjs[j])));
			else{
				cerr << "ENTRA" << endl;
				actualInd->setObj(j, (1 - ((actualInd->getObj(j) - minObjs[j])/(maxObjs[j] - minObjs[j]))));
				//actualInd->setObj(j, (0 - ((actualInd->getObj(j) - minObjs[j])/(maxObjs[j] - minObjs[j]))));			//OJOOO monoobjetivo con falso objetivo
			}
		}
	}	
}

long double calculateGenerationalDistance(const string &frontFile){
	stringstream cmd;
	return 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	cmd << "./generationalDistanceReferenceFront " << frontFile << " " << generalFrontName << " " << individualName << " " << individualParams;
	cout << "Ejecuta: " << cmd.str() << endl;
	FILE *gdValueFile = popen(cmd.str().c_str(), "r");
	if (gdValueFile == NULL){
		cout << "error executing " << cmd.str();
		exit(-1);
	}
	if (wait(NULL) == -1){//wait termination
		cout << "Error waiting child termination" << endl;
		exit(-1);
	}
	long double gdValue;
	fscanf(gdValueFile, "%lf", &gdValue);
	pclose(gdValueFile);
	return gdValue;
}

void generateFileForSurface(const string &baseFile, const int evaluations, const int repetitions){
	stringstream outputFile;
	outputFile << baseFile << "_" << evaluations << ".obj";
	ofstream objFile(outputFile.str().c_str());
  objFile.precision(18);
	for (int currentRep = 0; currentRep < repetitions; currentRep++){
		MOFrontVector p(ind, false, true);
		stringstream frontFile;	
		frontFile << baseFile << "." << currentRep << "_" << evaluations << ".front";
		p.clear();
		p.readAll(frontFile.str().c_str());
		for (unsigned int i = 0; i < p.getFrontSize(); i++){
			for (int j = 0; j < p.getInd(i)->getNumberOfObj(); j++){
				objFile << p.getInd(i)->getObj(j) << " ";
			}
			objFile << endl;
		}
		objFile << endl;
	}
	objFile.close();
}

long double calculateHypervolume(const string &frontFile){
	ind->setNumberOfObj(2);
	MOFrontVector p(ind, false, true);
	cout << "Numero de objetivos: " << ind->getNumberOfObj() << endl;
	p.readAll(frontFile);
	bool inserted = false;
	if (p.getFrontSize() == 0){
		cout << "Error loading " << frontFile << endl;
		exit(-1);
	}
	ofstream objFile((frontFile + ".normObj").c_str());
  objFile.precision(18);
	cout << "Front size: " << p.getFrontSize() << endl;
	long double minValue = LDBL_MAX;
	long double maxValue = -LDBL_MAX;
	for (unsigned int i = 0; i < p.getFrontSize(); i++){
		Individual *actInd = p.getInd(i);
		//if (actInd->getObj(0) + actInd->getObj(1) < minValue) { //!!!!!!!!!!!!!!!!!!!!!!!
		if (actInd->getObj(0) < minValue){  //!!!!!!!!!!!!!!!!!!!!!!!
			//minValue = actInd->getObj(0) + actInd->getObj(1);
			minValue = actInd->getObj(0);
		}
		if (actInd->getObj(0) > maxValue){
			maxValue = actInd->getObj(0);
		}
		//comprobamos que todos los objetivos transformados sean menor que 1
		bool error = false;
		//{ int j = 0; //FORZAMOS MONOOBJETIVO	
		for (unsigned int j = 0; j < ind->getNumberOfObj(); j++) {
			if (ind->getOptDirection(j) == MINIMIZE) {
				if (((actInd->getObj(j) - minObjs[j])/(long double)(maxObjs[j] - minObjs[j])) > 1){
					error = true;
          cout << "ERROR: El valor normalizado del objetivo " << j << " es superior a 1" << endl;
					break;
				}
			} else if ((1 - ((actInd->getObj(j) - minObjs[j])/(long double)(maxObjs[j] - minObjs[j]))) > 1){
				error = true;
        cout << "ERROR: El valor normalizado del objetivo " << j << " es superior a 1" << endl;
				break;
			}
		}
		if (!error){//!!!!!!!!!!!!
			inserted = true;
			//{
		  	//int j = 0; //forzamos mono
			for (unsigned int j = 0; j < ind->getNumberOfObj(); j++){
				if (ind->getInternalOptDirection(j) == MINIMIZE)
					objFile << ((actInd->getObj(j) - minObjs[j])/(long double)(maxObjs[j] - minObjs[j])) << " ";
				else
					objFile << (1 - ((actInd->getObj(j) - minObjs[j])/(long double)(maxObjs[j] - minObjs[j]))) << " ";
					//objFile << (0 - ((actInd->getObj(j) - minObjs[j])/(maxObjs[j] - minObjs[j]))) << " ";//!!!!VALIDO PARA MONO+ALTERNATIVOS
			}
			objFile << endl;
		}//!!!!!!!!!!!!!!!
  //}
	}
	objFile.close();

	stringstream cmd;
	long double hvValue = 0;

	if (inserted){
		cmd << "./hv-1.1-src/hv -r \"";
		for (unsigned int i = 0; i < ind->getNumberOfObj(); i++)//Para multiobjetivo!!!
			cmd << "1 ";
		//Para monoobjetivo
		//cmd << "0 ";

		cmd << "\" " << frontFile << ".normObj";
		cout << "Ejecuta: " << cmd.str() << endl;
		FILE *hvValueFile = popen(cmd.str().c_str(), "r");
		if (hvValueFile == NULL){
			cout << "Error executing " << cmd.str() << endl << flush;
			exit(-1);
		}
		if (wait(NULL) == -1){//wait termination
			cout << "Error waiting child termination" << endl << flush;
			exit(-1);
		}
		fscanf(hvValueFile, "%Lf", &hvValue);
		pclose(hvValueFile);
	}

	// return minValue;//!!!!!	
	// return maxValue;
  cout << "hvValue: " << hvValue << endl;
	return hvValue; //Para multi-objetivo
	//return -hvValue; //Para el uniobjetivo (monoobjetivo)
}

long double calculateMonoRND(const string &frontFile){
	MOFrontVector p(ind, false, true);
	p.readAll(frontFile);
	bool inserted = false;
	if (p.getFrontSize() == 0){
		cout << "Error loading " << frontFile << endl;
		exit(-1);
	}
	long double value = 0;
	for (unsigned int i = 0; i < p.getFrontSize(); i++){
		long double usedBs = p.getInd(i)->getObj(0);
		long double coverage = p.getInd(i)->getObj(1);
		long double coverageRate = (coverage * 100.0) / (300 * 450);
		long double fitness;
		if (usedBs != 0){
			fitness = (coverageRate * coverageRate) / usedBs;
			if (fitness > value)
				value = fitness;
		}
	}
	return value;
}


long double calculateEpsilonIndicator(const string &frontFile){
	stringstream cmd;
	cmd << "./calculateEpsilonInd " << frontFile << ".normObj" << " " << generalFrontName << ".normObj" << " " << individualName << " " << individualParams;;
	cout << "Ejecuta: " << cmd.str() << endl;
	FILE *epsValueFile = popen(cmd.str().c_str(), "r");
	if (epsValueFile == NULL){
		cout << "Error executin " << cmd.str() << endl;
		exit(-1);
	}
	if (wait(NULL) == -1){
		cout << "Error waiting child termination " << cmd.str() << endl;
		exit(-1);
	}
	long double epsValue;
	fscanf(epsValueFile, "%lf", &epsValue);
	pclose(epsValueFile);
	return epsValue;
}

void analyzeFront(const string &baseFile, int evaluations, int repetitions){
	vector<long double> gdValues, hyperValues, epsValues, monoValues;
	stringstream frontFile;
	cout << "Numero de repeticiones: " << repetitions << endl;
	for (int i = 0; i < repetitions; i++){
		frontFile.str("");
		frontFile << baseFile << "." << i << "_" << evaluations << ".front";
		//!!!gdValues.push_back(calculateGenerationalDistance(frontFile.str()));
		cout << "Calculo de hipervolumen para " << i << endl;
		hyperValues.push_back(calculateHypervolume(frontFile.str()));
		//monoValues.push_back(calculateMonoRND(frontFile.str()));
		//!!!epsValues.push_back(calculateEpsilonIndicator(frontFile.str()));
	}//

	generateFileForSurface(baseFile, evaluations, repetitions);//!!!!multi-objetivo
	
	/*!!!stringstream gdValuesFileName;
	gdValuesFileName << baseFile << "_" << evaluations << ".allGD";
	ofstream gdValuesFile(gdValuesFileName.str().c_str());
	if (!gdValuesFile.is_open()){
		cout << "File " << gdValuesFileName.str() << " could not be open" << endl;
		exit(-1);
	}
	for (unsigned int i = 0; i < gdValues.size(); i++){
		gdValuesFile << gdValues[i] << endl;
	}
	gdValuesFile.close();*/

	stringstream hyperValuesFileName;
	hyperValuesFileName << baseFile << "_" << evaluations << ".allHV";
	ofstream hyperValuesFile(hyperValuesFileName.str().c_str());
  hyperValuesFile.precision(18);
	if (!hyperValuesFile.is_open()){
		cout << "File " << hyperValuesFileName.str() << " could not be open" << endl;
		exit(-1);
	}
	for (unsigned int i = 0; i < hyperValues.size(); i++){
		hyperValuesFile << hyperValues[i] << endl;
	}
	hyperValuesFile.close();

	
	/*stringstream MonoRNDValuesFileName;
	MonoRNDValuesFileName << baseFile << "_" << evaluations << ".allMonoRND";
	ofstream MonoRNDValuesFile(MonoRNDValuesFileName.str().c_str());
	if (!MonoRNDValuesFile.is_open()){
		cout << "File " << MonoRNDValuesFileName.str() << " could not be open" << endl;
		exit(-1);
	}
	for (unsigned int i = 0; i < monoValues.size(); i++){
		MonoRNDValuesFile << monoValues[i] << endl;
	}
	MonoRNDValuesFile.close();*/
   
	/*!!!stringstream epsValuesFileName;
	epsValuesFileName << baseFile << "_" << evaluations << ".allEpsInd";
	ofstream epsValuesFile(epsValuesFileName.str().c_str());
	if (!epsValuesFile.is_open()){
		cout << "File " << epsValuesFileName.str() << " could not be open" << endl;
		exit(-1);
	}
	for (unsigned int i = 0; i < epsValues.size(); i++){
		epsValuesFile << epsValues[i] << endl;
	}
	epsValuesFile.close();*/

	
}

void analyzeFile(const string &baseFile, int repetitions, const vector<int> &evaluations){
/*	ifstream fileNEvaluations((baseFile + ".0.evaluations").c_str());
	if (fileNEvaluations.is_open() == false){
		cout << "File " << baseFile << ".0.evaluations could not be opened" << endl;
		return;
	}
	int nEvaluations;
	fileNEvaluations >> nEvaluations;
	for (int i = 0; i < nEvaluations; i++){
		int n;
		fileNEvaluations >> n;
		analyzeFront(baseFile, n, repetitions);
	}
	fileNEvaluations.close();*/
	for (unsigned int i = 0; i < evaluations.size(); i++){
		analyzeFront(baseFile, evaluations[i], repetitions);
	}
}



void write2File(const string &fileName, const vector<long double> &values, const vector<int> &index){
	ofstream file(fileName.c_str(), ios::in|ios::out|ios::trunc);
  file.precision(18);
	if (!file.is_open()){
		cout << "file " << fileName << " could not be opened for write" << endl;
		exit(-1);
	}
	if (values.size() != index.size()){
		cout << "Number of indexes and values do not match" << endl;
		exit(-1);
	}
	for (unsigned int i = 0; i < values.size(); i++){
		file << index[i] << " " << values[i] << endl;
	}
	file.close();
}

void getMaxMinObjs(){
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/*	ifstream maxminValues((generalFrontName + ".maxmin").c_str());
	if (!maxminValues.is_open()){
		cout << "Error loading " << generalFrontName  << ".maxmin" << endl;
		exit(-1);
	}
	for (unsigned int j = 0; j < ind->getNumberOfObj(); j++){
		double min, max;
		maxminValues >> min >> max;
		minObjs.push_back(min);
		maxObjs.push_back(max);
	}
	maxminValues.close();*/
	//RND!!!!!!!!
	//minObjs.push_back(0);
	//minObjs.push_back(0);
	//Malaga
	//maxObjs.push_back(293);
	//maxObjs.push_back(128955);
	//La Laguna
	//maxObjs.push_back(293);
	//maxObjs.push_back(100);


	//ZDT1: min -> 0 0; max -> 1 2
	//ZDT2: min-> 0 0; max -> 1 2
	//ZDT3: min -> 0 -1; max -> 1 2.5
	//ZDT4: min -> 0 0; max -> 1.1 4
	//WFGx: min -> 0 0; max -> 2.5 4.5
	//Highway: min-> 0 0 1.74; max-> 40 1 30
	//MALL: min-> 0 0 0; max-> 42 1 10.75
	//KNAP_100_2: min -> 3235 3215; max -> 4266 4037
	//KNAP_250_2: min -> 7283 7570; max -> 9893 10103
	//KNAP_500_2: min -> 15780 16315; max -> 20094 20490
	/*MANET_HIghway*/

	//minObjs.push_back(0);
	//minObjs.push_back(0);
  //maxObjs.push_back(2.5);
  //maxObjs.push_back(4.5);
	
	//monoobjetivo sin normalizar
	minObjs.push_back(21.3);
	maxObjs.push_back(75.2);
	minObjs.push_back(86.6);
	maxObjs.push_back(850.4);

	/*minObjs.push_back(100);
	minObjs.push_back(75);
	maxObjs.push_back(150);
	maxObjs.push_back(400);*/
}

void calculateValues(const string &frontFile, const string &prefix, long double &av, long double &max, long double &min, long double &med, int repetitions){
	vector<long double> listValues;
	//all Values
	ifstream allFile((frontFile + ".all" + prefix).c_str());
  allFile.precision(18);
	if (!allFile.is_open()){
		cout << "File " << frontFile << ".all" << prefix << " could not be opened" << endl;
		exit(-1);
	}
	for(int i = 0; i < repetitions; i++){
		long double value;
		allFile >> value;
		if (!allFile.fail()){
			listValues.push_back(value);
		} else {
			cout << "Error reading file " << frontFile << ".all" << prefix << endl;
			exit(-1);
		}
	}
	allFile.close();
	
	//average Value
	ofstream avgFile((frontFile + ".av" + prefix).c_str());
  avgFile.precision(18);
  cout.precision(18);
	avgFile << (av = accumulate(listValues.begin(), listValues.end(), 0.)/listValues.size()) << endl;
	avgFile.close();

	//Maximum Value
	ofstream maxFile((frontFile + ".max" + prefix).c_str());
  maxFile.precision(18);
	maxFile << (max = *max_element(listValues.begin(), listValues.end()));
	maxFile.close();

	//Minimum Value
	ofstream minFile((frontFile + ".min" + prefix).c_str());
  minFile.precision(18);
	minFile << (min = *min_element(listValues.begin(), listValues.end()));
	minFile.close();
	
	//Median Value
	ofstream medFile((frontFile + ".med" + prefix).c_str());
  medFile.precision(18);
	sort(listValues.begin(), listValues.end());
	med = ((listValues.size() % 2) == 0) ? ((listValues[listValues.size() / 2] + listValues[listValues.size() / 2 - 1])/2) : (listValues[listValues.size() / 2]);
	medFile << med << endl;
	medFile.close();

	listValues.clear();
}

void analyzeValuesFrontFile(const string &frontFile, const unsigned int repetitions, const vector<int> &listEvaluations, const string &sufix, bool calculateBest, bool maximize){
	//Obtain each values
	vector<long double> minValues, maxValues, avgValues, bestAvgValues, medValues;
	stringstream file;
	ifstream dataFile;
  dataFile.precision(18);
	for (unsigned int i = 0; i < listEvaluations.size(); i++){
		stringstream outFile;
		outFile << frontFile << "_" << listEvaluations[i];
		long double av, max, min, med;
		calculateValues(outFile.str() , sufix, av, max, min, med, repetitions); 
		avgValues.push_back(av);
		maxValues.push_back(max);
		minValues.push_back(min);
		medValues.push_back(med);
		if (calculateBest){
			if (bestAvgValues.size() == 0)
				bestAvgValues.push_back(av);
			else {
				if (maximize){
					bestAvgValues.push_back((bestAvgValues[bestAvgValues.size()-1] > av)?(bestAvgValues[bestAvgValues.size()-1]):av);
				} else{
					bestAvgValues.push_back((bestAvgValues[bestAvgValues.size()-1] < av)?(bestAvgValues[bestAvgValues.size()-1]):av);
				}
			}
		}
	}
	if (calculateBest)
		write2File(frontFile + ".bestAvg" + sufix + ".evolution", bestAvgValues, listEvaluations);
	write2File(frontFile + ".avg" + sufix + ".evolution", avgValues, listEvaluations);
	write2File(frontFile + ".max" + sufix + ".evolution", maxValues, listEvaluations);
	write2File(frontFile + ".min" + sufix + ".evolution", minValues, listEvaluations);
	write2File(frontFile + ".med" + sufix + ".evolution", medValues, listEvaluations);
}

void analyzeValuesFrontFile(const string &frontFile, const unsigned int repetitions, const vector<int> &listEvaluations){
	//!!!!!!!!!!!analyzeValuesFrontFile(frontFile, repetitions, listEvaluations, "GD", true, true);
	analyzeValuesFrontFile(frontFile, repetitions, listEvaluations, "HV", true, true);
	//analyzeValuesFrontFile(frontFile, repetitions, listEvaluations, "MonoRND", true, true);
	//!!!!!!!!!!analyzeValuesFrontFile(frontFile, repetitions, listEvaluations, "EpsInd", true, false);
}

void calculateMerges(){
	stringstream cmd;
	//calculate merge of all
	cmd << exeDir << "/mergeFrontsExperiments " << generalFrontName << " 0 0 1 ";
	for (unsigned int i = 0; i < experimentFiles.size(); i++)
		cmd << experimentFiles[i] << " \\* ";
	cout << "Ejecuta: " << cmd.str() << endl << flush;
	system(cmd.str().c_str());
	//calculate unions
	for (unsigned int i = 0; i < unions.size(); i++){
		cmd.str("");
		cmd << exeDir << "/mergeFrontsExperiments " << unions[i].second.first << " 1 " << unions[i].second.second << " 0 ";
		for (unsigned int j = 0; j < unions[i].first.size(); j++){
			cmd << unions[i].first[j].first << " " << unions[i].first[j].second << " ";
		}
		cout << "Ejecuta: " << cmd.str() << endl;
		system(cmd.str().c_str());
	}
}

void metricCalculator(const string &baseFile, const vector<int> &evaluations, const int repetitions, const int type){
	if (ind == NULL){
		if (type == PARALLEL_ANALYSIS)
			getIndividualParallelExperiment(baseDirectory);
		 else if (type == SEQUENTIAL_ANALYSIS){
			getIndividualSequentialExperiment(baseDirectory);
		}
		else if (type == UNION_ANALYSIS) {
			cout << "Error interno" << endl << flush;
			exit(-1);
		}
		getMaxMinObjs();
		MOFrontVector p(ind, false, true);
		p.readAll(generalFrontName);
		cout << "Lee el frente de referencia de: " << generalFrontName << endl;
		MOFrontVector pnorm(ind, true, true);
		normalizeReferenceFront(&p, &pnorm, maxObjs, minObjs);
		pnorm.write(generalFrontName + ".normObj");
	}

	analyzeFile(baseFile, repetitions, evaluations);
	analyzeValuesFrontFile(baseFile, repetitions, evaluations);
}

void calculateMetricExperiments(){
	for (unsigned int i = 0; i < experimentFiles.size(); i++){
		analyzeExperimentResults(experimentFiles[i], "*", metricCalculator, false);
	}
}

void deleteFront(const string &baseFile, const vector<int> &evaluations, const int repetitions, const int type){
	stringstream cmd;
	for (int i = 0; i < repetitions; i++){
		for (unsigned int j = 0; j < evaluations.size(); j++){
			cmd.str("");
			cmd << "unlink " << baseFile << "." << i << "_" << evaluations[j] << ".front";
			system(cmd.str().c_str());
			cmd.str("");
			cmd << "unlink " << baseFile << "." << i << "_" << evaluations[j] << ".front.normObj";
			system(cmd.str().c_str());
		}
		cmd.str("");
		cmd << "unlink " << baseFile << "." << i << ".evaluations";
		system(cmd.str().c_str());
	}

	for (unsigned int j = 0; j < evaluations.size(); j++){
		cmd.str("");
		cmd << "unlink " << baseFile << "_" << evaluations[j] << ".obj";
		system(cmd.str().c_str());
	}
}

void deleteFrontFiles(){
	//delete front files of experiments
	for (unsigned int i = 0; i < experimentFiles.size(); i++){
		analyzeExperimentResults(experimentFiles[i], "*", deleteFront, false);
	}
	//delete front files of unions
	stringstream cmd;
	for (unsigned int i = 0; i < unions.size(); i++){
		vector<int> nEvaluations;
		ifstream evalFile((unions[i].second.first + ".0.evaluations").c_str());
    evalFile.precision(18);
		if (!evalFile.is_open()){
			cout << "Error: no se pudo abrir " << unions[i].second.first << ".0.evaluations" << endl;
			exit(-1);
		}
		int nEval;
		evalFile >> nEval;
		for (int j = 0; j < nEval; j++){
			int val;
			evalFile >> val;
			nEvaluations.push_back(val);
		}
		evalFile.close();
		for (unsigned int j = 0; j < repetitions; j++){
			for (unsigned int k = 0; k < nEvaluations.size(); k++){
				cmd.str("");
				cmd << "unlink " << unions[i].second.first << "." << j << "_" << nEvaluations[k] << ".front";
				system(cmd.str().c_str());
				cmd.str("");
				cmd << "unlink " << unions[i].second.first << "." << j << "_" << nEvaluations[k] << ".front.normObj";
				system(cmd.str().c_str());
			}
			cmd.str("");
			cmd << "unlink " << unions[i].second.first << "." << j << ".evaluations";
			system(cmd.str().c_str());
		}
	}

}

void valuesCalculator(const string &baseFile, const vector<int> &evaluations, const int repetitions, const int type){
	analyzeValuesFrontFile(baseFile, repetitions, evaluations, sufixName, false, false);
}

void calculateValuesContributions(){
	for (unsigned int i = 0; i < contributions.size(); i++){
		sufixName = string("contribution_") + contributions[i].second;
		for (unsigned int j = 0; j < contributions[i].first.size(); j++){
			analyzeExperimentResults(contributions[i].first[j].first, contributions[i].first[j].second, valuesCalculator, false);
		}
	}
}

void calculateMetricUnions(){
	stringstream file;
	for (unsigned int i = 0; i < unions.size(); i++){
		vector<int> nEvaluations;
		ifstream evalFile((unions[i].second.first + ".0.evaluations").c_str());
    evalFile.precision(18);
		int nEval;
		evalFile >> nEval;
		for (int j = 0; j < nEval; j++){
			int val;
			evalFile >> val;
			nEvaluations.push_back(val);
		}
		evalFile.close();
		analyzeFile(unions[i].second.first, repetitions, nEvaluations);
		analyzeValuesFrontFile(unions[i].second.first, repetitions, nEvaluations);
	}
}

void calculateContributions(){
	for (unsigned int i = 0; i < contributions.size(); i++){
		stringstream cmd;
		cmd.str("");
		cmd << exeDir << "/calculateContributions " << contributions[i].second << " ";
		for (unsigned int j = 0; j < contributions[i].first.size(); j++){
			cmd << contributions[i].first[j].first << " " << contributions[i].first[j].second << " ";
		}
		cout << "Ejecuta: " << cmd.str() << endl;
		system(cmd.str().c_str());
	}
}

void calculateCoverages(){
	for (unsigned int i = 0; i < coverages.size(); i++){
		stringstream cmd;
		cmd.str("");
		cmd << exeDir << "/calculateCoverages " << coverages[i].first.first << " " << coverages[i].first.second << " " << coverages[i].second.first << " " << coverages[i].second.second;
		cout << "Ejecuta: " << cmd.str() << endl;
		system(cmd.str().c_str());
	}
}

void calculateValuesCoverages(){
	char *tmp;
	for (unsigned int i = 0; i < coverages.size(); i++){
		tmp = new char[coverages[i].second.second.size() + 1];
		strcpy(tmp, coverages[i].second.second.c_str());
		sufixName = string("cover_") + basename(tmp);
		delete tmp;
		analyzeExperimentResults(coverages[i].first.first, coverages[i].first.second, valuesCalculator, false);
		tmp = new char[coverages[i].first.second .size() + 1];
		strcpy(tmp, coverages[i].first.second.c_str());
		sufixName = string("cover_") + basename(tmp);
		delete tmp;
		analyzeExperimentResults(coverages[i].second.first, coverages[i].second.second, valuesCalculator, false);
	}
}

void createAttainmentSurface(const string &baseFile, const vector<int> &evaluations, const int repetitions, const int type){
	stringstream cmd;
	vector<int> indexSurface;
	indexSurface.push_back(1);
	indexSurface.push_back(repetitions/2);
	indexSurface.push_back(repetitions);
	stringstream directions;
	for (int j = 0; j < ind->getNumberOfObj(); j++){
		if (ind->getInternalOptDirection(j) == MINIMIZE){
			directions << "-1 ";
		} else {
			directions << "1 ";
		}
	}
	for (unsigned int i = 0; i < evaluations.size(); i++){
		for (unsigned int j = 0; j < indexSurface.size(); j++){
			cmd.str("");
			cmd << "./plot-attainments2/source/plot-att " << baseFile << "_" << evaluations[i] << ".obj" << " -r 30 -k " << ind->getNumberOfObj();
			cmd << " -mm " << directions.str() << "-a " << indexSurface[j] << " > " << baseFile << "_" << evaluations[i];
			cmd << ".surface_" << indexSurface[j]; 
			cout << "Comando: " << cmd.str() << endl;
			system(cmd.str().c_str());
		}
	}
}

void calculateAttainmentSurfaces(){
	for (unsigned int i = 0; i < experimentFiles.size(); i++){
		analyzeExperimentResults(experimentFiles[i], "*", createAttainmentSurface, false);
	}
}

int main(int argc, char *argv[]){
	if (argc != 2){
		cout << "Uso " << argv[0] << " metricFile" << endl;
		exit(-1);
	}
	exeDir = dirname(argv[0]);
	baseDirectory = exeDir + "/../../src/skeleton/main";
	metricFilein = fopen(argv[1], "r");
	metricFileparse();
	if (errorParsingMetric){
		cout << "File format not recognized" << endl;
		exit(-1);
	}
	if (generalFrontName.size() == 0){
		cout << "Error: GeneralFrontName non-specified" << endl;
		exit(-1);
	}
	calculateMerges();
	calculateContributions();
	calculateValuesContributions();
	calculateCoverages();
	calculateValuesCoverages();
	calculateMetricExperiments();
	//calculateAttainmentSurfaces();	//!!multi-objetivo
	calculateMetricUnions();
	deleteFrontFiles();
	fclose(metricFilein);
}
