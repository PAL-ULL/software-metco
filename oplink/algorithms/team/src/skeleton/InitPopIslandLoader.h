/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 * 
 * DATE
 * 	 April 2010
 *
 * DESCRIPTION
 *   Plugin base class to represent an Initial Population Loader for the slave islands 
 *********************************************************************************************/


#ifndef INIT_POP_ISLAND_LOADER_H
#define INIT_POP_ISLAND_LOADER_H

#define GLOBAL 1
#define PREVIOUSINIT 2
#define PREVIOUSEND 3

#include "Individual.h"
#include "Plugin.h"

#include <vector>

using namespace std;

class InitPopIslandLoader: public Plugin {
	public:
		virtual void loadIslandInitPop(const int previousAlg, const int previousConf, const int newAlg, const int newConf, const vector< Individual * > *previousInit, const vector< Individual * > *previousEnd, const vector< Individual * > *global, vector <Individual * > &newPopulation, vector<int> &origin) const = 0;
		virtual bool usePreviousInit() const { return false; }
		virtual bool usePreviousEnd() const { return false; }
		virtual bool useGlobal() const { return false; }

};

InitPopIslandLoader *getInitPopIslandLoader(const string &pluginPath, const string &initPopIslandLoaderName, const vector<string> &args, bool printError);
#endif
