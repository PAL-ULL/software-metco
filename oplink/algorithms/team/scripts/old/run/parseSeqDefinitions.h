#ifndef PARSE_SEQ_DEFINITIONS_H
#define PARSE_SEQ_DEFINITIONS_H
#include <map>
#include <string>
#include <vector> 
#include <sstream>

#include "parseDefinitions.h"

using namespace std;

//variables
extern string outputFileSeq;
extern FILE *expSeqin;
extern string individualParams;
extern string individualName;

extern map <string, vector<string> > seqParams;

//funciones
int expSeqparse();
void checkSeqCorrectValues();
void resetSeqValues();
#endif
