/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    April 2010
 *
 * DESCRIPTION
 *    All-to-all connected topology. It sends the individuals to one island (randomly selected)
 *    in each migration
 ************************************************************************************************/


#ifndef ALL_ONE_H
#define ALL_ONE_H

#include "Individual.h"
#include "MigrationTopology.h"

#include <vector>

using namespace std;

class All_One : public MigrationTopology {
	public:
		void sendTo(const int myId, const int slaveIslands, vector<int> &destination);
};

#endif
