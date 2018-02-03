/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *
 *********************************************************************************************/


#include <dlfcn.h>
#include <sstream>

#include "Mutation.h"

Mutation *getMutation(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError){
	return ((Mutation *)(loadPlugin("Mutation", pluginPath, pluginName, args, printError, true)));
}
