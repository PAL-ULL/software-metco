/***********************************************************************************
 * AUTORES
 *   Gara Miranda Valladares 
 *   Carlos Segura González
 ***********************************************************************************/


#include "Uniform.h"
#include <math.h>

long double Uniform::score(MOFront *front){
	return 1;
}

long double Uniform::estimateScore(const long double breakFromScore, const int alg, const int conf, const int islandId, const double rndValue){
	return 1;
}

bool Uniform::init(const vector<string> &params) {
	if (params.size() != 0)
		return false;
	return true;
}
