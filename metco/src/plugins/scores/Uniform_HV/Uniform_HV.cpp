/***********************************************************************************
 * AUTORES
 *   Gara Miranda Valladares 
 *   Carlos Segura González
 ***********************************************************************************/


#include "Uniform_HV.h"
#include <math.h>

long double Uniform_HV::score(MOFront *front){
	return front->getHV(refPoint);
}

long double Uniform_HV::estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	return 1;
}

bool Uniform_HV::init(const vector<string> &params) {
	for (int i = 0; i < params.size(); i++){
		refPoint.push_back(atof(params[i].c_str()));
	}
	return true;
}
