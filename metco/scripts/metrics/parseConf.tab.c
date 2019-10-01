/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         Confparse
#define yylex           Conflex
#define yyerror         Conferror
#define yydebug         Confdebug
#define yynerrs         Confnerrs

#define yylval          Conflval
#define yychar          Confchar

/* Copy the first part of user declarations.  */
#line 18 "../../src/skeleton/parseConf.y" /* yacc.c:339  */

	#include <strings.h>
	#include <string>
	#include <sstream>
	#include "Conversion.h"
	#include "ScoreAlgorithm.h"
	#include "MigrationTopology.h"
	#include "InitPopIslandLoader.h"
	#include "MigrationSelector.h"
	#include "ExchangeSelector.h"
	#include "Individual.h"
	#include "Configuration.h"
	#include "EA.h"
	#include "utils.h"
	using namespace std;

	// Inicializamos los parámetros de configuración a valores no válidos, para detectar
	// errores en el fichero de configuración
	extern double realValueConf;
	extern string stringValueConf;
	void Conferror(Configuration &conf, int myId, char *);
	int Conflex();
	vector<double> realBufferConf; 
	vector<string> stringBufferConf;
	vector<TAlgorithm> teamAlgorithms;
	vector<TExecutionModel> executionModel;
	TExecutionModel nextModel;
	bool realExecutionModel = false;
	double initPercentOfIndividuals = -1;
	double migrationProbability = -1;
	int numberOfIndividualsToMigrate = -1;
	int maxGlobalFrontSize = -1;
	int maxFinalSolutionSize = -1;
	int parSize = -1;
	int sendResultsPerGeneration = -1;
	int numberParallelExecutions = -1;
	bool insertNextModel(const Configuration &configuration);
	vector<string> newConfig;
	vector<Individual*> allIndividuals;

//Deteccion de redefiniciones de parámetros
#define setCheckedValue(old, newValue, error, configuration)\
	if (old != -1){\
		configuration.setErrorParsingStr(string(error) + " redefined");\
	}\
	if (newValue == -1){\
		configuration.setErrorParsingStr(string(error) + ": not recognized value");\
	}\
	old = newValue;\

//Deteccion de parámetros obligatorios no definidos
#define checkDefined(value, error, configuration)\
	if (value == -1){\
		configuration.setErrorParsingStr(string(error) + " non-defined");\
	}\

	void reset();


#line 134 "parseConf.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parseConf.tab.h".  */
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

/* Copy the second part of user declarations.  */

#line 229 "parseConf.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   162

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  14
/* YYNRULES -- Number of rules.  */
#define YYNRULES  55
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  165

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   298

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    85,    85,    85,   316,   317,   322,   323,   328,   329,
     337,   345,   353,   361,   369,   377,   388,   399,   410,   421,
     422,   423,   424,   425,   426,   437,   444,   443,   455,   463,
     462,   474,   484,   489,   499,   500,   505,   509,   510,   514,
     521,   528,   535,   542,   549,   556,   563,   570,   577,   584,
     591,   599,   607,   608,   609,   610
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TYPE_STOPPING_CRITERION",
  "VALUE_STOPPING_CRITERION", "EXECUTION_MODEL", "CORCHETE_ABIERTO",
  "CORCHETE_CERRADO", "INIT_PERCENT_OF_INDIVIDUALS",
  "MIGRATION_PROBABILITY", "NUMBER_OF_INDIVIDUALS_TO_MIGRATE", "NUMERO",
  "DOS_PUNTOS", "STRING", "MAX_GLOBAL_FRONT_SIZE", "MAX_LOCAL_FRONT_SIZE",
  "SHARING_GROUP", "MAX_FINAL_SOLUTION_SIZE", "INDIVIDUAL", "NEWLINE",
  "INDIVIDUALPARAMS", "ALGORITHM", "COMA", "SEND_RESULTS_PER_GENERATION",
  "CONVERSION", "CONVERSIONPARAMS", "SOLUTION_SOURCE", "SCORE",
  "SCOREPARAMS", "MIGRATION_SELECTOR", "EXCHANGE_SELECTOR", "MUTATION",
  "CROSSOVER", "OUTPUT_PATH", "PLUGIN_PATH", "MIGRATION_TOPOLOGY",
  "MIGRATION_TOPOLOGY_PARAMS", "INIT_POP_ISLAND_LOADER",
  "INIT_POP_ISLAND_LOADER_PARAMS", "LOCAL_SEARCH", "LOCAL_SEARCH_PARAMS",
  "MULTIOBJECTIVIZATION", "MULTIOBJECTIVIZATION_PARAMS",
  "NUMBER_PARALLEL_EXECUTIONS", "$accept", "confFile", "$@1",
  "generalSection", "algorithmSection", "lineaGeneralSection",
  "model_execution_list", "$@2", "$@3", "defAlg", "HeaderConfiguration",
  "RestConfiguration", "RestConfigurationLine", "set_string_numeros", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298
};
# endif

