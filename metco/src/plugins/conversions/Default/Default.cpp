/*****************************************************************************************
 * AUTHORS
 *  - Gara Miranda Valladares
 *  - Carlos Segura González
 *
 * DATE
 *   February 2008
 *****************************************************************************************/

#include "Default.h"

bool Default::init(const vector<string> &v){
	if (v.size() != 0){
		return false;
	}
	return true;
}

Individual* Default::convert(const Individual *ind, const int confFrom, const int confTo){
	Individual *newInd = exampleIndividuals[confTo]->internalClone();
	newInd->internalCloneData(ind);
	newInd->cloned();
	return newInd;
}
