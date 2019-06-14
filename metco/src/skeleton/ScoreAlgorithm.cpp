/*********************************************************************************************
 * AUTHORS
 *   Carlos Segura González
 *
 *********************************************************************************************/


#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <dlfcn.h>
#include <stdlib.h>


#include "ScoreAlgorithm.h"

ScoreAlgorithm::ScoreAlgorithm(const int type) { 
	if ( (type != GLOBAL_SCORE_ALGORITHM) && (type != LOCAL_SCORE_ALGORITHM) ){
		cerr << "Incorrect Score Algorithm module" << endl;
		exit(-1);
	}
	this->type = type; 
}

bool ScoreAlgorithm::init(const vector<string> &params) {
	if (params.size() != 0){
		cerr << "Warning: parameters received, but ignored by Score Algorithm" << endl;
	}
	return true;
}

void ScoreAlgorithm::setConfigurations(const vector<int> &configs){ 
	this->configs = configs;
	numberOfConfigurations = 0;
	for (int i = 0; i < configs.size(); i++){
		numberOfConfigurations += configs[i];
	}
}

ScoreAlgorithm *getScoreAlgorithm(const string &pluginPath, const string &name, const vector<string> &scoreArgs, bool printError){
	return ((ScoreAlgorithm*) (loadPlugin("ScoreAlgorithm", pluginPath, name, scoreArgs, printError, true)));
}
