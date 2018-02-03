/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *
 *********************************************************************************************/


#include <dlfcn.h>
#include <sstream>

#include "LocalSearch.h"

LocalSearch *getLocalSearch(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError){
	return ((LocalSearch *)(loadPlugin("LocalSearch", pluginPath, pluginName, args, printError, true)));
}
