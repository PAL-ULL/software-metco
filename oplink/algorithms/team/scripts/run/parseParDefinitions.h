#ifndef PARSE_PAR_DEFINITIONS_H
#define PARSE_PAR_DEFINITIONS_H

#include "parseDefinitions.h"
#include <map>
#include <string>
#include <vector> 
#include <sstream>

using namespace std;

typedef struct Score {
	string moduleName;
	vector<string> params;
	string id;
} Score;

//variables
extern vector<int> number_of_processors;
extern vector<string> configurations;
extern vector<Score> scores;
extern vector< vector<string> > execution_model;
extern vector<string> execution_model_names;
extern vector<double> init_percent_of_individuals;
extern vector<double> migration_probability;
extern vector<int> number_of_individuals_to_migrate;
extern vector<int> max_global_front_size;
extern vector<int> max_final_solution_size;
extern vector<int> number_parallel_executions;
extern string machinefile;
extern string conversion;
extern string conversionParams;
extern string migrationTopology;
extern string migrationTopologyParams;
extern string initPopIslandLoader;
extern string initPopIslandLoaderParams;
extern bool sendResultsPerGenerationBool;
extern FILE *expParin;
extern string pluginPath;

//funciones
int expParparse();
extern void expParrestart(FILE *y);
void checkParCorrectValues();
void resetParValues();

#endif
