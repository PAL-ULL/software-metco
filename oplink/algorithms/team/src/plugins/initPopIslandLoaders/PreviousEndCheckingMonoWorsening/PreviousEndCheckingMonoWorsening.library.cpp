#include "PreviousEndCheckingMonoWorsening.h"

extern "C" {
	InitPopIslandLoader *maker(){
		return new PreviousEndCheckingMonoWorsening();
	}
}
