
#include "Decomposition.h"
#include <dlfcn.h>
#include <sstream>
#include <iostream>

bool Decomposition::init(const vector<string> &params) {
	if(params.size() != 0) {
		std::cerr << "Error loading Decomposition.\n Decomposition does not need args" << std::endl;
 		return false;
	} else {
		return true;
	}
}


Decomposition* getDecomposition(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError){
	return ((Decomposition *)(loadPlugin("Decomposition", pluginPath, pluginName, args, printError, true)));
}
