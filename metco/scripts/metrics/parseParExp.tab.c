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
#define yyparse         expParparse
#define yylex           expParlex
#define yyerror         expParerror
#define yydebug         expPardebug
#define yynerrs         expParnerrs

#define yylval          expParlval
#define yychar          expParchar

/* Copy the first part of user declarations.  */
#line 3 "../run/parseParExp.y" /* yacc.c:339  */

	#include <vector>
	#include <string>
	#include <iostream>
	#include <sstream>
	#include <stdlib.h>
	#include <string.h>

	using namespace std;

	#include "parseParDefinitions.h"
	void expParerror(char *);
	int expParlex();
	vector<double> realBuffer; 
	vector<string> stringBuffer;
	stringstream tmpString;
	bool executionModelDefined;
	bool sendResultsPerGenerationFixed;
	bool sharingFixed;

	void comp_mueve(vector<double> &destino, vector<double> &origen, const string &error);
	void comp_mueve(vector<int> &destino, vector<double> &origen, const string &error);
	void comp_mueve(vector<string> &destino, vector<string> &origen, const string &error);
	void reset();

#line 100 "parseParExp.tab.c" /* yacc.c:339  */

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
   by #include "parseParExp.tab.h".  */
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

/* Copy the second part of user declarations.  */

#line 187 "parseParExp.tab.c" /* yacc.c:358  */

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
#define YYLAST   144

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  36
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  61
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  159

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   290

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
      35
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    32,    32,    32,    36,    37,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    84,
      84,    84,    84,    84,    85,    85,    85,    85,    85,    89,
      89,    89,    90,    90,    90,    94,    94,    94,    95,    95,
      95,    96,    97,   101,   102,   106,   107,   108,   109,   110,
     114,   115
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "REPETITIONS", "NUMBER_OF_PROCESSORS",
  "NAME_OF_EXPERIMENT", "CONFIGURATIONS", "INIT_PERCENT_OF_INDIVIDUALS",
  "MIGRATION_PROBABILITY", "NUMBER_OF_INDIVIDUALS_TO_MIGRATE", "NUMERO",
  "DOS_PUNTOS", "COMA", "STRING", "MAX_GLOBAL_FRONT_SIZE",
  "MAX_FINAL_SOLUTION_SIZE", "SEND_RESULTS_PER_GENERATION", "SHARING",
  "EXECUTION_LINE", "MACHINEFILE", "PREACTION", "CORCHETE_ABIERTO",
  "CORCHETE_CERRADO", "LLAVE_ABIERTA", "LLAVE_CERRADA", "EXECUTION_MODEL",
  "FLECHA", "CONVERSION", "CONVERSIONPARAMS", "SCORE",
  "INIT_POP_ISLAND_LOADER", "INIT_POP_ISLAND_LOADER_PARAMS",
  "MIGRATION_TOPOLOGY", "MIGRATION_TOPOLOGY_PARAMS", "PLUGIN_PATH",
  "NUMBER_PARALLEL_EXECUTIONS", "$accept", "inicio", "$@1", "experiment",
  "linea", "scores", "$@2", "$@3", "$@4", "$@5", "$@6", "$@7", "$@8",
  "$@9", "execution_models", "$@10", "$@11", "$@12", "$@13",
  "execution_model", "$@14", "$@15", "$@16", "$@17", "set_strings",
  "set_strings_set_numeros_o_vacio", "set_numeros", YY_NULLPTR
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
     285,   286,   287,   288,   289,   290
};
# endif

