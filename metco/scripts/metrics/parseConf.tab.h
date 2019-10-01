/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_CONF_PARSECONF_TAB_H_INCLUDED
# define YY_CONF_PARSECONF_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int Confdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TYPE_STOPPING_CRITERION = 258,
    VALUE_STOPPING_CRITERION = 259,
    EXECUTION_MODEL = 260,
    CORCHETE_ABIERTO = 261,
    CORCHETE_CERRADO = 262,
    INIT_PERCENT_OF_INDIVIDUALS = 263,
    MIGRATION_PROBABILITY = 264,
    NUMBER_OF_INDIVIDUALS_TO_MIGRATE = 265,
    NUMERO = 266,
    DOS_PUNTOS = 267,
    STRING = 268,
    MAX_GLOBAL_FRONT_SIZE = 269,
    MAX_LOCAL_FRONT_SIZE = 270,
    SHARING_GROUP = 271,
    MAX_FINAL_SOLUTION_SIZE = 272,
    INDIVIDUAL = 273,
    NEWLINE = 274,
    INDIVIDUALPARAMS = 275,
    ALGORITHM = 276,
    COMA = 277,
    SEND_RESULTS_PER_GENERATION = 278,
    CONVERSION = 279,
    CONVERSIONPARAMS = 280,
    SOLUTION_SOURCE = 281,
    SCORE = 282,
    SCOREPARAMS = 283,
    MIGRATION_SELECTOR = 284,
    EXCHANGE_SELECTOR = 285,
    MUTATION = 286,
    CROSSOVER = 287,
    OUTPUT_PATH = 288,
    PLUGIN_PATH = 289,
    MIGRATION_TOPOLOGY = 290,
    MIGRATION_TOPOLOGY_PARAMS = 291,
    INIT_POP_ISLAND_LOADER = 292,
    INIT_POP_ISLAND_LOADER_PARAMS = 293,
    LOCAL_SEARCH = 294,
    LOCAL_SEARCH_PARAMS = 295,
    MULTIOBJECTIVIZATION = 296,
    MULTIOBJECTIVIZATION_PARAMS = 297,
    NUMBER_PARALLEL_EXECUTIONS = 298
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE Conflval;

int Confparse (Configuration &configuration, int myId);

#endif /* !YY_CONF_PARSECONF_TAB_H_INCLUDED  */
