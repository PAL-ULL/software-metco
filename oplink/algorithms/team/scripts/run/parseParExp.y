%token REPETITIONS NUMBER_OF_PROCESSORS NAME_OF_EXPERIMENT CONFIGURATIONS INIT_PERCENT_OF_INDIVIDUALS MIGRATION_PROBABILITY NUMBER_OF_INDIVIDUALS_TO_MIGRATE NUMERO DOS_PUNTOS COMA STRING MAX_GLOBAL_FRONT_SIZE MAX_FINAL_SOLUTION_SIZE SEND_RESULTS_PER_GENERATION SHARING EXECUTION_LINE MACHINEFILE PREACTION CORCHETE_ABIERTO CORCHETE_CERRADO LLAVE_ABIERTA LLAVE_CERRADA EXECUTION_MODEL FLECHA CONVERSION CONVERSIONPARAMS SCORE INIT_POP_ISLAND_LOADER  INIT_POP_ISLAND_LOADER_PARAMS MIGRATION_TOPOLOGY MIGRATION_TOPOLOGY_PARAMS PLUGIN_PATH NUMBER_PARALLEL_EXECUTIONS

%{
	#include <vector>
	#include <string>
	#include <iostream>
	#include <sstream>
	#include <stdlib.h>
	#include <string.h>

	using namespace std;

	#include "parseParDefinitions.h"
	void expParerror(char *);
	int expParlex();
	vector<double> realBuffer; 
	vector<string> stringBuffer;
	stringstream tmpString;
	bool executionModelDefined;
	bool sendResultsPerGenerationFixed;
	bool sharingFixed;

	void comp_mueve(vector<double> &destino, vector<double> &origen, const string &error);
	void comp_mueve(vector<int> &destino, vector<double> &origen, const string &error);
	void comp_mueve(vector<string> &destino, vector<string> &origen, const string &error);
	void reset();
%}

%%

inicio: 
	{ executionModelDefined = false; sendResultsPerGenerationFixed = false; sharingFixed = false; } experiment
	;
	
experiment:
	experiment linea
	| linea
	;

linea:
	REPETITIONS DOS_PUNTOS NUMERO { repetitions = (int)realValue; }
	| EXECUTION_MODEL DOS_PUNTOS execution_models { if (executionModelDefined){ cout << "Error: executionModel redefined" << endl; exit(-1); } executionModelDefined = true; }
	| NUMBER_OF_PROCESSORS DOS_PUNTOS set_numeros { comp_mueve(number_of_processors, realBuffer, "Number_of_processors"); }
	| NAME_OF_EXPERIMENT DOS_PUNTOS STRING { name_of_experiment = stringValue; } 
	| MACHINEFILE DOS_PUNTOS STRING { machinefile = stringValue; } 	
	| CONFIGURATIONS DOS_PUNTOS set_strings { comp_mueve(configurations, stringBuffer, "Configurations"); } 
	| INIT_PERCENT_OF_INDIVIDUALS DOS_PUNTOS set_numeros { comp_mueve(init_percent_of_individuals, realBuffer, "Init_percent_of_individuals"); }
	| MIGRATION_PROBABILITY DOS_PUNTOS set_numeros { comp_mueve(migration_probability, realBuffer, "Migration_probability"); }
	| NUMBER_OF_INDIVIDUALS_TO_MIGRATE DOS_PUNTOS set_numeros { comp_mueve(number_of_individuals_to_migrate, realBuffer, "Number_of_individuals_to_migrate"); }
	| MAX_GLOBAL_FRONT_SIZE DOS_PUNTOS set_numeros { comp_mueve(max_global_front_size, realBuffer, "Max_front_size"); }
	| MAX_FINAL_SOLUTION_SIZE DOS_PUNTOS set_numeros { comp_mueve(max_final_solution_size, realBuffer, "Max_final_solution_size"); }
	| NUMBER_PARALLEL_EXECUTIONS DOS_PUNTOS set_numeros { comp_mueve(number_parallel_executions, realBuffer, "Number_parallel_executions"); }
	| SEND_RESULTS_PER_GENERATION DOS_PUNTOS STRING 
			{
				if (sendResultsPerGenerationFixed){
					cout << "Error: Send_results_per_generation redefined" << endl;
					exit(-1);
				}
				sendResultsPerGenerationFixed = true;
				if (strcasecmp("yes", stringValue.c_str()) == 0){
					sendResultsPerGenerationBool = true;
				} 
				else if (strcasecmp("no", stringValue.c_str()) == 0){
					sendResultsPerGenerationBool = false;
				} else {
					cout << "Error: Send_results_per_generation not recognized value" << endl;
					exit(-1);
				}

			}
	| EXECUTION_LINE DOS_PUNTOS STRING { execution_line = stringValue; } 
	| PREACTION DOS_PUNTOS STRING {}
	| CONVERSION DOS_PUNTOS STRING { conversion = stringValue; }
	| CONVERSIONPARAMS DOS_PUNTOS STRING { conversionParams = stringValue; }
	| INIT_POP_ISLAND_LOADER DOS_PUNTOS STRING { initPopIslandLoader = stringValue; }
	| INIT_POP_ISLAND_LOADER_PARAMS DOS_PUNTOS STRING { initPopIslandLoaderParams = stringValue; }
	| MIGRATION_TOPOLOGY DOS_PUNTOS STRING { migrationTopology = stringValue; }
	| MIGRATION_TOPOLOGY_PARAMS DOS_PUNTOS STRING { migrationTopologyParams = stringValue; }
	| SCORE DOS_PUNTOS scores
	| PLUGIN_PATH DOS_PUNTOS STRING { pluginPath = stringValue; }
	;

