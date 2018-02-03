#include "getIndividuals.h"
#include "Configuration.h"
#include "utils.h"
#include "parseLineDefinitions.h"

#include <iostream>

using namespace std;

void getIndividualSequentialExperiment(string baseDirectory){
	if (individualName.length() == 0){
		cout << "Error: no se pudo determinar el individuo del experimento " << baseDirectory << endl << flush;
		exit(-1);
	}
	ind = getInd(baseDirectory + "/", individualName, splitString(individualParams), true);
}

void getIndividualParallelExperiment(string baseDirectory){
	if (configurations.size() == 0){
		cout << "Error: no se pudo determinar el individuo del experimento " << baseDirectory << endl << flush;
		exit(-1);
	}
	Configuration configuration;
	configuration.setBaseDirectory(baseDirectory);
	configuration.setPluginPath(get_variable("pluginPath"));
	configuration.load(configurations[0], 0);
	if (configuration.getTeamAlgorithms().size() == 0){
		cout << "error: no se pudo determinar el individuo del experimento " << baseDirectory << endl << flush;
		exit(-1);
	}
	if (configuration.getTeamAlgorithms()[0].ind == NULL){
		cout << "Error: no se pudo determinar el individuo del experimento " << baseDirectory << endl << flush;
		exit(-1);
	}
	individualName = configuration.getTeamAlgorithms()[0].individualName;
	individualParams = configuration.getTeamAlgorithms()[0].individualParams;
	ind = configuration.getTeamAlgorithms()[0].ind;
}

Individual *ind = NULL;
