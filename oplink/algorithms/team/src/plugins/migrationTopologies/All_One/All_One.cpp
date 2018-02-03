#include "All_One.h"

void All_One::sendTo(const int myId, const int slaveIslands, vector<int> &destination){
	//Generamos aleatorio entre 1 y slave-1.
	//Si el numero que se genera es >= myId, lo incrementamos en 1
	int slave = 1 + (int) ((double)(slaveIslands) * rand() / (RAND_MAX + 1.0));
	if (slave >= myId)
		slave++;
	destination.push_back(slave);
}
