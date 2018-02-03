#include "parseParDefinitions.h"
#include <map>
#include <string>
#include <vector> 
#include <sstream>
#include <iostream>
#include <stdlib.h>
using namespace std;

//variables
vector<int> number_of_processors;
vector<string> configurations;
vector<Score> scores;
vector< vector<string> > execution_model;
vector<string> execution_model_names;
vector<double> init_percent_of_individuals;
vector<double> migration_probability;
vector<int> number_of_individuals_to_migrate;
vector<int> max_global_front_size;
vector<int> max_final_solution_size;
vector<int> number_parallel_executions;
bool sendResultsPerGenerationBool;
string machinefile;
string conversion;
string conversionParams;
string pluginPath;
string migrationTopology;
string migrationTopologyParams;
string initPopIslandLoader;
string initPopIslandLoaderParams;

void resetParValues(){
	repetitions = 0;
	errorParsing = 0;
	number_of_processors.clear();
	number_parallel_executions.clear();
	name_of_experiment = "";
	configurations.clear();
	scores.clear();
	execution_model.clear();
	execution_model_names.clear();
	init_percent_of_individuals.clear();
	migration_probability.clear();
	number_of_individuals_to_migrate.clear();
	max_global_front_size.clear();
	max_final_solution_size.clear();
	execution_line.clear();
	machinefile = "";
	sendResultsPerGenerationBool = false;
	conversion = "";
	conversionParams = "";
	pluginPath = "";
	migrationTopology = "";
	migrationTopologyParams = "";
	initPopIslandLoader = "";
	initPopIslandLoaderParams = "";
}

void checkParCorrectValues(){
	if (errorParsing != 0){
		cout << "formato del fichero de configuracion incorrecto" << endl;
		exit(-1);
	}
	if (repetitions == 0){
		cout << "Repetitions non defined" << endl;
		exit(-1);
	}
	if (number_of_processors.size() == 0){
		cout << "Number_of_processors non defined" << endl;
		exit(-1);
	}
	if (name_of_experiment.length() == 0){
		cout << "Name_of_experiment non defined" << endl;
		exit(-1);
	} 
	if (configurations.size() == 0){
		cout << "Configurations non defined" << endl;
		exit(-1);
	}
	if (scores.size() == 0){
		cout << "Scores non defined" << endl;
		exit(-1);
	}
	if (execution_model.size() == 0){
		cout << "Execution_model non defined" << endl;
		exit(-1);
	}
	if (init_percent_of_individuals.size() == 0){
		cout << "Init_percent_of_individuals non defined" << endl;
		exit(-1);
	}
	if (migration_probability.size() == 0){
		cout << "Migration_probability non defined" << endl;
		exit(-1);
	}
	if (number_of_individuals_to_migrate.size() == 0){
		cout << "Number_of_individuals_to_migrate non defined" << endl;
		exit(-1);
	}
	if (max_global_front_size.size() == 0){
		cout << "Max_front_size non defined" << endl;
		exit(-1);
	}
	if (max_final_solution_size.size() == 0){
		cout << "Max_final_solutions_size non defined" << endl;
		exit(-1);
	}
	if (execution_line.length() == 0){
		cout << "Execution_line non defined" << endl;
		exit(-1);
	}
	if (conversion.length() == 0){
		cout << "Conversion non defined" << endl;
		exit(-1);
	}
	if (initPopIslandLoader.length() == 0){
		cout << "InitPopIslandLoader non defined" << endl;
		exit(-1);
	}
	if (migrationTopology.length() == 0){
		cout << "MigrationTopology non defined" << endl;
		exit(-1);
	}
	if (pluginPath.length() == 0){
		cout << "Plugin Path non defined" << endl;
		exit(-1);
	}
	if (number_parallel_executions.size() == 0){
		cout << "Number_parallel_executions non defined" << endl;
		exit(-1);
	}
}
