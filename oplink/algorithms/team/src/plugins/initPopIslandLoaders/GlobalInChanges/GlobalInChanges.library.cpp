#include "GlobalInChanges.h"

extern "C" {
	InitPopIslandLoader *maker(){
		return new GlobalInChanges();
	}
}
