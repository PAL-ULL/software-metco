#include "RingBidirectional_One.h"

void RingBidirectional_One::sendTo(const int myId, const int slaveIslands, vector<int> &destination){
	//Generamos aleatorio entre 0 y 1 para determinar la direccion.
	int direction = (int) ((2.0 * rand()) / (RAND_MAX + 1.0));
	int slave;
	if (direction == 1){
		slave = (myId + 1) % (slaveIslands + 1);
		if (slave == 0)
			slave = 1;
	} else {
		slave = (myId - 1) % (slaveIslands + 1);
		if (slave == 0)
			slave = slaveIslands;
	}
	destination.push_back(slave);
}
