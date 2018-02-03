#include "All_One.h"

extern "C" {
	MigrationTopology *maker(){
		return new All_One();
	}
}
