/***********************************************************************************
 * AUTORES
 *   Gara Miranda Valladares 
 *   Carlos Segura González
 *
 * STATUS
 * 	 Partially implemented
 ***********************************************************************************/


#include "MonoWeightAvgImp.h"
#include <math.h>

long double MonoWeightAvgImp::score(MOFront *front){
	if (front->getFrontSize() == 0){
		return 0;
	}
	//Puntuacion del best
	long double value = front->getInd(0)->getObj(0);
	optDirection = front->getInd(0)->getInternalOptDirection(0);

	for (int i = 1; i < front->getFrontSize(); i++) {
		if ((front->getInd(i)->getObj(0) > value) && (optDirection == MAXIMIZE)) {
			value = front->getInd(i)->getObj(0);
		}
    else if ((front->getInd(i)->getObj(0) < value) && (optDirection == MINIMIZE)) {
			value = front->getInd(i)->getObj(0);
    }
	}

	return value;
}

long double MonoWeightAvgImp::getScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	long double imp = 0;
	int n = 0;
	if ((scores[alg][conf].size() == 0) && (configToExecuting[alg][conf] == 0)){//No ha terminado ejecucion ni esta ejecutando
		//return 1000000000; #Modificado para large scale optimization
		return DBL_MAX;
	}
	if (scores[alg][conf].size() == 0){//No ha terminado de ejecutar, pero esta ejecutando
		return DBL_MIN;
	}
	
	if (rndValue < pRandom){
		return 1;
	}

	int begin = (((int)(scores[alg][conf].size()) - consideredValues) >= 0)? ((int)(scores[alg][conf].size()) - consideredValues) : 0;
	int j = 0;
	//cout << "Comienza a calcular en: " << begin << endl;
	for (int i = begin; i < scores[alg][conf].size(); i++, j++){
		imp += (scores[alg][conf][i])*(j+1);
		n += (j+1);
	}
	if (imp == 0){
		return DBL_MIN;
	}
	//cout << "Pronostica para " << alg << ": " << imp / n << endl;
	return imp / n;
}

long double MonoWeightAvgImp::estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	long double sc = getScore(breakFromScore, alg, conf, islandId, rndValue);
	return sc;
}

void MonoWeightAvgImp::updateScore(const long double breakFrom, const long double arriveTo, int alg, int conf, int islandId){
	executions[alg][conf]++;
	// Los que parten de 0 no los registramos
	if (breakFrom == 0)
		return;
	long double imp;
	if ((optDirection != MAXIMIZE) && (optDirection != MINIMIZE)) {
		cerr << "Error Interno: optDirection no fijado y se intenta usar" << endl;
		exit(-1);
	}
	if (optDirection == MINIMIZE){
		imp = breakFrom - arriveTo;
	} else {
		imp = arriveTo - breakFrom;
	}
	//cout << "Algoritmo: " << alg << ": " << imp << endl;
	//cout << "Desde: " << breakFrom << " a " << arriveTo << endl;
	if (imp >= 0){
		scores[alg][conf].push_back(imp);
	} else {//Para los no elitistas
		scores[alg][conf].push_back(0);
	}
}

void MonoWeightAvgImp::printHistory(){
	for (int i = 0; i < scores.size(); i++){
		for (int j = 0; j < scores[i].size(); j++){
			cout << "Config(" << i << ", " << j << ")" << endl;
			for (int k = 0; k < scores[i][j].size(); k++){
				cout << scores[i][j][k] << endl;
			}
			cout << endl << endl;
		}
	}
}

void MonoWeightAvgImp::begins(const int algId, const int confId, const int islandId){
	islandToConfig[islandId] = pair<int, int>(algId, confId);
	if (configToExecuting[algId].count(confId) == 0)
		configToExecuting[algId][confId] = 1;
	else
		configToExecuting[algId][confId]++;
}

void MonoWeightAvgImp::ends(const int algId, const int confId, const int islandId){
	configToExecuting[algId][confId]--;
	terminated++;
	if (terminated == restart){
		//cerr << "Limpia" << endl;
		scores.clear();
	}
}

//Parámetros: número de valores a considerar (se consideran las últimas n) fuerza_aleatoria 
bool MonoWeightAvgImp::init(const vector<string> &params) {
	terminated = 0;
	if ((params.size() != 2) && (params.size() != 3))
		return false;
	consideredValues = atoi(params[0].c_str());
	pRandom = atof(params[1].c_str());
	if (params.size() == 3){
		restart = atoi(params[2].c_str());//Restart after X executions
	} else {
		restart = 0;
	}
	return true;
}
