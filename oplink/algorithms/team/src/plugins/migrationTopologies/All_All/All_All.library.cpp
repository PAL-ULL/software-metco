#include "All_All.h"

extern "C" {
	MigrationTopology *maker(){
		return new All_All();
	}
}