scores:
	scores LLAVE_ABIERTA { Score emptyScore; scores.push_back(emptyScore); } CORCHETE_ABIERTO STRING CORCHETE_CERRADO { scores[scores.size()-1].moduleName = stringValue; } FLECHA CORCHETE_ABIERTO set_strings_set_numeros_o_vacio CORCHETE_CERRADO { scores[scores.size()-1].params = stringBuffer; stringBuffer.clear(); } FLECHA CORCHETE_ABIERTO STRING CORCHETE_CERRADO { scores[scores.size()-1].id = stringValue; } LLAVE_CERRADA
	| LLAVE_ABIERTA { Score emptyScore; scores.push_back(emptyScore); } CORCHETE_ABIERTO STRING CORCHETE_CERRADO { scores[scores.size()-1].moduleName = stringValue; } FLECHA CORCHETE_ABIERTO set_strings_set_numeros_o_vacio CORCHETE_CERRADO { scores[scores.size()-1].params = stringBuffer; stringBuffer.clear(); } FLECHA CORCHETE_ABIERTO STRING CORCHETE_CERRADO { scores[scores.size()-1].id = stringValue; } LLAVE_CERRADA
	;

execution_models:
	execution_models LLAVE_ABIERTA { execution_model.push_back(vector<string>()); } execution_model FLECHA CORCHETE_ABIERTO STRING CORCHETE_CERRADO { execution_model_names.push_back(stringValue); } LLAVE_CERRADA
	| LLAVE_ABIERTA { execution_model.push_back(vector<string>()); } execution_model FLECHA CORCHETE_ABIERTO STRING CORCHETE_CERRADO { execution_model_names.push_back(stringValue); } LLAVE_CERRADA
	;

execution_model:
	execution_model CORCHETE_ABIERTO STRING { tmpString.str(""); tmpString << stringValue; } COMA NUMERO { tmpString << ", " << (long)realValue; } COMA STRING CORCHETE_CERRADO { tmpString << ", " << stringValue; execution_model[execution_model.size() - 1].push_back(tmpString.str()); }
  | CORCHETE_ABIERTO STRING { tmpString.str(""); tmpString << stringValue; } COMA NUMERO { tmpString << ", " << (long)realValue; } COMA STRING CORCHETE_CERRADO { tmpString << ", " << stringValue; execution_model[execution_model.size() - 1].push_back(tmpString.str()); }
  | execution_model CORCHETE_ABIERTO STRING CORCHETE_CERRADO { execution_model[execution_model.size() - 1].push_back(stringValue); }
	| CORCHETE_ABIERTO STRING CORCHETE_CERRADO { execution_model[execution_model.size() - 1].push_back(stringValue); }
	;

set_strings:
  	set_strings COMA STRING { stringBuffer.push_back(stringValue); }
  	| STRING { stringBuffer.push_back(stringValue); }
	;

set_strings_set_numeros_o_vacio:
  	set_strings_set_numeros_o_vacio COMA STRING { stringBuffer.push_back(stringValue); }
		| set_strings_set_numeros_o_vacio COMA NUMERO { stringstream st; st.setf(ios::fixed); st << realValue; stringBuffer.push_back(st.str()); }
  	| STRING { stringBuffer.push_back(stringValue); }
		| NUMERO { stringstream st; st.setf(ios::fixed); st << realValue; stringBuffer.push_back(st.str()); }
		|
	;

set_numeros:
	set_numeros COMA NUMERO { realBuffer.push_back(realValue); }
	| NUMERO { realBuffer.push_back(realValue); } 
	;

%%

void comp_mueve(vector<double> &destino, vector<double> &origen, const string &cad){
	if (destino.size() != 0){
		cout << "Error_: " << cad << " repeated" << endl;
		exit(-1);
	}
	destino = origen;
	origen.clear();
}

void comp_mueve(vector<int> &destino, vector<double> &origen, const string &cad){
	if (destino.size() != 0){
		cout << "Error_2: " << cad << " repeated" << endl;
		exit(-1);
	}
	for (unsigned int i = 0; i < origen.size(); i++)
		destino.push_back((int)origen[i]);
	origen.clear();
}

void comp_mueve(vector<string> &destino, vector<string> &origen, const string &cad){
	if (destino.size() != 0){
		cout << "Error_3: " << cad << " repeated" << endl;
		exit(-1);
	}
	destino = origen;
	origen.clear();
}

void expParerror(char *s) {
	errorParsing = 1; 
}