#define YYPACT_NINF -13

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-13)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -13,     2,   -13,   -13,    35,    -8,    -6,    -5,    -1,     7,
       9,    12,    13,    18,    19,    20,    21,    22,    23,    29,
      30,    34,    36,    38,   -12,   -13,   -13,    -3,    41,    40,
      42,    43,    44,    46,    48,    51,    52,    53,    54,    61,
      62,    63,    64,    66,    67,    68,    71,   -13,    72,    73,
     -13,   -13,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    -3,   -13,     3,    70,    93,
      87,    88,    89,    90,    91,    92,    94,    95,   -13,    96,
     -13,    97,    98,    99,   -13,   100,   -13,   101,   108,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   -13,   -13,   -13,   -13,    -4,   125,   -13,
     -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,
     -13,   -13,   -13,   106,   107,   126,   127,   128,   -13,   129,
     130,   131,   132,   133,   134,   -13,   135,   -13,   -13,   144,
      -2,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,
     -13,   -13,   -13,   136,   -13,   145,   -13,   137,   147,   -13,
     105,   148,   -13,   150,   -13
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     5,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     3,     4,     7,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     6,     0,     0,
      55,    54,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    32,    35,     0,     0,     8,
       0,     0,     0,     0,     0,     0,     0,     0,    15,     0,
      16,     0,     0,     0,    17,     0,    18,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    34,    53,    52,    51,     0,     0,    19,
      20,    21,    22,    23,    33,    24,     9,    10,    13,    14,
      11,    12,    25,     0,     0,     0,     0,     0,    40,     0,
       0,     0,     0,     0,     0,    47,     0,    49,    31,     0,
       0,    36,    37,    41,    42,    39,    38,    43,    50,    44,
      45,    46,    48,     0,    28,     0,    29,     0,     0,    26,
       0,     0,    30,     0,    27
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   138,
     -13,   -13,    37,   -13
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     4,    24,    25,    69,   161,   158,    26,
      27,    65,    66,    67
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      48,    49,     3,   138,    28,   154,    29,    30,    50,    11,
      51,    31,    52,    53,   104,    54,   105,    55,   139,    32,
     155,    33,   106,    56,    34,    35,    57,    58,    59,    60,
      36,    37,    38,    39,    40,    41,    61,    62,    63,    64,
       5,    42,    43,     6,     7,     8,    44,    68,    45,     9,
      46,    70,    10,    71,    72,    73,    11,    74,    12,    13,
      14,    75,    15,    16,    76,    77,    78,    79,    17,    18,
      19,    20,    21,    22,    80,    81,    82,    83,    23,    84,
      85,    86,    87,   107,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   108,
       0,     0,   103,     0,     0,     0,   109,   110,   111,   112,
     113,   114,   162,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   141,   142,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   140,     0,
       0,     0,     0,     0,     0,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   157,   164,   156,   159,
     160,   163,    47
};

