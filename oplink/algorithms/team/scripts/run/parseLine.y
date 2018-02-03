%token STRING VARIABLE 

%{
	#include <map>
	#include <string>
	#include <iostream>
	#include <sstream>
	#include <stdlib.h>
	#include "parseDefinitions.h"
	#include "parseLineDefinitions.h"

	using namespace std;
	
	void Lineerror(char *);
	int Linelex();
	void LineparseStr(char *s);

%}

%%

linea: linea texto
	| texto
	;

texto:  STRING { line_parsed << stringValue; }
	| VARIABLE
		{
			map<string,string>::iterator iter = variables.find(stringValue);
			if ( iter == variables.end() ){
				cout << "Error: variable " << stringValue << " non-defined" << endl;
				exit(-1);
			}
			line_parsed << iter->second;
		}
	;

%%

void Lineerror(char *s) {
   errorParsing = 1; 
}
