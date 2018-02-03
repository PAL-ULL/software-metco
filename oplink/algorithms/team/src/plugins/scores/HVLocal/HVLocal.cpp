#include "HVLocal.h"
#include <math.h>

#define BIG_SCORE 10000000

//Puntuacion de un frente: el hipervolumen
long double HVLocal::score(MOFront *front){
	return front->getHV(refPoint);
}

long double HVLocal::getScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	if (rndValue < probRandom){//caso random
		return BIG_SCORE;
	}
	if (penalizations[alg][conf] > 0){//si esta penalizada
		return DBL_EPSILON;
	}
	if ((configToExecuting[alg][conf] > 0) && (scores[alg].count(conf) == 0)){//si esta ejecutando pero no hay puntuaciones previas aun
		return DBL_EPSILON;
	}
	if (breakFromScore == 0){//inicio
		return BIG_SCORE;
	}
	if (scores[alg].count(conf) == 0){//sin datos previos y no esta ejecutando
		return BIG_SCORE;
	}
	//Si solo hay 1 ejecucion, estimamos que va a mejorar de la misma forma
	if (scores[alg][conf].size() == 1){
		return scores[alg][conf][0].second - scores[alg][conf][0].first;
	}
	
	//Regresión exponencial
	long double sumxy = 0, sumx = 0, sumy = 0, sumx2 = 0;
	int n = scores[alg][conf].size();
	int numElem = (n > expRegSize)?expRegSize:n;	
	for (int i = ((n-expRegSize)>= 0)?(n-expRegSize):0; i < n; i++){
		sumxy = scores[alg][conf][i].first * log(scores[alg][conf][i].second - scores[alg][conf][i].first);
		sumx = scores[alg][conf][i].first;
		sumy = log(scores[alg][conf][i].second - scores[alg][conf][i].first);
		sumx2 = scores[alg][conf][i].first * scores[alg][conf][i].first;
	}
	long double m = (numElem*sumxy - sumx*sumy)/(numElem*(sumx2) - sumx * sumx);
	long double b = (sumy - m*sumx) / numElem;
	long double estimated = (exp(b)*pow(exp(m), breakFromScore)); /// (penalizations[alg][conf] + 1);
	return estimated;
}

long double HVLocal::estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	vector<long double> values;
	for (int i = 0; i < getNumberOfAlgorithms(); i++){
		for (int j = 0; j < getNumberOfConfigurations(i); j++){
			long double value = getScore(breakFromScore, i, j, islandId, rndValue);
			values.push_back(value);
		}
	}
	sort(values.begin(), values.end());
	long double sc = getScore(breakFromScore, alg, conf, islandId, rndValue);
	long double estimated = (sc >= values[values.size() - consideredConfigurations])?((islandToConfig[islandId] == pair<int, int>(alg, conf))?sc:sc):DBL_EPSILON;
	if (estimated < 0){
		estimated = DBL_EPSILON * 1000;
	}
	return estimated;
}

void HVLocal::updateScore(const long double breakFrom, const long double arriveTo, int alg, int conf, int islandId){
	// Los que parten de 0 no los registramos
	if (breakFrom == 0)
		return;
	//Buscamos posición desde el final (buscamos uno que sea menor)
	if (penalizations[alg].count(conf) == 0)//inicializacion de info. de penalizaciones
		penalizations[alg][conf] = 0;
	if (arriveTo <= breakFrom){//no avanzo, penalizamos
		if (penalizations[alg][conf] == 0){
			penalized++;
			if (penalized > (getNumberOfConfigurations() / 2)){//+ de la mitad penalizados
				penalized = 0;
				for (int i = 0; i < getNumberOfAlgorithms(); i++){
					for (int j = 0; j < getNumberOfConfigurations(i); j++){
						penalizations[i][j] = 0;
					}
				}
			} else {
				penalizations[alg][conf]++;
			}
			return;
		}
	} 
	for (int pos = scores[alg][conf].size() - 1; pos >= 0; pos--){
		if (scores[alg][conf][pos].first <= breakFrom){
			scores[alg][conf].insert(scores[alg][conf].begin() + pos + 1, 1, pair<long double, long double>(breakFrom, arriveTo));
			return;
		}
	}
	scores[alg][conf].insert(scores[alg][conf].begin(), 1, pair<long double, long double>(breakFrom, arriveTo));
}

void HVLocal::printHistory(){
	for (int i = 0; i < scores.size(); i++){
		for (int j = 0; j < scores[i].size(); j++){
			cout << "Config(" << i << ", " << j << ")" << endl;
			cout << "    Penalizations: " << penalizations[i][j] << endl;
			for (int k = 0; k < scores[i][j].size(); k++){
				cout << scores[i][j][k].first << " --> " << scores[i][j][k].second << endl;
			}
			cout << endl << endl;
		}
	}
}

void HVLocal::begins(const int algId, const int confId, const int islandId){
	islandToConfig[islandId] = pair<int, int>(algId, confId);
	if (configToExecuting[algId].count(confId) == 0)
		configToExecuting[algId][confId] = 1;
	else
		configToExecuting[algId][confId]++;
}

void HVLocal::ends(const int algId, const int confId, const int islandId){
	configToExecuting[algId][confId]--;
}

//Parámetros: 
//- número de configuraciones a considerar (se consideran sólo las n con mejor puntuacion)
//- número de últimas ejecuciones a considerar para la regresion exponencial
//- prob. de usar random [0 - 1]
//- punto de referencia para el calculo del hipervolumen
bool HVLocal::init(const vector<string> &params) {
	if (params.size() < 4)
		return false;
	consideredConfigurations = atoi(params[0].c_str());
	expRegSize = atoi(params[1].c_str());
	probRandom = atof(params[2].c_str());
	for (int i = 3; i < params.size(); i++){
		refPoint.push_back(atof(params[i].c_str()));
	}
	return true;
}
