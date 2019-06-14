/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 ********************************************************************************************/


#include <dlfcn.h>

#include "InitPopIslandLoader.h"

InitPopIslandLoader *getInitPopIslandLoader(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError){
	return ((InitPopIslandLoader*)(loadPlugin("InitPopIslandLoader", pluginPath, pluginName, args, printError, true)));
}
