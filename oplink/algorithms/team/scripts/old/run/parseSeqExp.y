%token REPETITIONS NAME_OF_EXPERIMENT DOS_PUNTOS COMA STRING EXECUTION_TOKEN PREACTION MACHINEFILE INDIVIDUAL_PARAMS OUTPUT_FILE

%{
	#include <vector>
	#include <string>
	#include <iostream>
	#include <sstream>
	#include <stdlib.h>

//	#include "utils.h"
	#include "parseSeqDefinitions.h"
	using namespace std;

	void expSeqerror(char *);
	int expSeqlex();
	vector<string> stringSet;
	string actualParam;
%}

%%

experiment:
	experiment linea
	| linea
	;

linea:
	MACHINEFILE DOS_PUNTOS STRING
	| REPETITIONS DOS_PUNTOS STRING 
			{
				if (repetitions != 0){ 
					cout << "Error: repetitions repeated" << endl; 
					exit(-1); 
				} 
				repetitions = atoi(stringValue.c_str()); 
			}
	| NAME_OF_EXPERIMENT DOS_PUNTOS STRING 
			{ 
				if (name_of_experiment.size() != 0) { 
					cout << "Error: name_of_experiment repeated" << endl; 
					exit(-1); 
				} 
				name_of_experiment = stringValue; 
			} 
	| PREACTION DOS_PUNTOS STRING {}
	| EXECUTION_TOKEN DOS_PUNTOS STRING 
			{ 
				if (execution_line.size() != 0) { 
					cout << "Error: executions repeated" << endl; 
					exit(-1); 
				} 
				execution_line = stringValue;  
			}
	| STRING DOS_PUNTOS {actualParam = stringValue; } set_strings 
			{
				seqParams[actualParam] = stringSet;
				stringSet.clear();
			}
	| INDIVIDUAL_PARAMS DOS_PUNTOS STRING { individualParams = stringValue; }
	| OUTPUT_FILE DOS_PUNTOS STRING { outputFileSeq = stringValue; }			
	;

set_strings:
 	set_strings COMA STRING { stringSet.push_back(stringValue); }
 	| STRING { stringSet.push_back(stringValue); }
	;

%%
	
void expSeqerror(char *s) {
	errorParsing = 1; 
}



