#include "RingBidirectional_One.h"

extern "C" {
	MigrationTopology *maker(){
		return new RingBidirectional_One();
	}
}
