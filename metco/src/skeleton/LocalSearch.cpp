/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura Gonz�lez
 *
 *********************************************************************************************/


#include <dlfcn.h>
#include <sstream>

#include "LocalSearch.h"

LocalSearch *getLocalSearch(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError){
	return ((LocalSearch *)(loadPlugin("LocalSearch", pluginPath, pluginName, args, printError, true)));
}
