%token EXPERIMENT_FILES CONTRIBUTION COVERAGE UNIONS CORCHETE_ABIERTO CORCHETE_CERRADO LLAVE_ABIERTA LLAVE_CERRADA COMA STRING DOS_PUNTOS PREACTION FLECHA NUMBER GENERAL_FRONT_NAME

%{
	#include <vector>
	#include <string>
	#include <iostream>
	#include <sstream>
	#include <stdlib.h>

	using namespace std;

	#include "metricDefinitions.h"
	void metricFileerror(char *);
	int metricFilelex();
	vector<string> stringBufferMetric;
	pair<string,string> pairString;
	pair< pair<string,string>, pair<string,string> > pairpairString;
	vector<pair <string,string> > setPairStrings;

	void comp_mueve_metric(vector<string> &destino, vector<string> &origen, const string &cad);
%}

%%

metric: 
	metric linea
	| linea
	;

linea:
	EXPERIMENT_FILES DOS_PUNTOS CORCHETE_ABIERTO set_strings CORCHETE_CERRADO { comp_mueve_metric(experimentFiles, stringBufferMetric, "ExperimentFiles"); }
	| CONTRIBUTION DOS_PUNTOS LLAVE_ABIERTA 
		{ 
			if (contributions.size() != 0){ 
				cout << "Error: contributions repeated" << endl; 
				exit(-1); 
			}
		}
		conjunto_contributions LLAVE_CERRADA
	| COVERAGE DOS_PUNTOS LLAVE_ABIERTA 
		{ 
			if (coverages.size() != 0){ 
				cout << "Error: contributions repeated" << endl; 
				exit(-1); 
			}
		}
		conjunto_coverage LLAVE_CERRADA
	| UNIONS DOS_PUNTOS LLAVE_ABIERTA 
		{ 
			if (unions.size() != 0){ 
				cout << "Error: contributions repeated" << endl; 
				exit(-1); 
			}
		}
		conjunto_unions LLAVE_CERRADA
	| PREACTION DOS_PUNTOS STRING { system(stringValueMetric.c_str()); } 
	| GENERAL_FRONT_NAME DOS_PUNTOS STRING { generalFrontName = stringValueMetric; }
	;
	
conjunto_contributions:
	conjunto_contributions COMA contribution 
	| contribution 
	;

contribution:
  CORCHETE_ABIERTO set_pair_strings CORCHETE_CERRADO FLECHA STRING  
		{
			contributions.push_back(pair< vector<pair<string,string> > , string >(setPairStrings, stringValueMetric)); 
			setPairStrings.clear();
		}
  ;

conjunto_coverage:
	conjunto_coverage COMA coverage
	| coverage
	;

coverage:
	CORCHETE_ABIERTO CORCHETE_ABIERTO STRING { pairString.first = stringValueMetric; } COMA STRING { pairString.second = stringValueMetric; pairpairString.first = pairString; } CORCHETE_CERRADO COMA CORCHETE_ABIERTO STRING {pairString.first = stringValueMetric; } COMA STRING CORCHETE_CERRADO CORCHETE_CERRADO
 		{
			pairString.second = stringValueMetric;
			pairpairString.second = pairString;
			coverages.push_back(pairpairString); 
		}
	;

conjunto_unions:
	conjunto_unions COMA union
	| union
	;

union:
	CORCHETE_ABIERTO set_pair_strings CORCHETE_CERRADO FLECHA CORCHETE_ABIERTO STRING 
 		{
			unions.push_back(pair < vector<pair<string,string> >, pair<string,int> >(setPairStrings, pair<string,int>(stringValueMetric, 0)));
			setPairStrings.clear();
		}
		COMA NUMBER CORCHETE_CERRADO
		{
			unions[unions.size()-1].second.second = intValueMetric;
		}
	;

set_pair_strings: 
	set_pair_strings COMA CORCHETE_ABIERTO STRING {pairString.first = stringValueMetric; } COMA STRING { pairString.second = stringValueMetric; setPairStrings.push_back(pairString); } CORCHETE_CERRADO
	| CORCHETE_ABIERTO STRING {pairString.first = stringValueMetric; } COMA STRING { pairString.second = stringValueMetric; setPairStrings.push_back(pairString); } CORCHETE_CERRADO

set_strings:
	set_strings COMA STRING { stringBufferMetric.push_back(stringValueMetric); }
	| STRING { stringBufferMetric.push_back(stringValueMetric); }
	;

%%

void comp_mueve_metric(vector<string> &destino, vector<string> &origen, const string &cad){
	if (destino.size() != 0){
		cout << "Error: " << cad << " repeated" << endl;
		exit(-1);
	}
	destino = origen;
	origen.clear();
}

void metricFileerror(char *s) {
   errorParsingMetric = 1; 
}
