/*****************************************************************************************
 * AUTHORS
 *  - Gara Miranda Valladares
 *  - Carlos Segura González
 *
 * DATE
 *   February 2008
 ****************************************************************************************/

#include "Oplink.h"

#define EE_ALG 0
#define NSGA2_ALG 1
#define SPEA_ALG 2
#define SPEA2_ALG 3
#define MOPSO_ALG 4
#define IBEA_ALG 5
#define IBEA_ADAPT_ALG 6
#define MOCELL 7
#define DE 8

bool Oplink::init(const vector<string> &v){
	if (v.size() != 1){
		cout << "El modulo de conversion Oplink espera un parametro" << endl << flush;
		return false;
	}
	EEvarianza = atof(v[0].c_str());
	vector < string > algs;
	algs.push_back("EE");
	algs.push_back("NSGA2");
	algs.push_back("SPEA");
	algs.push_back("SPEA2");
	algs.push_back("MOPSO");
	algs.push_back("IBEA");
	algs.push_back("IBEA_Adapt");
	algs.push_back("MOCell");
	algs.push_back("DE");
	return makeTableTranslation(algs);
}

Individual* Oplink::convert(const Individual *ind, const int confFrom, const int confTo){
	int realFrom = getTranslation(confFrom);
	int realTo = getTranslation(confTo);
	Individual *nuevo = ind->internalClone();
	if (realFrom == realTo)
		return nuevo;
	if (realFrom == EE_ALG){
		nuevo->setMigrationDataSize(0);
	} else if (realTo == EE_ALG){
		nuevo->setMigrationDataSize(1);
		nuevo->setMigrationData(0, EEvarianza);
	}
	return nuevo;
}
