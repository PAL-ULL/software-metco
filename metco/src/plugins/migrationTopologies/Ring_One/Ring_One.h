/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    April 2010
 *
 * DESCRIPTION
 *    Ring (Unidirectional) topology.
 ************************************************************************************************/


#ifndef RINGBIDIRECTIONAL_ONE_H
#define RINGBIDIRECTIONAL_ONE_H

#include "Individual.h"
#include "MigrationTopology.h"

#include <vector>

using namespace std;

class Ring_One : public MigrationTopology {
	public:
		void sendTo(const int myId, const int slaveIslands, vector<int> &destination);
};

#endif