static const yytype_int8 yycheck[] =
{
       3,     4,     0,     7,    12,     7,    12,    12,    11,    21,
      13,    12,    15,    16,    11,    18,    13,    20,    22,    12,
      22,    12,    19,    26,    12,    12,    29,    30,    31,    32,
      12,    12,    12,    12,    12,    12,    39,    40,    41,    42,
       5,    12,    12,     8,     9,    10,    12,     6,    12,    14,
      12,    11,    17,    11,    11,    11,    21,    11,    23,    24,
      25,    13,    27,    28,    13,    13,    13,    13,    33,    34,
      35,    36,    37,    38,    13,    13,    13,    13,    43,    13,
      13,    13,    11,    13,    12,    12,    12,    12,    12,    12,
      12,    12,    12,    12,    12,    12,    12,    12,    12,     6,
      -1,    -1,    65,    -1,    -1,    -1,    19,    19,    19,    19,
      19,    19,     7,    19,    19,    19,    19,    19,    19,    19,
      19,    13,    11,    11,    11,    19,    19,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,    13,    -1,
      -1,    -1,    -1,    -1,    -1,    19,    19,    19,    19,    19,
      19,    19,    19,    19,    19,    11,    11,     7,    22,    22,
      13,    13,    24
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    45,    46,     0,    47,     5,     8,     9,    10,    14,
      17,    21,    23,    24,    25,    27,    28,    33,    34,    35,
      36,    37,    38,    43,    48,    49,    53,    54,    12,    12,
      12,    12,    12,    12,    12,    12,    12,    12,    12,    12,
      12,    12,    12,    12,    12,    12,    12,    53,     3,     4,
      11,    13,    15,    16,    18,    20,    26,    29,    30,    31,
      32,    39,    40,    41,    42,    55,    56,    57,     6,    50,
      11,    11,    11,    11,    11,    13,    13,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    11,    12,    12,
      12,    12,    12,    12,    12,    12,    12,    12,    12,    12,
      12,    12,    12,    56,    11,    13,    19,    13,     6,    19,
      19,    19,    19,    19,    19,    19,    19,    19,    19,    19,
      19,    19,    19,    13,    11,    11,    11,    13,    13,    13,
      13,    13,    13,    13,    13,    13,    13,    13,     7,    22,
      13,    19,    19,    19,    19,    19,    19,    19,    19,    19,
      19,    19,    19,    11,     7,    22,    22,    11,    52,    22,
      13,    51,     7,    13,     7
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    44,    46,    45,    47,    47,    48,    48,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    51,    50,    50,    52,
      50,    50,    53,    54,    55,    55,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    57,    57,    57,    57
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     3,     2,     0,     2,     1,     3,     4,
       4,     4,     4,     4,     4,     3,     3,     3,     3,     4,
       4,     4,     4,     4,     4,     4,     0,     9,     4,     0,
       8,     3,     2,     4,     2,     1,     4,     4,     4,     4,
       3,     4,     4,     4,     4,     4,     4,     3,     4,     3,
       4,     2,     2,     2,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (configuration, myId, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, configuration, myId); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, Configuration &configuration, int myId)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (configuration);
  YYUSE (myId);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, Configuration &configuration, int myId)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, configuration, myId);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, Configuration &configuration, int myId)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , configuration, myId);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, configuration, myId); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, Configuration &configuration, int myId)
{
  YYUSE (yyvaluep);
  YYUSE (configuration);
  YYUSE (myId);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (Configuration &configuration, int myId)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 85 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { reset(); }
#line 1418 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 87 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (!realExecutionModel){
				configuration.setErrorParsingStr("Execution Model non-defined");
			}
			// Chequear si todo esta definido
			checkDefined(initPercentOfIndividuals, "Init_Percent_Of_Individuals", configuration);
			checkDefined(migrationProbability, "Migration_probability", configuration);
			checkDefined(numberOfIndividualsToMigrate, "Number_Of_Individuals_To_Migrate", configuration);
			checkDefined(maxGlobalFrontSize, "Max_front_size", configuration);
			checkDefined(maxFinalSolutionSize, "Max_solution_size", configuration);
			checkDefined(sendResultsPerGeneration, "Send_results_per_generation", configuration);
			checkDefined(numberParallelExecutions, "Number_parallel_executions", configuration);

			// Chequear que al menos hay un algoritmo
			if (teamAlgorithms.size() == 0){
				configuration.setErrorParsingStr("0 configurations defined");
			}
			
			vector <pair <string, string> > algIndPairs;
			// Para cada configuración chequear que todo está bien definido y que se pueden
			// cargar los individuos y EAs
			for (unsigned int i = 0; i < teamAlgorithms.size(); i++){
				checkDefined(teamAlgorithms[i].critStop, "Type_Stopping_Criterion (Configuration)", configuration);
				checkDefined(teamAlgorithms[i].valueSC, "Value_Stopping_Criterion (Configuration)", configuration);
				checkDefined(teamAlgorithms[i].solutionSource, "Solution Source (Configuration)", configuration);
				checkDefined(teamAlgorithms[i].sharingGroup, "Sharing_Group (Configuration)", configuration);
				if (teamAlgorithms[i].individualName.size() == 0){
					configuration.setErrorParsingStr("Individual (Configuration) non-defined");
					YYABORT;
				}
				if (teamAlgorithms[i].migrationSelectorName.size() == 0){
					configuration.setErrorParsingStr("Migration Selector non-defined");
					YYABORT;
				}

				if (teamAlgorithms[i].exchangeSelectorName.size() == 0){
					configuration.setErrorParsingStr("Exchange Selector non-defined");
					YYABORT;
				}

				if (teamAlgorithms[i].mutationName.size() == 0){
					configuration.setErrorParsingStr("Mutation non-defined");
					YYABORT;
				}

				if (teamAlgorithms[i].crossoverName.size() == 0){
					configuration.setErrorParsingStr("Crossover non-defined");
					YYABORT;
				}

				if (teamAlgorithms[i].localSearchName.size() == 0){
					configuration.setErrorParsingStr("Local Search non-defined");
					YYABORT;
				}

				if (teamAlgorithms[i].configs.size() == 0){
					configuration.setErrorParsingStr("Algoritm without any configuration");
					YYABORT;
				}
				for (unsigned int j = 0; j < teamAlgorithms[i].configs.size(); j++){
					if (teamAlgorithms[i].configs[j].size() != teamAlgorithms[i].configs[0].size()){
						configuration.setErrorParsingStr("Not all configuration in the same algorithms have the same number of parameters");
						YYABORT;
					}
				}

				// Carga del operador de mutacion
				if (strcasecmp(teamAlgorithms[i].mutationName.c_str(), "individual_internal") != 0){
					vector<string> empty;
					Mutation *mutation = getMutation(configuration.getPluginPath(), teamAlgorithms[i].mutationName, empty, (myId == 0));
					if (mutation == NULL){
						configuration.setErrorParsingStr("Loading mutation operator");
						YYABORT;
					}
					teamAlgorithms[i].mutation = mutation;
				} else {
					teamAlgorithms[i].mutation = NULL;
				}

				// Carga del operador de crossover
				if (strcasecmp(teamAlgorithms[i].crossoverName.c_str(), "individual_internal") != 0){
					vector<string> empty;
					Crossover *crossover = getCrossover(configuration.getPluginPath(), teamAlgorithms[i].crossoverName, empty, (myId == 0));
					if (crossover == NULL){
						configuration.setErrorParsingStr("Loading Crossover operator");
						YYABORT;
					}
					teamAlgorithms[i].crossover = crossover;
				} else {
					teamAlgorithms[i].crossover = NULL;
				}

				// Carga del operador de busqueda local
				LocalSearch *ls = getLocalSearch(configuration.getPluginPath(), teamAlgorithms[i].localSearchName, teamAlgorithms[i].localSearchParams, (myId == 0));
				if (ls == NULL){
					configuration.setErrorParsingStr("LocalSearch module could not be loaded");
				} else {
					teamAlgorithms[i].localSearch = ls;
				}
				
        // Carga del plugin con la multiobjetivizacion
        if (teamAlgorithms[i].multiObjectivizationName.size() != 0) {
				  MultiObjectivization *mo = getMultiObjectivization(configuration.getPluginPath(), teamAlgorithms[i].multiObjectivizationName, teamAlgorithms[i].multiObjectivizationParams, (myId == 0));
				  if (mo == NULL){
            teamAlgorithms[i].multiObjectivization = NULL;
					  configuration.setErrorParsingStr("MultiObjectivization module could not be loaded");
				  } else {
					  teamAlgorithms[i].multiObjectivization = mo;
				  }
        }
        else {
          teamAlgorithms[i].multiObjectivization = NULL;
        }

				//Carga del individuo
				vector<string> params = splitString(teamAlgorithms[i].individualParams);
				void *hndl;
				Individual *ind = getInd(configuration.getPluginPath(), teamAlgorithms[i].individualName, params, (myId == 0));
				if (ind == NULL){
					configuration.setErrorParsingStr("Loading individual");
					YYABORT;
				}

				teamAlgorithms[i].ind = ind;

        if (teamAlgorithms[i].multiObjectivization != NULL) {
          teamAlgorithms[i].multiObjectivization->initiallization(teamAlgorithms[i].ind);
          teamAlgorithms[i].ind->setNumberOfObj(teamAlgorithms[i].ind->getNumberOfObj() + 1);
        }
				teamAlgorithms[i].ind->setMutOperator(teamAlgorithms[i].mutation);
				teamAlgorithms[i].ind->setCrossOperator(teamAlgorithms[i].crossover);
        if (teamAlgorithms[i].multiObjectivization != NULL) {
          teamAlgorithms[i].ind->setMultiObjectivizationPlugins(vector<MultiObjectivization *>(1, teamAlgorithms[i].multiObjectivization));
        }
				allIndividuals.push_back(teamAlgorithms[i].ind);

				if (strcasecmp(teamAlgorithms[i].migrationSelectorName.c_str(), "algorithm_internal") != 0){
					vector<string> empty;
					MigrationSelector *migSelector = getMigrationSelector(configuration.getPluginPath(), teamAlgorithms[i].migrationSelectorName, empty, (myId == 0));
					if (migSelector == NULL){
						configuration.setErrorParsingStr("Loading migration Selector");
						YYABORT;
					}
					teamAlgorithms[i].migrationSelector = migSelector;
				} else {
					teamAlgorithms[i].migrationSelector = NULL;
				}


				if (strcasecmp(teamAlgorithms[i].exchangeSelectorName.c_str(), "algorithm_internal") != 0){
					vector<string> empty;
					ExchangeSelector *migSelector = getExchangeSelector(configuration.getPluginPath(), teamAlgorithms[i].exchangeSelectorName, empty, (myId == 0));
					if (migSelector == NULL){
						configuration.setErrorParsingStr("Loading exchange Selector");
						YYABORT;
					}
					teamAlgorithms[i].exchangeSelector = migSelector;
				} else {
					teamAlgorithms[i].exchangeSelector = NULL;
				}

				algIndPairs.push_back(pair<string, string>(teamAlgorithms[i].algorithmStr, teamAlgorithms[i].individualName));
			}

			// Chequear que se fijo un nombre para el módulo de conversión y se puede cargar
			if (configuration.getConversionStr().size() == 0){
				configuration.setErrorParsingStr("conversion module not specified");
			} else {
				Conversion *conv = getConversion(configuration.getBaseDirectory() + "/" + configuration.getConversionStr(), configuration.getConversionParams(), algIndPairs, (myId == 0));
				if (conv == NULL){
					configuration.setErrorParsingStr("Conversion module could not be loaded");
				} else{
					conv->setExampleIndividuals(allIndividuals);
					configuration.setConversion(conv);
				}
			}

			// Chequear que se fijo el módulo de Score y se puede cargar
			if (configuration.getScoreStr().size() == 0){
				configuration.setErrorParsingStr("Score module not specified");
			} else {
				ScoreAlgorithm *score = getScoreAlgorithm(configuration.getPluginPath(), configuration.getScoreStr(), configuration.getScoreParams(), (myId == 0));
				if (score == NULL){
					configuration.setErrorParsingStr("Score module could not be loaded");
				} else {
					configuration.setScore(score);
				}
			}

			// Chequear que se fijo el modelo de MigrationTopology y se puede cargar
			if (configuration.getMigrationTopologyStr().size() == 0){
				configuration.setErrorParsingStr("MigrationTopology module not specified");
			} else {
				MigrationTopology *migTop = getMigrationTopology(configuration.getPluginPath(), configuration.getMigrationTopologyStr(), configuration.getMigrationTopologyParams(), (myId == 0));
				if (migTop == NULL){
					configuration.setErrorParsingStr("MigrationTopology module could not be loaded");
				} else {
					configuration.setMigrationTopology(migTop);
				}
			}

			// Chequear que se fijo el initPopIslandLoader 
			if (configuration.getInitPopIslandLoaderStr().size() == 0){
				configuration.setErrorParsingStr("InitPopIslandLoader module not specified");
			} else {
				InitPopIslandLoader *ipil = getInitPopIslandLoader(configuration.getPluginPath(), configuration.getInitPopIslandLoaderStr(), configuration.getInitPopIslandLoaderParams(), (myId == 0));
				if (ipil == NULL){
					configuration.setErrorParsingStr("InitPopIslandLoader module could not be loaded");
				} else {
					configuration.setInitPopIslandLoader(ipil);
				}
			}

			// Guardar los parámetros "globales"
			configuration.setExecutionModel(executionModel);
			configuration.setInitPercentOfIndividuals(initPercentOfIndividuals);
			configuration.setMigrationProbability(migrationProbability); 
			configuration.setNumberOfIndividualsToMigrate(numberOfIndividualsToMigrate);
			configuration.setMaxGlobalFrontSize(maxGlobalFrontSize);
			configuration.setMaxFinalSolutionSize(maxFinalSolutionSize);
			configuration.setSendResultsPerGeneration(sendResultsPerGeneration == 1);
			configuration.setTeamAlgorithms(teamAlgorithms);
			configuration.setNumberParallelExecutions(numberParallelExecutions);
		}
