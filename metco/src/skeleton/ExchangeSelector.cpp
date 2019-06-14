/***********************************************************************************************
 * AUTHORS
 * - Carlos Segura González
 *
 ***********************************************************************************************/

#include <dlfcn.h>
#include <sstream>

#include "ExchangeSelector.h"

ExchangeSelector *getExchangeSelector(const string &pluginPath, const string &pluginName, vector<string> &args, bool printError){
	return ((ExchangeSelector*)(loadPlugin("ExchangeSelector", pluginPath, pluginName, args, printError, true)));
}
