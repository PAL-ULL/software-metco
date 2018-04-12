/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 *   - Eduardo Manuel Segredo González
 * 
 * FECHA
 *    Noviembre 2007
 * 
 * DESCRIPCION
 *    Analizador léxico del fichero de configuración para las ejecuciones
 *    paralelas
 *
 ***********************************************************************************/


%{
#include <string.h>
#include <string>
#include <iostream>

using namespace std;

#include "Configuration.h"
#include "parseConf.tab.h"

double realValueConf;
string stringValueConf;

%}

%x DEV_NEWLINES 
%x LINEA_COMPLETA
%%

<DEV_NEWLINES,INITIAL>[ \t]*                  {}
<INITIAL>\n                                   {}
<DEV_NEWLINES>\n                              { BEGIN 0; return NEWLINE; }
<DEV_NEWLINES,INITIAL>[[]                     { return CORCHETE_ABIERTO; }
<DEV_NEWLINES,INITIAL>[]]                     { BEGIN 0; return CORCHETE_CERRADO; }
<DEV_NEWLINES,INITIAL>,                       { return COMA; }
Type_Stopping_Criterion                       { return TYPE_STOPPING_CRITERION; }
Value_stopping_criterion                      { return VALUE_STOPPING_CRITERION; }
Solution_source                               { return SOLUTION_SOURCE; }
Execution_Model                               { return EXECUTION_MODEL; }
Number_parallel_executions										{ return NUMBER_PARALLEL_EXECUTIONS; }
Conversion                                    { return CONVERSION; }
Score                                         { return SCORE; }
Migration_Topology                            { return MIGRATION_TOPOLOGY; }
Init_Pop_Island_Loader                        { return INIT_POP_ISLAND_LOADER; }
Plugin_path                                   { return PLUGIN_PATH; }
Output_path                                   { return OUTPUT_PATH; }
ConversionParams                              { BEGIN LINEA_COMPLETA; return CONVERSIONPARAMS; }
ScoreParams                                   { BEGIN LINEA_COMPLETA; return SCOREPARAMS; }
Migration_TopologyParams                      { BEGIN LINEA_COMPLETA; return MIGRATION_TOPOLOGY_PARAMS; }
Init_Pop_Island_LoaderParams                  { BEGIN LINEA_COMPLETA; return INIT_POP_ISLAND_LOADER_PARAMS; }
Init_percent_of_individuals                   { return INIT_PERCENT_OF_INDIVIDUALS; }
Migration_probability                         { return MIGRATION_PROBABILITY; }
Number_of_individuals_to_migrate              { return NUMBER_OF_INDIVIDUALS_TO_MIGRATE; }
Max_Global_Front_Size                         { return MAX_GLOBAL_FRONT_SIZE; }
Max_Local_Front_Size                          { return MAX_LOCAL_FRONT_SIZE; }
Sharing_Group                                 { return SHARING_GROUP; }
Max_Final_Solution_Size                       { return MAX_FINAL_SOLUTION_SIZE; }
Send_results_per_generation                   { return SEND_RESULTS_PER_GENERATION; }
Algorithm                                     { return ALGORITHM; }
Individual                                    { return INDIVIDUAL; }
Migration_Selector                            { return MIGRATION_SELECTOR; }
Exchange_Selector                             { return EXCHANGE_SELECTOR; }
Mutation                                      { return MUTATION; }
Crossover                                     { return CROSSOVER; }
Local_Search                                  { return LOCAL_SEARCH; }
Local_SearchParams                            { BEGIN LINEA_COMPLETA; return LOCAL_SEARCH_PARAMS; }
Multiobjectivization													{ return MULTIOBJECTIVIZATION; }
MultiobjectivizationParams										{ BEGIN LINEA_COMPLETA; return MULTIOBJECTIVIZATION_PARAMS; }
IndividualParams                              { BEGIN LINEA_COMPLETA; return INDIVIDUALPARAMS; }
<DEV_NEWLINES,INITIAL>-?[0-9]+\.[0-9]+        { BEGIN DEV_NEWLINES; realValueConf = atof(yytext); return NUMERO; }
<DEV_NEWLINES,INITIAL>-?[0-9]+                { BEGIN DEV_NEWLINES; realValueConf = (double) atoi(yytext); return NUMERO; }
:                                             { return DOS_PUNTOS; }
<DEV_NEWLINES,INITIAL>[a-zA-Z/_.][a-zA-Z0-9/_.]*  { BEGIN DEV_NEWLINES; stringValueConf = yytext; return STRING; }
<LINEA_COMPLETA>[^: \t\n](.*)                 { BEGIN 0; stringValueConf = yytext; return STRING; }
<LINEA_COMPLETA>:                             { return DOS_PUNTOS;}
<LINEA_COMPLETA>[ \t\n]*                      {}

%%

int yywrap(){ return 1; }
