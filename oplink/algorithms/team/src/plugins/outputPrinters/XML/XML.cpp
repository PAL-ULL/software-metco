#include "MOFrontVector.h"
#include "XML.h"
#include <time.h>
#include <iomanip>
#include <sstream>
#include <ctype.h>

string XML::getStrWithMinLength(int num, int minLength){
	stringstream str, str2;
	str << num;
	int dif = minLength - str.str().size();
	for (int i = 0; i < dif; i++){
		str2 << "0";
	}
	str2 << num;
	return str2.str();
}

void XML::printSolution(EA *ga, bool end){
	if (end){
		return;
	}
	double time, evaluations;
	if (ga->getCritStop() == ga->getTypeStoppingCriterion("TIME")) {
		time = ga->getNextPrint();
		evaluations = ga->getPerformedEvaluations();
	} else if (ga->getCritStop() == ga->getTypeStoppingCriterion("EVALUATIONS")){
		time = ga->getElapsedTime();
		evaluations = ga->getNextPrint();
	}
	MOFront *solution;
	if (ga->isGeneratingArchive()){
		solution = ga->getLocalSolution();
	} else {
		solution = new MOFrontVector(ga->getSampleInd(), false, false);
		ga->getSolution(solution);
	}

	//Calculamos minimos y maximos de cada objetivo
	vector <double> maxObjs(solution->getInd(0)->getNumberOfObj(), -DBL_MAX);
	vector <double> minObjs(solution->getInd(0)->getNumberOfObj(), DBL_MAX);
	for (int i = 0; i < solution->getInd(0)->getNumberOfObj(); i++){
		for (int j = 0; j < solution->getFrontSize(); j++){
			if (solution->getInd(j)->getObj(i) > maxObjs[i]){
				maxObjs[i] = solution->getInd(j)->getObj(i);
			}
			if (solution->getInd(j)->getObj(i) < minObjs[i]){
				minObjs[i] = solution->getInd(j)->getObj(i);
			}
		}
	}


	outputFile << "\t\t<trace nEval=\"" << evaluations << "\" nSolutions=\"" << solution->getFrontSize() << "\" ";
	for (int i = 0; i < solution->getInd(0)->getNumberOfObj(); i++){
			string objName = solution->getInd(0)->getObjName(i);
			if (objName.size() > 0){
				objName[0] = toupper(objName[0]);
			}
		outputFile << "min" << objName << "=\"" << minObjs[i] << "\" max" << objName << "=\"" << maxObjs[i] << "\" ";
	}
	
	int hours = time / 3600;
	time -= hours * 3600;
	int minutes = time / 60;
	double seconds = time - minutes * 60;
	string hoursStr = getStrWithMinLength(hours, 2);
	string minutesStr = getStrWithMinLength(minutes, 2);

	if (seconds < 10){
		outputFile << "executionTime = \"" << hoursStr << ":" << minutesStr << ":0" << seconds << "\">" << endl;
	} else {
		outputFile << "executionTime = \"" << hoursStr << ":" << minutesStr << ":" << seconds << "\">" << endl;
	}

	outputFile <<"\t\t\t<solutionSet>" << endl;
	for (int i = 0; i < solution->getFrontSize(); i++){
		outputFile <<"\t\t\t\t<solution";
		for (int j = 0; j < solution->getInd(i)->getNumberOfObj(); j++){
			outputFile << " " << solution->getInd(i)->getObjName(j) << "=\"" << solution->getInd(i)->getObj(j) << "\"";
		}
		outputFile << ">" << endl;
		//Nota no estamos dejando espacios entre las variables porque para el RND querian la cadena toda seguida y como
		//eran 0 y 1 se distinguen.
		//Habria que cambiarlo para problemas en los que los valores de las variables sean double. Estaria bien
		//que de alguna forma pudieramos "preguntar" que tipo de salida tienen...
		for (int j = 0; j < solution->getInd(i)->getNumberOfVar(); j++){
			outputFile << solution->getInd(i)->getVar(j);
		}
		outputFile << endl;
		outputFile << "\t\t\t\t</solution>" << endl;
	}
	outputFile <<"\t\t\t</solutionSet>" << endl;
	outputFile << "\t\t</trace>" << endl;

	if (!ga->isGeneratingArchive()){
		delete solution;
	}
}