#define YYPACT_NINF -58

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-58)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -58,     7,    -3,   -58,     8,    10,    12,    34,    35,    36,
      37,    38,    39,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    -3,   -58,    30,
      56,    55,    57,    56,    56,    56,    56,    56,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    56,   -58,   -58,   -58,    72,   -58,   -58,    74,    72,
      72,    72,    72,    72,   -58,   -58,   -58,   -58,   -58,    71,
     -58,   -58,   -58,    73,   -58,   -58,   -58,   -58,   -58,    72,
      77,    76,    78,   -58,    79,   -58,   -58,   -58,    80,   -12,
      78,    82,    81,    75,    85,    83,    17,    84,    88,   -58,
      91,    86,    92,    89,   -58,    87,    97,   -58,    99,    90,
     100,    93,   -58,   -58,   104,   -58,    94,    96,    95,   103,
     -58,    98,   -58,    29,   102,   105,   108,   -58,   101,   -58,
     -58,    -4,    29,   106,   111,   -58,    31,   -58,    -2,   -58,
     107,   -58,   -58,   109,   -58,   -58,   110,   112,   113,   115,
     117,   114,   -58,   118,   119,   -58,   -58,   120,   -58
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     3,     5,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     4,     6,    61,     8,     9,    54,    11,    12,
      13,    14,    15,    16,    18,    19,    10,    20,    42,     7,
      21,    22,    34,    27,    23,    24,    25,    26,    28,    17,
       0,     0,     0,    39,     0,    29,    60,    53,     0,     0,
       0,     0,     0,    48,     0,     0,     0,     0,     0,    52,
       0,    45,     0,     0,    35,     0,     0,    51,     0,     0,
       0,     0,    30,    49,     0,    43,     0,     0,     0,     0,
      46,     0,    40,    59,     0,     0,     0,    44,     0,    58,
      57,     0,    59,     0,     0,    41,     0,    36,     0,    50,
       0,    56,    55,     0,    31,    47,     0,     0,     0,     0,
       0,     0,    37,     0,     0,    32,    38,     0,    33
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -58,   -58,   -58,   -58,    40,   -58,   -58,   -58,   -58,   -58,
     -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -21,
     -58,   -58,   -58,   -58,   -58,   -57,     0
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    27,    28,    73,    92,   118,   147,   157,
      84,   111,   143,   154,    69,    90,   128,    82,   121,    89,
     108,   126,   100,   119,    58,   131,    55
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
       4,     5,     6,     7,     8,     9,    10,     3,   136,    94,
     136,    11,    12,    13,    95,    14,    15,    16,   137,    29,
     144,    30,    17,    31,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    59,    60,    61,    62,    63,    94,   129,
      53,   141,   130,   103,   142,    32,    33,    34,    35,    36,
      37,    79,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    54,    52,    56,    96,
      57,    64,    65,    66,    67,   138,    70,    71,     0,    74,
      75,    76,    77,    78,    80,    68,    81,    86,    72,    87,
       0,     0,     0,    93,    83,    97,    85,    99,   101,    88,
      91,   105,    98,   106,   102,   109,   104,   113,   107,   112,
     110,   114,   115,   116,   120,   125,   122,   123,   133,   117,
     134,   124,   127,   132,   140,   135,   150,   153,   139,   145,
       0,   148,     0,     0,     0,   146,   151,     0,   149,   152,
     155,     0,     0,   156,   158
};