#line 1647 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 330 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (configuration.getConversionStr().size() != 0){
				configuration.setErrorParsingStr("Conversion redefined");
			} else {
				configuration.setConversionStr(stringValueConf);
			}
		}
#line 1659 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 338 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (configuration.getScoreStr().size() != 0){
				configuration.setErrorParsingStr("Score redefined");
			} else {
				configuration.setScoreStr(stringValueConf);
			}
		}
#line 1671 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 346 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (configuration.getMigrationTopologyStr().size() != 0){
				configuration.setErrorParsingStr("Migration_Topology redefined");
			} else {
				configuration.setMigrationTopologyStr(stringValueConf);
			}
		}
#line 1683 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 354 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (configuration.getInitPopIslandLoaderStr().size() != 0){
				configuration.setErrorParsingStr("InitPopIslandLoader redefined");
			} else {
				configuration.setInitPopIslandLoaderStr(stringValueConf);
			}
		}
#line 1695 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 362 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (configuration.getOutputPath().size() != 0){
				configuration.setErrorParsingStr("Output_path redefined");
			} else {
				configuration.setOutputPath(stringValueConf);
			}
		}
#line 1707 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 370 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (configuration.getPluginPath().size() != 0){
				configuration.setErrorParsingStr("Plugin_path redefined");
			} else {
				configuration.setPluginPath(stringValueConf);
			}
		}
