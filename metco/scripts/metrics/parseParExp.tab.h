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

#ifndef YY_EXPPAR_PARSEPAREXP_TAB_H_INCLUDED
# define YY_EXPPAR_PARSEPAREXP_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int expPardebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    REPETITIONS = 258,
    NUMBER_OF_PROCESSORS = 259,
    NAME_OF_EXPERIMENT = 260,
    CONFIGURATIONS = 261,
    INIT_PERCENT_OF_INDIVIDUALS = 262,
    MIGRATION_PROBABILITY = 263,
    NUMBER_OF_INDIVIDUALS_TO_MIGRATE = 264,
    NUMERO = 265,
    DOS_PUNTOS = 266,
    COMA = 267,
    STRING = 268,
    MAX_GLOBAL_FRONT_SIZE = 269,
    MAX_FINAL_SOLUTION_SIZE = 270,
    SEND_RESULTS_PER_GENERATION = 271,
    SHARING = 272,
    EXECUTION_LINE = 273,
    MACHINEFILE = 274,
    PREACTION = 275,
    CORCHETE_ABIERTO = 276,
    CORCHETE_CERRADO = 277,
    LLAVE_ABIERTA = 278,
    LLAVE_CERRADA = 279,
    EXECUTION_MODEL = 280,
    FLECHA = 281,
    CONVERSION = 282,
    CONVERSIONPARAMS = 283,
    SCORE = 284,
    INIT_POP_ISLAND_LOADER = 285,
    INIT_POP_ISLAND_LOADER_PARAMS = 286,
    MIGRATION_TOPOLOGY = 287,
    MIGRATION_TOPOLOGY_PARAMS = 288,
    PLUGIN_PATH = 289,
    NUMBER_PARALLEL_EXECUTIONS = 290
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE expParlval;

int expParparse (void);

#endif /* !YY_EXPPAR_PARSEPAREXP_TAB_H_INCLUDED  */