void XML::printInit(EA *ga){
	outputFile << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>" << endl;
	struct timeval actualTime; 
	gettimeofday(&actualTime, NULL);
	struct tm *timeBroken = localtime(&actualTime.tv_sec);
	outputFile << "<experiment timestamp=\"" << timeBroken->tm_year + 1900 << "-" << getStrWithMinLength(timeBroken->tm_mon + 1, 2) << "-" << getStrWithMinLength(timeBroken->tm_mday, 2) << "T" << getStrWithMinLength(timeBroken->tm_hour, 2) << ":" << getStrWithMinLength(timeBroken->tm_min, 2) << ":" << getStrWithMinLength(timeBroken->tm_sec, 2) << "\" ";
	ga->getSampleInd()->printInfo(outputFile);
	if (ga->getCritStop() == ga->getTypeStoppingCriterion("TIME")) {
		outputFile << " maxTime=\"" << ga->getCritStopValue() << "\" ";
	} else if (ga->getCritStop() == ga->getTypeStoppingCriterion("EVALUATIONS")){
		outputFile << " maxFitnessEvaluations=\"" << ga->getCritStopValue() << "\" ";
	}
	outputFile << " checkpointPeriod=\"" << ga->getPrintPeriod() << "\">" << endl;
	
	outputFile << "\t<algorithm name=\"" << ga->getName() << "\">" << endl;

	outputFile << "\t\t<parameters>" << endl;
	for (int i = 0; i < ga->getNumberOfParameters(); i++)
		outputFile << "\t\t\t<parameter id=\"" << ga->getParameterName(i) << "\" value=\"" << ga->getParameterValue(i) << "\" />" << endl;
	outputFile << "\t\t</parameters>" << endl;
	
	outputFile << "\t\t<operators>" << endl;
	
	outputFile << "\t\t\t<operator id=\"" << ga->getSampleInd()->getMutationName() << "\" type =\"mutation\">" << endl;
	outputFile << "\t\t\t\t<parameters>" << endl;  
	for (int i = 0; i < ga->getSampleInd()->getMutationNumberOfParameters(); i++){
		outputFile << "\t\t\t\t\t<parameter id=\"" << ga->getSampleInd()->getMutationParameterName(i) << "\" value=\"" << ga->getSampleInd()->getMutationParameterValue(i) << "\" />" << endl;
	}
	outputFile << "\t\t\t\t</parameters>" << endl;  
	outputFile << "\t\t\t</operator>" << endl;

	outputFile << "\t\t\t<operator id=\"" << ga->getSampleInd()->getCrossoverName() << "\" type =\"crossover\">" << endl;
	outputFile << "\t\t\t\t<parameters>" << endl;  
	for (int i = 0; i < ga->getSampleInd()->getCrossoverNumberOfParameters(); i++){
		outputFile << "\t\t\t\t\t<parameter id=\"" << ga->getSampleInd()->getCrossoverParameterName(i) << "\" value=\"" << ga->getSampleInd()->getCrossoverParameterValue(i) << "\" />" << endl;
	}
	outputFile << "\t\t\t\t</parameters>" << endl;  
	outputFile << "\t\t\t</operator>" << endl;
	
	outputFile << "\t\t</operators>" << endl;
	
	outputFile << "\t</algorithm>" << endl; 

	outputFile << "\t<traces>" << endl;
}

bool XML::init(const vector<string> &params){
	if (params.size() != 1){
		cerr << "Incorrect parameters to module XML: outputFile" << endl;
		return false;
	}
	outputFile.open(params[0].c_str(), ios::in|ios::out|ios::trunc);
	outputFile << setprecision(8);
	if (!outputFile.is_open()){
		cerr << "No pudo abrir fichero: " << params[0] << endl;
	}
	return true;
}

void XML::finish(){
	outputFile << "\t</traces>" << endl;
	outputFile << "</experiment>" << endl;
	outputFile.close();
}
