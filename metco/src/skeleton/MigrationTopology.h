/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 * 
 * DATE
 * 	 April 2010
 *
 * DESCRIPTION
 *   Plugin base class to represent a migration topology 
 *********************************************************************************************/


#ifndef MIGRATION_TOPOLOGY_H
#define MIGRATION_TOPOLOGY_H

#include "Individual.h"
#include "Plugin.h"

#include <vector>

using namespace std;

class MigrationTopology : public Plugin {
	public:
		virtual void sendTo(const int myId, const int slaveIslands, vector<int> &destination) = 0;
};

MigrationTopology *getMigrationTopology(const string &pluginPath, const string &migrationTopologyName, const vector<string> &args, bool printError);
#endif
