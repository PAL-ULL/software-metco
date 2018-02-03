/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *
 * DATE
 *    July 2008
 *********************************************************************************************/


#include <dlfcn.h>
#include <sstream>

#include "Crossover.h"

Crossover *getCrossover(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError){
	return ((Crossover*)(loadPlugin("Crossover", pluginPath, pluginName, args, printError, true)));
}
