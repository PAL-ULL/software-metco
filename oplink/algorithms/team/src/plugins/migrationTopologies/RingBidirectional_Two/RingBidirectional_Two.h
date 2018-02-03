/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    April 2010
 *
 * DESCRIPTION
 *    Ring Bidirectional topology. It sends the individuals to both connected islands 
 ************************************************************************************************/


#ifndef RINGBIDIRECTIONAL_TWO_H
#define RINGBIDIRECTIONAL_TWO_H

#include "Individual.h"
#include "MigrationTopology.h"

#include <vector>

using namespace std;

class RingBidirectional_Two : public MigrationTopology {
	public:
		void sendTo(const int myId, const int slaveIslands, vector<int> &destination);
};

#endif