#line 1719 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 378 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (configuration.getConversionParams().size() != 0){
				configuration.setErrorParsingStr("Conversion params redefined");
			} else {
				configuration.setConversionParams(splitString(stringValueConf));
				if (configuration.getConversionParams().size() == 0){
					configuration.setErrorParsingStr("Conversion params bad syntax");
				}
			}
		}
#line 1734 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 389 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (configuration.getScoreParams().size() != 0){
				configuration.setErrorParsingStr("Score params redefined");
			} else {
				configuration.setScoreParams(splitString(stringValueConf));
				if (configuration.getScoreParams().size() == 0){
					configuration.setErrorParsingStr("Score params bad syntax");
				}
			}
		}
#line 1749 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 400 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (configuration.getMigrationTopologyParams().size() != 0){
				configuration.setErrorParsingStr("Migration_Topology params redefined");
			} else {
				configuration.setMigrationTopologyParams(splitString(stringValueConf));
				if (configuration.getMigrationTopologyParams().size() == 0){
					configuration.setErrorParsingStr("Migration_Topology params bad syntax");
				}
			}
		}
#line 1764 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 411 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (configuration.getInitPopIslandLoaderParams().size() != 0){
				configuration.setErrorParsingStr("Init_Pop_Island_Loader params redefined");
			} else {
				configuration.setInitPopIslandLoaderParams(splitString(stringValueConf));
				if (configuration.getInitPopIslandLoaderParams().size() == 0){
					configuration.setErrorParsingStr("Init_Pop_Island_Loader params bad syntax");
				}
			}
		}
