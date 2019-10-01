#include "parseSeqDefinitions.h"

#include <map>
#include <string>
#include <vector> 
#include <string>
#include <iostream>

#include <stdlib.h>

using namespace std; 

string outputFileSeq;
map <string, vector<string> > seqParams;
string individualParams, individualName;

void checkSeqCorrectValues(){
	if (repetitions == 0){
		cout << "Repetitions non defined" << endl;
		exit(-1);
	}
	if (name_of_experiment.length() == 0){
		cout << "Name_of_experiment non defined" << endl;
		exit(-1);
	} 
	if (execution_line.size() == 0){
		cout << "Execution non defined" << endl;
		exit(-1);
	}
}

void resetSeqValues(){
	seqParams.clear();
	individualParams = "";
	repetitions = 0;
	errorParsing = 0;
	name_of_experiment = "";
	execution_line.clear();
}
