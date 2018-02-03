/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 ********************************************************************************************/


#include <dlfcn.h>
#include <sstream>

#include "MigrationSelector.h"

MigrationSelector::MigrationSelector(const int t) : type(t){
	if ((type != SELECT_FROM_ARCHIVE) && (type != SELECT_FROM_POPULATION)){
		cerr << "MigrationSelector type incorrect" << endl;
		exit(-1);
	}
}

MigrationSelector *getMigrationSelector(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError){
	return ((MigrationSelector*)(loadPlugin("MigrationSelector", pluginPath, pluginName, args, printError, true)));
}
