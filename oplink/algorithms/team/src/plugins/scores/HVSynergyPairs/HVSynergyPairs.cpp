/***********************************************************************************
 * AUTORES
 *   Carlos Segura González
 ***********************************************************************************/


#include "HVSynergyPairs.h"
#include <math.h>

long double HVSynergyPairs::score(MOFront *front){
	return front->getHV(refPoint);
}

long double HVSynergyPairs::getScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	long double imp = 0;
	int n = 0;
	//Parte de selección inicial: todos deben ejecutar una vez al menos
	if ((scores[alg][conf].size() == 0) && (configToExecuting[alg][conf] == 0)){//No ha terminado ejecucion ni esta ejecutando
		return 10000000000.0;
	}
	if (scores[alg][conf].size() == 0){//No ha terminado de ejecutar, pero esta ejecutando
		return DBL_MIN;
	}

	//Si es la primera heurística del grupo en ser asignada, o toca debido al rndValue
	if (((assignedHeuristics.size() % slaveIslands) == 0) || (rndValue < p1Value)){//media ponderada de las mejoras
		int begin = (((int)(scores[alg][conf].size()) - consideredValues) >= 0)? ((int)(scores[alg][conf].size()) - consideredValues) : 0;
		int j = 0;
		for (int i = begin; i < scores[alg][conf].size(); i++, j++){
			imp += (scores[alg][conf][i])*(j+1);
			n += (j+1);
		}
		return imp / n;
	} else if (rndValue < p2Value){//Puntuacion basada en pares
		//Las heuristicas asignadas se encuentran al final (las primeras slaveIslands son de la asignación anterior)
		int heuristicsToExplore = (int)(assignedHeuristics.size()) - slaveIslands;
		long double maxValue = LDBL_MIN;
		list< pair <int, int> >::iterator currentH = --assignedHeuristics.end();
		for (int i = 0; i < heuristicsToExplore; i++, currentH--){
			int assignedAlgorithm = currentH->first;
			int assignedConfiguration = currentH->second;
			//Calculamos la media con cada uno de los asignado y nos quedamos con el maximo
			int begin = (((int)(pairScores[alg][conf][assignedAlgorithm][assignedConfiguration].size()) - consideredValues) >= 0)? ((int)(pairScores[alg][conf][assignedAlgorithm][assignedConfiguration].size()) - consideredValues) : 0;
			int k = 0;
			for (int j = begin; j < pairScores[alg][conf][assignedAlgorithm][assignedConfiguration].size(); j++, k++){
				imp += (pairScores[alg][conf][assignedAlgorithm][assignedConfiguration][j])*(k+1);
				n += (k+1);
			}
			long double value = (n != 0) ? (imp / n) : LDBL_MIN;
			if (value > maxValue)
				maxValue = value;
		}
		return maxValue;
	} else { //Aleatorio
		return 1;
	}
}

long double HVSynergyPairs::estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	long double sc = getScore(breakFromScore, alg, conf, islandId, rndValue);
	return sc;
}

void HVSynergyPairs::updateScore(const long double breakFrom, const long double arriveTo, int algId, int confId, int islandId){
	executions[algId][confId]++;
	// Los que parten de 0 no los registramos
	if (breakFrom == 0)
		return;
	long double imp;
	imp = arriveTo - breakFrom;
	scores[algId][confId].push_back(imp);
	int end = (assignedHeuristics.size() < slaveIslands) ? assignedHeuristics.size() : slaveIslands;
	list< pair <int, int> >::iterator elemento = assignedHeuristics.begin();
	bool itself = false;
	for (int i = 0; i < end; i++, elemento++){
		if ((algId == elemento->first) && (confId == elemento->second) && (itself == false)){
			itself = true;
			continue;
		}
		pairScores[algId][confId][elemento->first][elemento->second].push_back(imp);
	}
}

void HVSynergyPairs::printHistory(){
}

void HVSynergyPairs::begins(const int algId, const int confId, const int islandId){
	if (configToExecuting[algId].count(confId) == 0)
		configToExecuting[algId][confId] = 1;
	else
		configToExecuting[algId][confId]++;
	assignedHeuristics.push_back(pair<int, int> (algId, confId));
	//Borramos las heuristicas viejas
	if (assignedHeuristics.size() == 2 * slaveIslands){
		for (int i = 0; i < slaveIslands; i++){
			assignedHeuristics.pop_front();
		}
	}
}

//Detecta si "los grupos" pierden la sincronia, ya que en ese caso el algoritmo no se
//estara comportando como se espera y se emite warning. Se considera que se piderde
//la sincronía si una isla ha acabado n veces, mientras que otra aún no ha acabado
//n-1 veces.
void HVSynergyPairs::asynchronyDetection(int islandId){
}

void HVSynergyPairs::ends(const int algId, const int confId, const int islandId){
	configToExecuting[algId][confId]--;
	asynchronyDetection(islandId); 	
}

//Parámetros: num_islas_esclavas p1(porcentaje de uso puntuacion independiente) p2(porcentaje de uso puntuacion de pares) n(maximos valores media)  punto_referencia
bool HVSynergyPairs::init(const vector<string> &params) {
	if (params.size() < 6)
		return false;
	slaveIslands = atoi(params[0].c_str());
	if (slaveIslands < 1){
		cout << "Error: slaveIslands must be greater than 1" << endl;
		return false;
	}
	p1Value = atof(params[1].c_str()) / 100;
	if (p1Value < 0){
		cout << "Error: p1 must be greater or equal than 0" << endl;
		return false;
	}
	p2Value = atof(params[2].c_str()) / 100;
	if (p2Value < 0){
		cout << "Error: p2 must be greater or equal than 0" << endl;
		return false;
	}
	p2Value += p1Value;
	if (p2Value > 1){
		cout << "Error: p2 + p1 must be lower than 100" << endl;
		return false;
	}
	consideredValues = atoi(params[3].c_str());
	for (int i = 4; i < params.size(); i++){
		refPoint.push_back(atof(params[i].c_str()));
	}

	return true;
}
