
#include "Decomposition.h"
#include <dlfcn.h>
#include <sstream>
#include <iostream>

bool Decomposition::init(const vector<string> &params) {
	if(params.size() != 1) {
		std::cerr << "Error loading Decomposition" << std::endl;
		std::cerr << "direction must be specified: ";
		std::cerr << " <0 = MINIMIZE | 1 = MAXIMIZE>" << std::endl;
 		return false;
	} else {
		int dir = stoi(params[0].c_str());
		if(dir != MINIMIZE && dir != MAXIMIZE) {
			std::cerr << "Error: direction must be  <0 = MINIMIZE | 1 = MAXIMIZE>" << endl;
			return false;
		} else {
			direction = dir;
		}
		return true;
		std::cout << "Decomposition init done. Direction: " << direction << std::endl;
	}
}


Decomposition* getDecomposition(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError){
	return ((Decomposition *)(loadPlugin("Decomposition", pluginPath, pluginName, args, printError, true)));
}
