#include "RingBidirectional_Two.h"

void RingBidirectional_Two::sendTo(const int myId, const int slaveIslands, vector<int> &destination){
	//Generamos aleatorio entre 0 y 1 para determinar la direccion.
	int slave1, slave2;
	//Incrementando
	slave1 = (myId + 1) % (slaveIslands + 1);
	if (slave1 == 0)
		slave1 = 1;
	destination.push_back(slave1);
	//Decrementando
	slave2 = (myId - 1) % (slaveIslands + 1);
	if (slave2 == 0)
		slave2 = slaveIslands;
	if (slave1 != slave2)
		destination.push_back(slave2);
}
