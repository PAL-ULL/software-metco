#include "All_All.h"

void All_All::sendTo(const int myId, const int slaveIslands, vector<int> &destination){
	for (int i = 1; i <= slaveIslands; i++){
		if (myId != i)
			destination.push_back(i);
	}
}
