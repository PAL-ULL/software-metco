/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    April 2010
 *
 * DESCRIPTION
 *    All-to-all connected topology. It sends the individuals to every island in each migration 
 ************************************************************************************************/


#ifndef ALL_ALL_H
#define ALL_ALL_H

#include "Individual.h"
#include "MigrationTopology.h"

#include <vector>

using namespace std;

class All_All : public MigrationTopology {
	public:
		void sendTo(const int myId, const int slaveIslands, vector<int> &destination);
};

#endif
