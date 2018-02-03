#include "GlobalAlways.h"

extern "C" {
	InitPopIslandLoader *maker(){
		return new GlobalAlways();
	}
}
