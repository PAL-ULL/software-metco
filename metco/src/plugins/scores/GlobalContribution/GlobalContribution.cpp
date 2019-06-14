//Este modulo de puntuacion calcula cuantos individuos se insertan al final de cada ejecucion
//La puntuacion es una media ponderada de las ultimas ejecuciones
//No es adecuado con estrategias de migracion agresivas ya que se ignora el efecto que produce la migracion
//Probado con casos como el RND en que se migra cada vez que la solucion mejora es un desastre, porque
//toda la puntuacion se le concede al algoritmo que primero termina.
//Nota: no es exactamente la forma de puntuar que usabamos al principio, al principio lo que se usaba era: ¿cuantos
//individuos de este algoritmo hay actualmente en el frente local? 
//Eso ahora mismo no esta implementado, habria que modificar la cabecera del estimate score para que se le pase el globalSolutin
//y hacer la cuenta, pero tiene el mismo problema que este con las estrategias de migracion agresivas

#include "GlobalContribution.h"

void GlobalContribution::updateScore(const int alg, const int conf, const MOFront *globalSolution, const int inserted){
	//La primera finalizacion no la consideramos porque el frente estaba vacio
	if (finished > 1){
		scores[alg][conf].push_back(inserted);
		cout << "( " << alg << ", " << conf << ") = " << inserted << endl;
	}
}

long double GlobalContribution::estimateScore(const int alg, const int conf, const double rndValue){
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
	int n = 0;
	long double imp = 0;
	for (int i = begin; i < scores[alg][conf].size(); i++, j++){
		imp += (scores[alg][conf][i])*(j+1);
		n += (j+1);
	}
	return imp / n;
}

long double GlobalContribution::score(const MOFront *front){
	cout << "Error: Score no definido. No deberia ser llamado" << endl << flush;
	exit(-1);
	return 0;
}

//Parametros: consideredValues pRandom
bool GlobalContribution::init(const vector<string> &params) {
	if (params.size() != 2)
		return false;
	consideredValues = atoi(params[0].c_str());
	pRandom = atof(params[1].c_str());
	return true;
}

void GlobalContribution::begins(const int algId, const int confId, const int islandId) {
	if (configToExecuting[algId].count(confId) == 0)
		configToExecuting[algId][confId] = 1;
	else
		configToExecuting[algId][confId]++;
}

void GlobalContribution::ends(const int algId, const int confId, const int islandId){
	configToExecuting[algId][confId]--;
	finished++;
}
