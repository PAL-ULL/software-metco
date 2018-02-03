/***********************************************************************************
 * AUTORES
 *   Gara Miranda Valladares 
 *   Carlos Segura González
 ***********************************************************************************/

#include "HVWeightAvg.h"
#include <math.h>

long double HVWeightAvg::score(MOFront *front){
	long double hv = front->getHV(refPoint);
	return hv;
}

long double HVWeightAvg::getScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	long double imp = 0;
	int n = 0;
	if ((scores[alg][conf].size() == 0) && (configToExecuting[alg][conf] == 0)){//No ha terminado ejecucion ni esta ejecutando
		return 1000000000;
	}
	if (scores[alg][conf].size() == 0){//No ha terminado de ejecutar, pero esta ejecutando
		return DBL_MIN;
	}
	if (rndValue < pRandom){
		return 1;
	}
	int begin = (((int)(scores[alg][conf].size()) - consideredValues) >= 0)? ((int)(scores[alg][conf].size()) - consideredValues) : 0;
	int j = 0;
	for (int i = begin; i < scores[alg][conf].size(); i++, j++){
		imp += (scores[alg][conf][i].second - scores[alg][conf][i].first)*(j+1);
		n += (j+1);
	}
	if (imp == 0){
		return DBL_MIN;
	}
	return imp / n;
}

long double HVWeightAvg::estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	long double sc = getScore(breakFromScore, alg, conf, islandId, rndValue);
	return sc;
}

void HVWeightAvg::updateScore(const long double breakFrom, const long double arriveTo, int alg, int conf, int islandId){
	// Los que parten de 0 no los registramos
	if (islandExecutions.count(islandId) == 0){
		islandExecutions[islandId] = 1;
		return;
	}
	islandExecutions[islandId]++;
	if (breakFrom == 0)
		return;
	if (arriveTo < breakFrom){
		scores[alg][conf].push_back(pair<long double, long double>(breakFrom, breakFrom));
	} else {
		scores[alg][conf].push_back(pair<long double, long double>(breakFrom, arriveTo));
	}
}

void HVWeightAvg::begins(const int algId, const int confId, const int islandId){
	islandToConfig[islandId] = pair<int, int>(algId, confId);
	if (configToExecuting[algId].count(confId) == 0)
		configToExecuting[algId][confId] = 1;
	else
		configToExecuting[algId][confId]++;
}

void HVWeightAvg::ends(const int algId, const int confId, const int islandId){
	configToExecuting[algId][confId]--;
}

//Parámetros: consideredValues pRanom punto de referencia para el calculo del hipervolumen 
bool HVWeightAvg::init(const vector<string> &params) {
	if (params.size() < 4)
		return false;
	consideredValues = atoi(params[0].c_str());
	pRandom = atof(params[1].c_str());
	for (int i = 2; i < params.size(); i++){
		refPoint.push_back(atof(params[i].c_str()));
	}
	return true;
}