#line 1779 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 421 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { setCheckedValue(initPercentOfIndividuals, (realValueConf/100), "Init_Percent_Of_Individuals", configuration);  }
#line 1785 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 422 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { setCheckedValue(migrationProbability, realValueConf, "Migration_probability", configuration); }
#line 1791 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 423 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { setCheckedValue(numberOfIndividualsToMigrate, (int)realValueConf, "Number_of_individuals_to_migrate", configuration);  }
#line 1797 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 424 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { setCheckedValue(maxGlobalFrontSize, (int)realValueConf, "Max_front_size", configuration); }
#line 1803 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 425 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { setCheckedValue(maxFinalSolutionSize, (int)realValueConf, "Solution_size", configuration); }
#line 1809 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 427 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { 
				if (strcasecmp("yes", stringValueConf.c_str()) == 0){
					setCheckedValue(sendResultsPerGeneration, 1, "Send_results_per_generation", configuration); 
				} 
				else if (strcasecmp("no", stringValueConf.c_str()) == 0){
					setCheckedValue(sendResultsPerGeneration, 0, "Send_results_per_generation", configuration);
				} else {
					configuration.setErrorParsingStr("Send_results_per_generation: not recognized value");
				}
			}
#line 1824 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 437 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { setCheckedValue(numberParallelExecutions, (int)realValueConf, "Number_parallel_executions", configuration); }
#line 1830 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 444 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			nextModel.stopCrit = Configuration::getGlobalTypeStoppingCriterion(stringValueConf); 
			nextModel.valueStopCrit = realValueConf; 
		}