static const yytype_int16 yycheck[] =
{
       3,     4,     5,     6,     7,     8,     9,     0,    12,    21,
      12,    14,    15,    16,    26,    18,    19,    20,    22,    11,
      22,    11,    25,    11,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    33,    34,    35,    36,    37,    21,    10,
      10,    10,    13,    26,    13,    11,    11,    11,    11,    11,
      11,    51,    11,    11,    11,    11,    11,    11,    11,    11,
      11,    11,    11,    11,    11,    11,    10,    27,    13,    90,
      13,    13,    13,    13,    13,   132,    13,    13,    -1,    13,
      13,    13,    13,    13,    12,    23,    12,    10,    23,    13,
      -1,    -1,    -1,    13,    23,    13,    23,    22,    13,    21,
      21,    13,    21,    12,    21,    13,    22,    10,    22,    22,
      21,    12,    22,    13,    10,    12,    22,    21,    13,    26,
      12,    26,    24,    21,    13,    24,    13,    13,    22,    22,
      -1,    21,    -1,    -1,    -1,    26,    21,    -1,    26,    22,
      22,    -1,    -1,    24,    24
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    37,    38,     0,     3,     4,     5,     6,     7,     8,
       9,    14,    15,    16,    18,    19,    20,    25,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    39,    40,    11,
      11,    11,    11,    11,    11,    11,    11,    11,    11,    11,
      11,    11,    11,    11,    11,    11,    11,    11,    11,    11,
      11,    11,    40,    10,    10,    62,    13,    13,    60,    62,
      62,    62,    62,    62,    13,    13,    13,    13,    23,    50,
      13,    13,    23,    41,    13,    13,    13,    13,    13,    62,
      12,    12,    53,    23,    46,    23,    10,    13,    21,    55,
      51,    21,    42,    13,    21,    26,    55,    13,    21,    22,
      58,    13,    21,    26,    22,    13,    12,    22,    56,    13,
      21,    47,    22,    10,    12,    22,    13,    26,    43,    59,
      10,    54,    22,    21,    26,    12,    57,    24,    52,    10,
      13,    61,    21,    13,    12,    24,    12,    22,    61,    22,
      13,    10,    13,    48,    22,    22,    26,    44,    21,    26,
      13,    21,    22,    13,    49,    22,    24,    45,    24
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    36,    38,    37,    39,    39,    40,    40,    40,    40,
      40,    40,    40,    40,    40,    40,    40,    40,    40,    40,
      40,    40,    40,    40,    40,    40,    40,    40,    40,    42,
      43,    44,    45,    41,    46,    47,    48,    49,    41,    51,
      52,    50,    53,    54,    50,    56,    57,    55,    58,    59,
      55,    55,    55,    60,    60,    61,    61,    61,    61,    61,
      62,    62
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     0,
       0,     0,     0,    18,     0,     0,     0,     0,    17,     0,
       0,    10,     0,     0,     9,     0,     0,    10,     0,     0,
       9,     4,     3,     3,     1,     3,     3,     1,     1,     0,
       3,     1
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
      yyerror (YY_("syntax error: cannot back up")); \
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
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
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
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
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
yyparse (void)
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
#line 32 "../run/parseParExp.y" /* yacc.c:1646  */
    { executionModelDefined = false; sendResultsPerGenerationFixed = false; sharingFixed = false; }
#line 1367 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 41 "../run/parseParExp.y" /* yacc.c:1646  */
    { repetitions = (int)realValue; }
#line 1373 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 42 "../run/parseParExp.y" /* yacc.c:1646  */
    { if (executionModelDefined){ cout << "Error: executionModel redefined" << endl; exit(-1); } executionModelDefined = true; }
#line 1379 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 43 "../run/parseParExp.y" /* yacc.c:1646  */
    { comp_mueve(number_of_processors, realBuffer, "Number_of_processors"); }
#line 1385 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 44 "../run/parseParExp.y" /* yacc.c:1646  */
    { name_of_experiment = stringValue; }
#line 1391 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 45 "../run/parseParExp.y" /* yacc.c:1646  */
    { machinefile = stringValue; }
#line 1397 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 46 "../run/parseParExp.y" /* yacc.c:1646  */
    { comp_mueve(configurations, stringBuffer, "Configurations"); }
#line 1403 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 47 "../run/parseParExp.y" /* yacc.c:1646  */
    { comp_mueve(init_percent_of_individuals, realBuffer, "Init_percent_of_individuals"); }
#line 1409 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 48 "../run/parseParExp.y" /* yacc.c:1646  */
    { comp_mueve(migration_probability, realBuffer, "Migration_probability"); }
#line 1415 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 49 "../run/parseParExp.y" /* yacc.c:1646  */
    { comp_mueve(number_of_individuals_to_migrate, realBuffer, "Number_of_individuals_to_migrate"); }
#line 1421 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 50 "../run/parseParExp.y" /* yacc.c:1646  */
    { comp_mueve(max_global_front_size, realBuffer, "Max_front_size"); }
#line 1427 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 51 "../run/parseParExp.y" /* yacc.c:1646  */
    { comp_mueve(max_final_solution_size, realBuffer, "Max_final_solution_size"); }
#line 1433 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 52 "../run/parseParExp.y" /* yacc.c:1646  */
    { comp_mueve(number_parallel_executions, realBuffer, "Number_parallel_executions"); }
#line 1439 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 54 "../run/parseParExp.y" /* yacc.c:1646  */
    {
				if (sendResultsPerGenerationFixed){
					cout << "Error: Send_results_per_generation redefined" << endl;
					exit(-1);
				}
				sendResultsPerGenerationFixed = true;
				if (strcasecmp("yes", stringValue.c_str()) == 0){
					sendResultsPerGenerationBool = true;
				} 
				else if (strcasecmp("no", stringValue.c_str()) == 0){
					sendResultsPerGenerationBool = false;
				} else {
					cout << "Error: Send_results_per_generation not recognized value" << endl;
					exit(-1);
				}

			}
#line 1461 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 71 "../run/parseParExp.y" /* yacc.c:1646  */
    { execution_line = stringValue; }
#line 1467 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 72 "../run/parseParExp.y" /* yacc.c:1646  */
    {}
#line 1473 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 73 "../run/parseParExp.y" /* yacc.c:1646  */
    { conversion = stringValue; }
#line 1479 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 74 "../run/parseParExp.y" /* yacc.c:1646  */
    { conversionParams = stringValue; }
#line 1485 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 75 "../run/parseParExp.y" /* yacc.c:1646  */
    { initPopIslandLoader = stringValue; }
#line 1491 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 76 "../run/parseParExp.y" /* yacc.c:1646  */
    { initPopIslandLoaderParams = stringValue; }
#line 1497 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 77 "../run/parseParExp.y" /* yacc.c:1646  */
    { migrationTopology = stringValue; }
#line 1503 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 78 "../run/parseParExp.y" /* yacc.c:1646  */
    { migrationTopologyParams = stringValue; }
#line 1509 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 80 "../run/parseParExp.y" /* yacc.c:1646  */
    { pluginPath = stringValue; }
#line 1515 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 84 "../run/parseParExp.y" /* yacc.c:1646  */
    { Score emptyScore; scores.push_back(emptyScore); }
#line 1521 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 84 "../run/parseParExp.y" /* yacc.c:1646  */
    { scores[scores.size()-1].moduleName = stringValue; }
#line 1527 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 84 "../run/parseParExp.y" /* yacc.c:1646  */
    { scores[scores.size()-1].params = stringBuffer; stringBuffer.clear(); }
#line 1533 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 84 "../run/parseParExp.y" /* yacc.c:1646  */
    { scores[scores.size()-1].id = stringValue; }
#line 1539 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 85 "../run/parseParExp.y" /* yacc.c:1646  */
    { Score emptyScore; scores.push_back(emptyScore); }
#line 1545 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 85 "../run/parseParExp.y" /* yacc.c:1646  */
    { scores[scores.size()-1].moduleName = stringValue; }
#line 1551 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 85 "../run/parseParExp.y" /* yacc.c:1646  */
    { scores[scores.size()-1].params = stringBuffer; stringBuffer.clear(); }
#line 1557 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 85 "../run/parseParExp.y" /* yacc.c:1646  */
    { scores[scores.size()-1].id = stringValue; }
#line 1563 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 89 "../run/parseParExp.y" /* yacc.c:1646  */
    { execution_model.push_back(vector<string>()); }
#line 1569 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 89 "../run/parseParExp.y" /* yacc.c:1646  */
    { execution_model_names.push_back(stringValue); }
#line 1575 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 90 "../run/parseParExp.y" /* yacc.c:1646  */
    { execution_model.push_back(vector<string>()); }
#line 1581 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 90 "../run/parseParExp.y" /* yacc.c:1646  */
    { execution_model_names.push_back(stringValue); }
#line 1587 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 94 "../run/parseParExp.y" /* yacc.c:1646  */
    { tmpString.str(""); tmpString << stringValue; }
#line 1593 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 94 "../run/parseParExp.y" /* yacc.c:1646  */
    { tmpString << ", " << (long)realValue; }
#line 1599 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 94 "../run/parseParExp.y" /* yacc.c:1646  */
    { tmpString << ", " << stringValue; execution_model[execution_model.size() - 1].push_back(tmpString.str()); }
#line 1605 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 95 "../run/parseParExp.y" /* yacc.c:1646  */
    { tmpString.str(""); tmpString << stringValue; }
#line 1611 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 95 "../run/parseParExp.y" /* yacc.c:1646  */
    { tmpString << ", " << (long)realValue; }
#line 1617 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 95 "../run/parseParExp.y" /* yacc.c:1646  */
    { tmpString << ", " << stringValue; execution_model[execution_model.size() - 1].push_back(tmpString.str()); }
#line 1623 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 96 "../run/parseParExp.y" /* yacc.c:1646  */
    { execution_model[execution_model.size() - 1].push_back(stringValue); }
#line 1629 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 97 "../run/parseParExp.y" /* yacc.c:1646  */
    { execution_model[execution_model.size() - 1].push_back(stringValue); }
#line 1635 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 101 "../run/parseParExp.y" /* yacc.c:1646  */
    { stringBuffer.push_back(stringValue); }
#line 1641 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 102 "../run/parseParExp.y" /* yacc.c:1646  */
    { stringBuffer.push_back(stringValue); }
#line 1647 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 106 "../run/parseParExp.y" /* yacc.c:1646  */
    { stringBuffer.push_back(stringValue); }
#line 1653 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 107 "../run/parseParExp.y" /* yacc.c:1646  */
    { stringstream st; st.setf(ios::fixed); st << realValue; stringBuffer.push_back(st.str()); }
#line 1659 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 108 "../run/parseParExp.y" /* yacc.c:1646  */
    { stringBuffer.push_back(stringValue); }
#line 1665 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 109 "../run/parseParExp.y" /* yacc.c:1646  */
    { stringstream st; st.setf(ios::fixed); st << realValue; stringBuffer.push_back(st.str()); }
#line 1671 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 114 "../run/parseParExp.y" /* yacc.c:1646  */
    { realBuffer.push_back(realValue); }
#line 1677 "parseParExp.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 115 "../run/parseParExp.y" /* yacc.c:1646  */
    { realBuffer.push_back(realValue); }
#line 1683 "parseParExp.tab.c" /* yacc.c:1646  */
    break;


#line 1687 "parseParExp.tab.c" /* yacc.c:1646  */
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
      yyerror (YY_("syntax error"));
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
        yyerror (yymsgp);
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
                      yytoken, &yylval);
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
                  yystos[yystate], yyvsp);
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
  yyerror (YY_("memory exhausted"));
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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
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
#line 118 "../run/parseParExp.y" /* yacc.c:1906  */


void comp_mueve(vector<double> &destino, vector<double> &origen, const string &cad){
	if (destino.size() != 0){
		cout << "Error_: " << cad << " repeated" << endl;
		exit(-1);
	}
	destino = origen;
	origen.clear();
}

void comp_mueve(vector<int> &destino, vector<double> &origen, const string &cad){
	if (destino.size() != 0){
		cout << "Error_2: " << cad << " repeated" << endl;
		exit(-1);
	}
	for (unsigned int i = 0; i < origen.size(); i++)
		destino.push_back((int)origen[i]);
	origen.clear();
}

void comp_mueve(vector<string> &destino, vector<string> &origen, const string &cad){
	if (destino.size() != 0){
		cout << "Error_3: " << cad << " repeated" << endl;
		exit(-1);
	}
	destino = origen;
	origen.clear();
}

void expParerror(char *s) {
	errorParsing = 1; 
}



