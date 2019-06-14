/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 ********************************************************************************************/


#include <dlfcn.h>

#include "MigrationTopology.h"

MigrationTopology *getMigrationTopology(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError){
	return ((MigrationTopology*)(loadPlugin("MigrationTopology", pluginPath, pluginName, args, printError, true)));
}
