#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "coverExperiments.h"
#include "parseSeqDefinitions.h"
#include "parseParDefinitions.h"
#include "parseLineDefinitions.h"

using namespace std;

stringstream nameOfExperimentDir;
bool dirCreado = false;
ofstream experiment_list;

void experimentInit(){
	if (!dirCreado){
		nameOfExperimentDir << "experimentsTmpFiles/" << get_variable("expName");
		if (mkdir(nameOfExperimentDir.str().c_str(), S_IRWXU) == -1){//could not create experiments directory
			if (errno != EEXIST){
				cout << "Error: directory " << nameOfExperimentDir <<  "could not be created";
				exit(-1);
			}
		}
		dirCreado = true;
		experiment_list.open((nameOfExperimentDir.str() + "/experiment_list").c_str());
		if (!experiment_list.is_open()){
			cout << "File " << nameOfExperimentDir.str() << "/experiment_list could not be opened" << endl;
			exit(-1);
		}
	}
}

void createSeqExecutionFile(int repetitions){
	experimentInit();

	for (int rep = 0; rep < repetitions; rep++){
		set_variable("rep", rep);
		line_parser(execution_line.c_str(), "Execution");
		experiment_list << line_parsed.str() << endl;
	}
}

void createParExecutionFile(int repetitions){
	experimentInit();
	
	//Execution File
	char *tmp = new char[get_variable("algFile").length() + 1];
	strcpy(tmp, get_variable("algFile").c_str());
	stringstream nameConfFile;
	nameConfFile << nameOfExperimentDir.str() << "/" << basename(tmp) << "_" << get_variable("execModelName") << "_" << get_variable("scoreId") << "_" << get_variable("initPerc") << "_" << get_variable("migProb") << "_" << get_variable("numIndMig") << "_" << get_variable("maxGlobalFrontSize") << "_" << get_variable("maxFinalSolutionSize");
	set_variable("confFile", nameConfFile.str());
	stringstream outputFile;
	for (int rep = 0; rep < repetitions; rep++){
		set_variable("rep", rep);
		outputFile.str("");
		outputFile << get_variable("expName") << "_" << basename(tmp) << "_" << get_variable("np") << "_" << get_variable("execModelName") << "_" << get_variable("scoreId") << "_" << get_variable("initPerc") << "_" << get_variable("migProb") << "_" << get_variable("numIndMig") << "_" << get_variable("maxGlobalFrontSize") << "_" << get_variable("maxFinalSolutionSize") << "." << rep;
		set_variable("outfile", outputFile.str());
		line_parser(execution_line.c_str(), "ExecLine");
		experiment_list << line_parsed.str() << endl;
	}

	//create Configuration file
	stringstream configurationBuffer;
	configurationBuffer.str("");
	ifstream onlyConfFile(get_variable("algFile").c_str()); 
	if (!onlyConfFile.is_open()){
		cerr << "File " << get_variable("algFile") << " does not exist\n" << endl;
		exit(-1);
	} else {
		char c; 
		while (!onlyConfFile.eof()){
			onlyConfFile.get(c);
			if (onlyConfFile.eof())
				break;
			configurationBuffer << c;
		}
		onlyConfFile.close();
	}
		
	ofstream confFile(nameConfFile.str().c_str()); 
	if (!confFile.is_open()){
		cerr << "Error: file " << nameConfFile.str() << " could not be created" << endl; 
		exit(-1);
	}
	//write Configuration Parameters
	confFile << "Execution_model: ";
	int modelNumber = atoi(get_variable("execModelNumber").c_str());
	for (int r = 0; r < execution_model[modelNumber].size(); r++)
		confFile << "[ " << execution_model[modelNumber][r] << " ]" << endl;
	confFile << "Plugin_path: " << get_variable("pluginPath") << endl;
	confFile << "Init_percent_of_individuals: " << get_variable("initPerc") << endl;
	confFile << "Migration_probability: " << get_variable("migProb") << endl;
	confFile << "Number_of_individuals_to_migrate: " << get_variable("numIndMig") << endl;
	confFile << "Max_global_front_size: " << get_variable("maxGlobalFrontSize") << endl;
	confFile << "Max_final_solution_size: " << get_variable("maxFinalSolutionSize") << endl;
	confFile << "Send_results_per_generation: " << get_variable("sendResultsPerGeneration") << endl;
	confFile << "Conversion: " << get_variable("conversion") << endl;
	confFile << "Init_Pop_Island_Loader: " << get_variable("initPopIslandLoader") << endl;
	confFile << "Number_parallel_executions: " << get_variable("numberParallelExecutions") << endl;
	if (get_variable("initPopIslandLoaderParams").length() != 0){
		confFile << "Init_Pop_Island_LoaderParams: " << get_variable("initPopIslandLoaderParams") << endl;
	}
	confFile << "Migration_Topology: " << get_variable("Migration_Topology") << endl;
	if (get_variable("Migration_TopologyParams").length() != 0){
		confFile << "Migration_TopologyParams: " << get_variable("Migration_TopologyParams") << endl;
	}
	if (get_variable("conversionParams").length() != 0){
		confFile << "ConversionParams: " << get_variable("conversionParams") << endl;
	}
	confFile << "Score: " << get_variable("score") << endl;
	if (get_variable("scoreParams").length() != 0){
		confFile << "ScoreParams: " << get_variable("scoreParams") << endl;
	}
	confFile << endl;
	//Algorithms
	confFile << configurationBuffer.str(); 
	confFile.close();
	delete(tmp);
}

void prepareExecution(int repetitions, int tipo){
	if (tipo == SEQUENTIAL_ANALYSIS){
		createSeqExecutionFile(repetitions);
	} else {
		createParExecutionFile(repetitions);
	}
}

int main(int argc, char *argv[]){
	if (argc != 2){
		cout << "Uso " << argv[0] << " fich_experimento" << endl;
		return -1;
	}
	//create experimentsTmpFiles/nameExp directory
	if (mkdir("experimentsTmpFiles", S_IRWXU) == -1){//could not create experiments directory
		if (errno != EEXIST){
			cout << "experiments directory could not be created" << endl;
			exit(-1);
		}
	}
	analyzeExperiment(argv[1], &prepareExecution);	
	experiment_list.close();
}
