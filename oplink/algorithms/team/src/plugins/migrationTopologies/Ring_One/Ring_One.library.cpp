#include "Ring_One.h"

extern "C" {
	MigrationTopology *maker(){
		return new Ring_One();
	}
}