#line 1839 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 449 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { 
			nextModel.typeSelection = Configuration::getTypeSelection(stringValueConf);
			if (!insertNextModel(configuration)){
				configuration.setErrorParsingStr("Execution model incorrect");
			}
		}
#line 1850 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 456 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { 
				nextModel.stopCrit = Configuration::getGlobalTypeStoppingCriterion(stringValueConf);
				if (!insertNextModel(configuration)){
					configuration.setErrorParsingStr("Execution model incorrect");
				}
			}
#line 1861 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 463 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { 
				nextModel.stopCrit = Configuration::getGlobalTypeStoppingCriterion(stringValueConf); 
				nextModel.valueStopCrit = realValueConf; 
			}
#line 1870 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 468 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { 
				nextModel.typeSelection = Configuration::getTypeSelection(stringValueConf); 
				if (!insertNextModel(configuration)) { 
					configuration.setErrorParsingStr("Execution model incorrect");
				} 
			}
#line 1881 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 475 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { 
				nextModel.stopCrit = Configuration::getGlobalTypeStoppingCriterion(stringValueConf);
				if (!insertNextModel(configuration)){
					configuration.setErrorParsingStr("Execution model incorrect");
				}
			}
#line 1892 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 490 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { 
			TAlgorithm nuevo; 
			nuevo.algorithmStr = stringValueConf;
			teamAlgorithms.push_back(nuevo);
		}
#line 1902 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 506 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { 
			setCheckedValue(teamAlgorithms[teamAlgorithms.size() - 1].critStop, EA::getTypeStoppingCriterion(stringValueConf), "Type_Stopping_Criterion (Algorithm)", configuration); 
		}
#line 1910 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 509 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { setCheckedValue(teamAlgorithms[teamAlgorithms.size() - 1].valueSC, realValueConf, "Value_Stopping_Criterion (Algorithm)", configuration); }
#line 1916 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 511 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			setCheckedValue(teamAlgorithms[teamAlgorithms.size() -1].solutionSource, Configuration::getSolutionSource(stringValueConf), "Solution_Source (Algorithm)", configuration); 
		}
#line 1924 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 515 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
				if (teamAlgorithms[teamAlgorithms.size()-1].individualName.length() != 0){
					configuration.setErrorParsingStr("Individual redefinition");
				}
				teamAlgorithms[teamAlgorithms.size()-1].individualName = stringValueConf;
			}
#line 1935 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 522 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
				if (teamAlgorithms[teamAlgorithms.size()-1].individualParams.length() != 0){
					configuration.setErrorParsingStr("IndividualParams redefinition");
				}
				teamAlgorithms[teamAlgorithms.size()-1].individualParams = stringValueConf;
			}
#line 1946 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 529 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
				if (teamAlgorithms[teamAlgorithms.size()-1].maxLocalFrontSize != 0){
					configuration.setErrorParsingStr("MaxLocalFrontSize redefined");
				}
				teamAlgorithms[teamAlgorithms.size()-1].maxLocalFrontSize = (int)realValueConf;
			}
#line 1957 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 536 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
				if (teamAlgorithms[teamAlgorithms.size()-1].sharingGroup != -1){
					configuration.setErrorParsingStr("Sharing_Group redefined");
				}
				teamAlgorithms[teamAlgorithms.size()-1].sharingGroup = (int)realValueConf;
			}
