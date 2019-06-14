%{
#include <string.h>
#include <string>
#include "parseSeqDefinitions.h"
#include "parseSeqExp.tab.h"

using namespace std;

%}

%x NEXT_COMA_CORCH
%x RESTO_LINEA

%%

[\n\t ]*                           {}
machinefile                       { BEGIN RESTO_LINEA; return MACHINEFILE; }
Preaction                         { BEGIN RESTO_LINEA; return PREACTION; }
Execution                         { BEGIN RESTO_LINEA; return EXECUTION_TOKEN; }
Repetitions                       { return REPETITIONS; }
Name_of_experiment                { return NAME_OF_EXPERIMENT; }
IndividualParams                  { BEGIN RESTO_LINEA; return INDIVIDUAL_PARAMS; }
OutputFile                        { BEGIN RESTO_LINEA; return OUTPUT_FILE; }
<RESTO_LINEA,INITIAL>:            { return DOS_PUNTOS; }
\,                                { return COMA; }
[a-zA-Z0-9._/\-][a-zA-Z0-9/._]*		  { stringValue = yytext; return STRING; }
<RESTO_LINEA>[^: \t\n][^,\n]*     { BEGIN INITIAL; stringValue = yytext; return STRING; }
<RESTO_LINEA>[ \t]*               {}

%%

int yywrap(){ return 1; }
