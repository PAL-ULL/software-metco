%{
#include <string.h>
#include <string>

#include "parseMetric.tab.h"
#include "metricDefinitions.h"

using namespace std;

%}
%x RESTO_LINEA

%%

[ \t\n]*                         {}
ExperimentFiles                  { return EXPERIMENT_FILES; }
Contribution                     { return CONTRIBUTION; }
Coverage                         { return COVERAGE; }
Unions                           { return UNIONS; }
GeneralFrontName                 { return GENERAL_FRONT_NAME; }
[[]                              { return CORCHETE_ABIERTO; }
[]]                              { return CORCHETE_CERRADO; }
[{]                              { return LLAVE_ABIERTA; }
[}]                              { return LLAVE_CERRADA; }
,                                { return COMA; }
:                                { return DOS_PUNTOS; }
->                               { return FLECHA; }
Preaction                        { BEGIN RESTO_LINEA; return PREACTION; }
[0-9][0-9]*                      { intValueMetric = atoi(yytext); return NUMBER; }
[a-zA-Z/0-9\\_][a-zA-Z0-9/.\*_]*  { stringValueMetric = yytext; return STRING; }
<RESTO_LINEA>[^: \t\n](.*)       { BEGIN 0; stringValueMetric = yytext; return STRING; }
<RESTO_LINEA>:                   { return DOS_PUNTOS;}
<RESTO_LINEA>[ \t\n]*            {}
%%

int yywrap(){ return 1; }
