#include "RingBidirectional_Two.h"

extern "C" {
	MigrationTopology *maker(){
		return new RingBidirectional_Two();
	}
}