#line 1968 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 543 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (teamAlgorithms[teamAlgorithms.size()-1].migrationSelectorName.size() != 0){
				configuration.setErrorParsingStr("Migration_Selector redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].migrationSelectorName = stringValueConf;
		}
#line 1979 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 550 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (teamAlgorithms[teamAlgorithms.size()-1].mutationName.size() != 0){
				configuration.setErrorParsingStr("Mutation redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].mutationName = stringValueConf;
		}
#line 1990 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 557 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (teamAlgorithms[teamAlgorithms.size()-1].crossoverName.size() != 0){
				configuration.setErrorParsingStr("Crossover redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].crossoverName = stringValueConf;
		}
#line 2001 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 564 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (teamAlgorithms[teamAlgorithms.size()-1].localSearchName.size() != 0){
				configuration.setErrorParsingStr("Local Search redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].localSearchName = stringValueConf;
		}
#line 2012 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 571 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
				if (teamAlgorithms[teamAlgorithms.size()-1].localSearchParams.size() != 0){
					configuration.setErrorParsingStr("Local_SearchParams redefined");
				}
				teamAlgorithms[teamAlgorithms.size()-1].localSearchParams = splitString(stringValueConf);
			}
#line 2023 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 578 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (teamAlgorithms[teamAlgorithms.size()-1].multiObjectivizationName.size() != 0){
				configuration.setErrorParsingStr("MultiObjectivization redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].multiObjectivizationName = stringValueConf;
		}
#line 2034 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 585 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
				if (teamAlgorithms[teamAlgorithms.size()-1].multiObjectivizationParams.size() != 0){
					configuration.setErrorParsingStr("MultiObjectivizationParams redefined");
				}
				teamAlgorithms[teamAlgorithms.size()-1].multiObjectivizationParams = splitString(stringValueConf);
			}
#line 2045 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 592 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
			if (teamAlgorithms[teamAlgorithms.size()-1].exchangeSelectorName.size() != 0){
				configuration.setErrorParsingStr("Exchange_Selector redefined");
			}
			teamAlgorithms[teamAlgorithms.size()-1].exchangeSelectorName = stringValueConf;
		}
#line 2056 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 600 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    {
				teamAlgorithms[teamAlgorithms.size()-1].configs.push_back(stringBufferConf);
				stringBufferConf.clear();
			}
#line 2065 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 607 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { stringBufferConf.push_back(stringValueConf); }
#line 2071 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 608 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { stringstream tmp; tmp << realValueConf; stringBufferConf.push_back(tmp.str()); }
#line 2077 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 609 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { stringBufferConf.push_back(stringValueConf); }
#line 2083 "parseConf.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 610 "../../src/skeleton/parseConf.y" /* yacc.c:1646  */
    { stringstream tmp; tmp << realValueConf; stringBufferConf.push_back(tmp.str()); }
#line 2089 "parseConf.tab.c" /* yacc.c:1646  */
    break;


#line 2093 "parseConf.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (configuration, myId, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (configuration, myId, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, configuration, myId);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, configuration, myId);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (configuration, myId, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, configuration, myId);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, configuration, myId);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 618 "../../src/skeleton/parseConf.y" /* yacc.c:1906  */


void Conferror(Configuration &configuration, int myId, char *s) {
	configuration.setErrorParsingStr("Unrecognized format in Configuration File");
}

// Añadir modelo de ejecucion
bool insertNextModel(const Configuration &configuration){
	if (nextModel.stopCrit == Configuration::getGlobalTypeStoppingCriterion("CLEARSTATISTICS")){
 		executionModel.push_back(nextModel);
		return true;
	}
	if ((nextModel.stopCrit == -1) || (nextModel.typeSelection == -1) || (nextModel.valueStopCrit <= 0)){
		return false;
	}
	executionModel.push_back(nextModel);
	realExecutionModel = true;
	return true;
}

// Resetear a valores no válidos
void reset(){
	teamAlgorithms.clear();
	executionModel.clear();
	realExecutionModel = false;
	initPercentOfIndividuals = -1;
	migrationProbability = -1;
	numberOfIndividualsToMigrate = -1;
	maxGlobalFrontSize = -1;
	maxFinalSolutionSize = -1;
	parSize = -1;
}
