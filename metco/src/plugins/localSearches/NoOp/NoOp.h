/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    May 2010
 *
 * DESCRIPTION
 *    LocalSearch module which makes nothing
 ************************************************************************************************/


#ifndef __NOOP_H__
#define __NOOP_H__

#include <vector>

#include "Individual.h"
#include "LocalSearch.h"

using namespace std;

class NoOp : public LocalSearch {
	public:
		void localSearch(vector <Individual*> *poblacion, MOFront *currentSolution);
};
#endif
