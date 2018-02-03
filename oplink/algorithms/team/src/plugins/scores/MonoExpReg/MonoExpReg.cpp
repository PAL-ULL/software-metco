#include "MonoExpReg.h"
#include <math.h>

long double MonoExpReg::score(MOFront *front){
	if (front->getFrontSize() == 0){
		return 0;
	}
	optDirection = front->getInd(0)->getInternalOptDirection(0);
	//Puntuacion del best
	long double value = front->getInd(0)->getObj(0);
/*	int optD = front->getInd(0)->getInternalOptDirection(0);
	for (int i = 1; i < front->getFrontSize(); i++){
		if (optD == MINIMIZE){
			if (front->getInd(i)->getObj(0) < value){
				value = front->getInd(i)->getObj(0);
			}
		} else if (optD == MAXIMIZE){
			if (front->getInd(i)->getObj(0) < value){
				value = front->getInd(i)->getObj(0);
			}
		}
	}*/	
	return value;
}

long double MonoExpReg::getScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	if (penalizations[alg][conf] > 0){
		return LDBL_EPSILON;
	}
	if ((configToExecuting[alg][conf] > 0) && (scores[alg].count(conf) == 0)){
		return LDBL_EPSILON;
	}
	//Esto es para que no ejecute mas de 4 de la misma configuracion
	if (configToExecuting[alg][conf] >= 4){
		return LDBL_EPSILON;
	}
	
	if (rndValue < 0.1){//el 10% de las veces reparto uniforme (entre los no penalizados)
		return 100000000;
	}
	if (breakFromScore == 0){//inicio
		return 100000000;
	}
	//si no hay ejecuciones de ese aún le damos valor 100000
	if (scores[alg].count(conf) == 0){
		return 100000000;
	}
	//Si solo hay 1 ejecucion, le damos lo que mejoro
	if (scores[alg][conf].size() == 1){
		return (scores[alg][conf][0].second - scores[alg][conf][0].first)/(penalizations[alg][conf] + 1);
	}
	//Hacemos una estimacion de cuánto va a mejorar en función de las 2 últimas ejecuciones si han partido del mismo valor 
	//double lastImp = scores[alg][conf][scores[alg][conf].size()-1].second - scores[alg][conf][scores[alg][conf].size()-1].first;
	//double prevImp = scores[alg][conf][scores[alg][conf].size()-2].second - scores[alg][conf][scores[alg][conf].size()-2].first;
	//if (scores[alg][conf][scores[alg][conf].size()-1].first == scores[alg][conf][scores[alg][conf].size()-2].first){
	//	return scores[alg][conf][scores[alg][conf].size() - 1].second - scores[alg][conf][scores[alg][conf].size() - 1].first;
	//}

	//Interpolación lineal con los 2 últimos puntos
	///double m = (lastImp - prevImp)/(scores[alg][conf][scores[alg][conf].size()-1].first - scores[alg][conf][scores[alg][conf].size()-2].first);
	//double estimated = lastImp + m * (breakFromScore - scores[alg][conf][scores[alg][conf].size()-1].first);
	
	//Interpolación exponencial con los 2 últimos puntos
	/*double y1 = prevImp;
	double y2 = lastImp;
	double x1 = scores[alg][conf][scores[alg][conf].size()-2].first;
	double x2 = scores[alg][conf][scores[alg][conf].size()-1].first;
	double m = log(y2/y1)/(x2-x1);
	double k = y1*exp(-m*x1);
	cout << " m = " << m << endl << flush;
	double estimated = k * exp(m*breakFromScore);*/

	//Regresión exponencial
	long double sumxy = 0, sumx = 0, sumy = 0, sumx2 = 0;
	int n = scores[alg][conf].size();
	int numElem = (n > 3)?3:n;	
	for (int i = ((n-3)>= 0)?(n-3):0; i < n; i++){
		sumxy = scores[alg][conf][i].first * log(scores[alg][conf][i].second - scores[alg][conf][i].first);
		sumx = scores[alg][conf][i].first;
		sumy = log(scores[alg][conf][i].second - scores[alg][conf][i].first);
		sumx2 = scores[alg][conf][i].first * scores[alg][conf][i].first;
	}
	long double m = (numElem*sumxy - sumx*sumy)/(numElem*(sumx2) - sumx * sumx);
	long double b = (sumy - m*sumx) / numElem;
	long double estimated = (exp(b)*pow(exp(m), breakFromScore)) / (penalizations[alg][conf] + 1);

	return estimated;

}

long double MonoExpReg::estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	vector<long double> values;
	for (int i = 0; i < getNumberOfAlgorithms(); i++){
		for (int j = 0; j < getNumberOfConfigurations(i); j++){
			values.push_back(getScore(breakFromScore, i, j, islandId, rndValue));
		}
	}
	sort(values.begin(), values.end());
	long double sc = getScore(breakFromScore, alg, conf, islandId, rndValue);
	//if ((islandToConfig[islandId] == pair<int, int>(alg, conf)) && (sc >= (values[values.size() - consideredConfigurations]))){
	//	cout << "SIIII: " << alg << endl;
	//}
	//Da preferencia a que continúe ejecutando el mismo
	int nowConsidered = (values.size() <= consideredConfigurations)?values.size():consideredConfigurations;
	return (sc >= values[values.size() - nowConsidered])?((islandToConfig[islandId] == pair<int, int>(alg, conf))?100000000:sc):DBL_EPSILON;
}

void MonoExpReg::updateScore(const long double breakFrom, const long double arriveTo, int alg, int conf, int islandId){
	executions[alg][conf]++;
	// Los que parten de 0 no los registramos
	if (breakFrom == 0)
		return;
	//Buscamos posición desde el final (buscamos uno que sea menor)
	if (penalizations[alg].count(conf) == 0)
		penalizations[alg][conf] = 0;
	if (arriveTo <= breakFrom){
		penalizations[alg][conf]++;
		if (penalizations[alg][conf] == 4){
			penalized++;
		}
		if (penalized > (getNumberOfConfigurations() / 2)){//+ de la mitad penalizados
			penalized = 0;
			for (int i = 0; i < getNumberOfAlgorithms(); i++){
				for (int j = 0; j < getNumberOfConfigurations(i); j++){
					penalizations[i][j] = 0;
				}
			}
		}
		return;
	} 
	int pos;
	for (pos = scores[alg][conf].size() - 1; pos >= 0; pos--){
		if (scores[alg][conf][pos].first <= breakFrom){
			scores[alg][conf].insert(scores[alg][conf].begin() + pos + 1, 1, pair<long double, long double>(breakFrom, arriveTo));
			return;
		}
	}
	scores[alg][conf].insert(scores[alg][conf].begin(), 1, pair<long double, long double>(breakFrom, arriveTo));

}

void MonoExpReg::printHistory(){
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

void MonoExpReg::begins(const int algId, const int confId, const int islandId){
	islandToConfig[islandId] = pair<int, int>(algId, confId);
	if (configToExecuting[algId].count(confId) == 0)
		configToExecuting[algId][confId] = 1;
	else
		configToExecuting[algId][confId]++;
}

void MonoExpReg::ends(const int algId, const int confId, const int islandId){
	configToExecuting[algId][confId]--;
}

//Parámetros: número de configuraciones a considerar (se consideran sólo las n con mejor puntuacion), punto
//de referencia para el calculo del hipervolumen
bool MonoExpReg::init(const vector<string> &params) {
	if (params.size() != 1)
		return false;
	consideredConfigurations = atoi(params[0].c_str());
	return true;
}
