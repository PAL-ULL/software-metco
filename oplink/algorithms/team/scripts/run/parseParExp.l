%{
#include <string.h>
#include <string>

#include "parseParExp.tab.h"
#include "parseParDefinitions.h"

using namespace std;

%}
%x RESTO_LINEA

%%

[ \t\n]*                         {}
Execution_Model                  { return EXECUTION_MODEL; }
[[]                              { return CORCHETE_ABIERTO; }
[]]                              { return CORCHETE_CERRADO; }
[{]                              { return LLAVE_ABIERTA; }
[}]                              { return LLAVE_CERRADA; }
Repetitions                      { return REPETITIONS; }
Number_of_processors             { return NUMBER_OF_PROCESSORS; }
Name_of_experiment               { return NAME_OF_EXPERIMENT; }
Configurations                   { return CONFIGURATIONS; }
Init_percent_of_individuals      { return INIT_PERCENT_OF_INDIVIDUALS; }
Migration_probability            { return MIGRATION_PROBABILITY; }
Number_of_individuals_to_migrate { return NUMBER_OF_INDIVIDUALS_TO_MIGRATE; }
Max_global_front_size            { return MAX_GLOBAL_FRONT_SIZE; }
Max_final_solution_size          { return MAX_FINAL_SOLUTION_SIZE; }
Send_results_per_generation      { return SEND_RESULTS_PER_GENERATION; }
Sharing                          { return SHARING; }
Score                            { return SCORE; }
Number_parallel_executions			 { return NUMBER_PARALLEL_EXECUTIONS; }
Migration_Topology               { return MIGRATION_TOPOLOGY; }
Migration_TopologyParams         { BEGIN RESTO_LINEA; return MIGRATION_TOPOLOGY_PARAMS; }
Init_Pop_Island_Loader           { return INIT_POP_ISLAND_LOADER; }
Init_Pop_Island_LoaderParams     { BEGIN RESTO_LINEA; return INIT_POP_ISLAND_LOADER_PARAMS; }
Plugin_path                      { return PLUGIN_PATH; }
Conversion                       { BEGIN RESTO_LINEA; return CONVERSION; }
ConversionParams                 { BEGIN RESTO_LINEA; return CONVERSIONPARAMS; }
Execution_line                   { BEGIN RESTO_LINEA; return EXECUTION_LINE; }
Machinefile                      { BEGIN RESTO_LINEA; return MACHINEFILE; }
Preaction                        { BEGIN RESTO_LINEA; return PREACTION; }
[0-9]+\.[0-9]+                   { realValue = atof(yytext); return NUMERO; }
[0-9]+                           { realValue = (double) atoi(yytext); return NUMERO; }
:                                { return DOS_PUNTOS; }
\,                               { return COMA; }
->                               { return FLECHA; }
[a-zA-Z_/.][a-zA-Z0-9/_.]*       { stringValue = yytext; return STRING; }
<RESTO_LINEA>[^: \t\n](.*)       { BEGIN 0; stringValue = yytext; return STRING; }
<RESTO_LINEA>:                   { return DOS_PUNTOS;}
<RESTO_LINEA>[ \t\n]*            {}
%%

int yywrap(){ return 1; }
