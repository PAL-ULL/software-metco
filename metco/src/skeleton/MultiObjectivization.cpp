/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *   Eduardo Segredo Gonzalez
 *
 * DATE
 *    November 2010
 *********************************************************************************************/


#include <dlfcn.h>
#include <sstream>

#include "MultiObjectivization.h"

MultiObjectivization* getMultiObjectivization(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError){
	return ((MultiObjectivization *)(loadPlugin("MultiObjectivization", pluginPath, pluginName, args, printError, true)));
}
