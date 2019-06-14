#include "Ring_One.h"

void Ring_One::sendTo(const int myId, const int slaveIslands, vector<int> &destination){
	int slave;
	slave = (myId + 1) % (slaveIslands + 1);
	if (slave == 0)
		slave = 1;
	destination.push_back(slave);
}
