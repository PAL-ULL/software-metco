/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *
 * DESCRIPTION
 *   Plugin base class to represent score algorithms. End users make use of LocalScoreAlgorithm
 *   or GlobalScoreAlgorithm (not this one)
 *********************************************************************************************/


#ifndef SCORE_ALGORITHM_H
#define SCORE_ALGORITHM_H

#include <vector>
#include <string>
#include "Plugin.h"
#include "Definitions.h"

using namespace std;

#define GLOBAL_SCORE_ALGORITHM 1
#define  LOCAL_SCORE_ALGORITHM 2
class ScoreAlgorithm : public Plugin{
	public:
		virtual ~ScoreAlgorithm() {}
		ScoreAlgorithm(const int type);
		inline int getType() const { return type; }
		virtual bool init(const vector<string> &params) ;
		virtual void begins(const int algId, const int confId, const int islandId) { }
		virtual void ends(const int algId, const int confId, const int islandId) { }
		void setConfigurations(const vector<int> &configs);
		virtual int getDirection() const = 0;
	protected:
		int getNumberOfAlgorithms() { return configs.size(); }
		int getNumberOfConfigurations(int algId){ return configs[algId]; }
		int getNumberOfConfigurations() { return numberOfConfigurations; }
						
	private:
		int numberOfConfigurations;
		int type;
		vector<int> configs;
};

ScoreAlgorithm *getScoreAlgorithm(const string &pluginPath, const string &pluginName, const vector<string> &pluginArgs, bool printError);

#endif
