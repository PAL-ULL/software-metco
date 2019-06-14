/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 ********************************************************************************************/


#include <dlfcn.h>

#include "OutputPrinter.h"

OutputPrinter *getOutputPrinter(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError){
	return ((OutputPrinter*)(loadPlugin("OutputPrinter", pluginPath, pluginName, args, printError, true)));
}
