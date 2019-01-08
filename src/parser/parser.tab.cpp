
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"

#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#include <cstdio>
#define __STRICT_ANSI__
#else
#include <cstdio>
#endif

#include <stdlib.h>
#include "../Constants.h"
#include "Tree.h"
#include "parser.yy.cpp"

int yylex();
void yyerror(const char *);

class Tree;


/* Line 189 of yacc.c  */
#line 94 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.tab.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     SELECT = 258,
     DELETE = 259,
     UPDATE = 260,
     INSERT = 261,
     CREATE = 262,
     DROP = 263,
     USE = 264,
     SHOW = 265,
     TABLES = 266,
     DATABASES = 267,
     DATABASE = 268,
     TABLE = 269,
     FROM = 270,
     WHERE = 271,
     OPERATOR = 272,
     VALUES = 273,
     SET = 274,
     INTO = 275,
     KINT = 276,
     KCHAR = 277,
     KFLOAT = 278,
     KVARCHAR = 279,
     KDATE = 280,
     PRIMARY = 281,
     FOREIGN = 282,
     REFERENCES = 283,
     INTEGER = 284,
     FLOAT = 285,
     STRING = 286,
     IDENTIFIER = 287,
     DATE_FORMAT = 288,
     EQ = 289,
     GT = 290,
     LT = 291,
     GE = 292,
     LE = 293,
     NE = 294,
     AVG = 295,
     SUM = 296,
     MIN = 297,
     MAX = 298,
     NOTNULL = 299,
     DESC = 300,
     GROUP = 301,
     LIKE = 302,
     INDEX = 303,
     CHECK = 304,
     IN = 305,
     T_NULL = 306,
     IS = 307,
     AND = 308
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 21 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"

    int ivalue;
    AttrType attrType;
    float fvalue;
    char *string;
    Tree *tree;
    Select *selectTree;
    Delete *deleteTree;
    AttributeList *attributesTree;
    AttributeNode *attributeTree;
    IdentList *identList;
    ConstValueList *constValuesTree;
    ColumnDecsList *columnsTree;
    ColumnNode *columnTree;
    ConstValueLists *insertValueTree;
    SetClauseList *setClauseList;
    TableConstraint *tbOptDec;
    TableConstraintList *tbOptDecList;
    Expr * expr;



/* Line 214 of yacc.c  */
#line 206 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.tab.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 218 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.tab.cpp"

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
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
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
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  36
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   236

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  63
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  97
/* YYNRULES -- Number of states.  */
#define YYNSTATES  219

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   308

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      56,    57,    55,    60,    58,    61,    59,    62,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    54,
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    10,    13,    17,    21,    24,
      28,    31,    35,    41,    47,    55,    62,    68,    73,    81,
      85,    88,    95,   102,   104,   108,   114,   118,   124,   126,
     130,   135,   140,   142,   146,   150,   151,   153,   155,   157,
     159,   161,   165,   166,   169,   170,   172,   175,   176,   178,
     182,   192,   197,   206,   208,   212,   214,   216,   221,   226,
     231,   236,   240,   247,   254,   261,   268,   270,   274,   277,
     278,   280,   284,   288,   292,   296,   300,   304,   308,   312,
     316,   320,   322,   326,   330,   334,   336,   340,   344,   346,
     348,   350,   353,   357,   359,   361,   363,   365
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      64,     0,    -1,    65,    -1,    64,    54,    65,    -1,    -1,
      10,    12,    -1,     7,    13,    32,    -1,     8,    13,    32,
      -1,     9,    32,    -1,    10,    13,    32,    -1,    10,    11,
      -1,    10,    14,    32,    -1,     3,    55,    15,    81,    82,
      -1,     3,    79,    15,    81,    82,    -1,     3,    79,    15,
      81,    82,    46,    32,    -1,     6,    20,    32,    71,    18,
      66,    -1,     5,    32,    19,    67,    82,    -1,     4,    15,
      32,    82,    -1,     7,    14,    32,    56,    68,    76,    57,
      -1,     8,    14,    32,    -1,    45,    32,    -1,     7,    48,
      32,    56,    80,    57,    -1,     8,    48,    32,    56,    80,
      57,    -1,    45,    -1,    56,    85,    57,    -1,    66,    58,
      56,    85,    57,    -1,    80,    34,    86,    -1,    67,    58,
      80,    34,    86,    -1,    69,    -1,    68,    58,    69,    -1,
      32,    72,    73,    74,    -1,    25,    72,    73,    74,    -1,
      32,    -1,    70,    58,    32,    -1,    56,    70,    57,    -1,
      -1,    21,    -1,    22,    -1,    24,    -1,    23,    -1,    25,
      -1,    56,    29,    57,    -1,    -1,    74,    75,    -1,    -1,
      44,    -1,    58,    77,    -1,    -1,    78,    -1,    77,    58,
      78,    -1,    27,    56,    32,    57,    28,    32,    56,    32,
      57,    -1,    26,    56,    70,    57,    -1,    49,    56,    32,
      50,    56,    85,    57,    57,    -1,    80,    -1,    79,    58,
      80,    -1,    32,    -1,    25,    -1,    41,    56,    32,    57,
      -1,    40,    56,    32,    57,    -1,    42,    56,    32,    57,
      -1,    43,    56,    32,    57,    -1,    32,    59,    32,    -1,
      41,    56,    32,    59,    32,    57,    -1,    40,    56,    32,
      59,    32,    57,    -1,    43,    56,    32,    59,    32,    57,
      -1,    42,    56,    32,    59,    32,    57,    -1,    32,    -1,
      81,    58,    32,    -1,    16,    83,    -1,    -1,    84,    -1,
      83,    53,    84,    -1,    86,    34,    86,    -1,    86,    35,
      86,    -1,    86,    36,    86,    -1,    86,    37,    86,    -1,
      86,    38,    86,    -1,    86,    39,    86,    -1,    86,    47,
      86,    -1,    86,    52,    51,    -1,    86,    52,    44,    -1,
      89,    -1,    85,    58,    89,    -1,    86,    60,    87,    -1,
      86,    61,    87,    -1,    87,    -1,    87,    55,    88,    -1,
      87,    62,    88,    -1,    88,    -1,    89,    -1,    80,    -1,
      61,    88,    -1,    56,    86,    57,    -1,    29,    -1,    30,
      -1,    31,    -1,    33,    -1,    51,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    84,    84,    88,    95,    98,   104,   111,   118,   125,
     131,   137,   143,   149,   155,   162,   169,   176,   183,   190,
     197,   204,   211,   218,   227,   232,   239,   244,   251,   256,
     263,   269,   276,   282,   289,   293,   296,   297,   298,   299,
     300,   304,   305,   308,   309,   312,   316,   317,   321,   326,
     332,   339,   343,   349,   354,   361,   366,   370,   375,   380,
     385,   390,   396,   402,   408,   414,   423,   429,   437,   442,
     448,   452,   459,   463,   467,   471,   475,   479,   483,   487,
     491,   498,   503,   509,   512,   515,   518,   521,   524,   528,
     529,   532,   535,   541,   542,   543,   544,   545
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SELECT", "DELETE", "UPDATE", "INSERT",
  "CREATE", "DROP", "USE", "SHOW", "TABLES", "DATABASES", "DATABASE",
  "TABLE", "FROM", "WHERE", "OPERATOR", "VALUES", "SET", "INTO", "KINT",
  "KCHAR", "KFLOAT", "KVARCHAR", "KDATE", "PRIMARY", "FOREIGN",
  "REFERENCES", "INTEGER", "FLOAT", "STRING", "IDENTIFIER", "DATE_FORMAT",
  "EQ", "GT", "LT", "GE", "LE", "NE", "AVG", "SUM", "MIN", "MAX",
  "NOTNULL", "DESC", "GROUP", "LIKE", "INDEX", "CHECK", "IN", "T_NULL",
  "IS", "AND", "';'", "'*'", "'('", "')'", "','", "'.'", "'+'", "'-'",
  "'/'", "$accept", "sql", "command", "valueLists", "setList",
  "column_decs", "column_dec", "column_list", "column_list_exist",
  "column_type", "type_width", "column_constraints", "column_constraint",
  "tb_opt_exist", "tb_opt_decs", "tb_opt_dec", "attributes", "attribute",
  "tableList", "whereclause", "conditions", "comparison", "constvalues",
  "expr", "factor", "value", "constvalue", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,    59,    42,    40,    41,    44,    46,
      43,    45,    47
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    63,    64,    64,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    66,    66,    67,    67,    68,    68,
      69,    69,    70,    70,    71,    71,    72,    72,    72,    72,
      72,    73,    73,    74,    74,    75,    76,    76,    77,    77,
      78,    78,    78,    79,    79,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    81,    81,    82,    82,
      83,    83,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    85,    85,    86,    86,    86,    87,    87,    87,    88,
      88,    88,    88,    89,    89,    89,    89,    89
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     0,     2,     3,     3,     2,     3,
       2,     3,     5,     5,     7,     6,     5,     4,     7,     3,
       2,     6,     6,     1,     3,     5,     3,     5,     1,     3,
       4,     4,     1,     3,     3,     0,     1,     1,     1,     1,
       1,     3,     0,     2,     0,     1,     2,     0,     1,     3,
       9,     4,     8,     1,     3,     1,     1,     4,     4,     4,
       4,     3,     6,     6,     6,     6,     1,     3,     2,     0,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     3,     3,     3,     1,     3,     3,     1,     1,
       1,     2,     3,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,     2,    56,    55,     0,     0,     0,     0,     0,     0,
      53,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       8,    10,     5,     0,     0,    20,     1,     4,     0,     0,
       0,     0,     0,     0,     0,     0,    69,     0,    35,     6,
       0,     0,     7,    19,     0,     9,    11,     3,    61,     0,
       0,     0,     0,    66,    69,    69,    54,     0,    17,    69,
       0,     0,     0,     0,     0,     0,    58,     0,    57,     0,
      59,     0,    60,     0,     0,    12,    13,    93,    94,    95,
      96,    97,     0,     0,    90,    68,    70,     0,    85,    88,
      89,     0,    16,     0,    32,     0,     0,     0,     0,    47,
      28,     0,     0,     0,     0,     0,     0,    67,     0,     0,
      91,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    26,    34,     0,     0,    15,
      36,    37,    39,    38,    40,    42,    42,     0,     0,    21,
      22,    63,    62,    65,    64,    14,    92,    71,    72,    73,
      74,    75,    76,    77,    78,    80,    79,    83,    84,    86,
      87,     0,    33,     0,    81,     0,     0,    44,    44,     0,
       0,     0,    29,    46,    48,    18,    27,    24,     0,     0,
       0,    31,    30,     0,     0,     0,     0,    82,     0,    41,
      45,    43,     0,     0,     0,    49,    25,    51,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    52,    50
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    10,    11,   139,    69,   109,   110,   105,    72,   145,
     177,   191,   201,   148,   183,   184,    19,    94,    64,    68,
      95,    96,   173,    97,    98,    99,   100
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -185
static const yytype_int16 yypact[] =
{
      10,    61,    44,   -22,    65,    -5,     8,    22,   116,    51,
       3,  -185,  -185,    40,    58,    66,    93,    95,   102,   -11,
    -185,   118,   133,   121,   122,   123,   124,   125,   126,   127,
    -185,  -185,  -185,   128,   129,  -185,  -185,    10,   130,   131,
     132,   134,   135,   136,   136,    94,   149,    94,   113,  -185,
     114,   115,  -185,  -185,   119,  -185,  -185,  -185,  -185,   -31,
       1,    25,    39,  -185,   -10,   -10,  -185,    36,  -185,    -9,
     138,   141,   156,    -2,    94,    94,  -185,   144,  -185,   145,
    -185,   146,  -185,   147,   148,  -185,   137,  -185,  -185,  -185,
    -185,  -185,    36,    36,  -185,   139,  -185,    71,    26,  -185,
    -185,    94,  -185,    36,  -185,    14,   140,   117,   117,   142,
    -185,   150,   151,   152,   153,   154,   155,  -185,   157,    34,
    -185,    36,    36,    36,    36,    36,    36,    36,    36,    19,
      36,    36,    36,    36,   159,    29,  -185,   158,    82,   143,
    -185,  -185,  -185,  -185,  -185,   160,   160,    13,   161,  -185,
    -185,  -185,  -185,  -185,  -185,  -185,  -185,  -185,    29,    29,
      29,    29,    29,    29,    29,  -185,  -185,    26,    26,  -185,
    -185,    36,  -185,    63,  -185,   163,   162,  -185,  -185,   164,
     165,   166,  -185,   167,  -185,  -185,    29,  -185,    82,    82,
     169,   170,   170,   141,   171,   172,    15,  -185,    67,  -185,
    -185,  -185,    86,   173,   174,  -185,  -185,  -185,   177,   175,
     181,    82,   176,    88,   183,   178,   179,  -185,  -185
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -185,  -185,   180,  -185,  -185,  -185,    35,   -12,  -185,    76,
      41,     7,  -185,  -185,  -185,    -8,  -185,    -1,   184,   -40,
    -185,    73,  -184,   -91,    17,   -82,  -136
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      20,   119,   174,    36,    44,   198,    67,    67,    24,    25,
      22,   120,   135,     1,     2,     3,     4,     5,     6,     7,
       8,    27,    28,   107,    85,    86,    76,   213,    77,   102,
     108,   158,   159,   160,   161,   162,   163,   164,   107,   179,
     180,   179,   180,    26,    66,   108,    70,    45,    84,   101,
     169,   170,   197,   174,    30,     9,    29,    37,    78,    21,
      79,    12,   181,   165,   181,    87,    88,    89,    13,    90,
     166,   136,   137,   111,   112,   174,    14,    15,    16,    17,
     186,   132,    80,    35,    81,    23,    12,    91,   133,   130,
     131,   156,    92,    13,   130,   131,    82,    93,    83,    38,
     134,    14,    15,    16,    17,   122,   123,   124,   125,   126,
     127,    87,    88,    89,    39,    90,    18,    43,   128,    12,
     187,   188,    40,   129,   206,   188,    13,    31,    32,    33,
      34,   130,   131,    91,    14,    15,    16,    17,   140,   141,
     142,   143,   144,   207,   137,   215,   188,   167,   168,    41,
      46,    42,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    58,    59,    60,    67,    61,    62,    63,    71,
      73,    74,   103,   104,   106,    75,   113,   114,   115,   116,
     117,   202,   182,   118,   146,   192,     0,   178,   205,   155,
     172,   190,   121,   171,   157,     0,   138,     0,     0,     0,
     147,   175,     0,   203,   204,   210,     0,   149,   150,   151,
     152,   153,   154,   212,   200,   216,   176,    57,   185,   189,
     193,   194,   195,     0,   209,   196,   199,     0,    65,     0,
     208,   211,   214,     0,     0,   217,   218
};

static const yytype_int16 yycheck[] =
{
       1,    92,   138,     0,    15,   189,    16,    16,    13,    14,
      32,    93,   103,     3,     4,     5,     6,     7,     8,     9,
      10,    13,    14,    25,    64,    65,    57,   211,    59,    69,
      32,   122,   123,   124,   125,   126,   127,   128,    25,    26,
      27,    26,    27,    48,    45,    32,    47,    58,    58,    58,
     132,   133,   188,   189,    32,    45,    48,    54,    57,    15,
      59,    25,    49,    44,    49,    29,    30,    31,    32,    33,
      51,    57,    58,    74,    75,   211,    40,    41,    42,    43,
     171,    55,    57,    32,    59,    20,    25,    51,    62,    60,
      61,    57,    56,    32,    60,    61,    57,    61,    59,    59,
     101,    40,    41,    42,    43,    34,    35,    36,    37,    38,
      39,    29,    30,    31,    56,    33,    55,    15,    47,    25,
      57,    58,    56,    52,    57,    58,    32,    11,    12,    13,
      14,    60,    61,    51,    40,    41,    42,    43,    21,    22,
      23,    24,    25,    57,    58,    57,    58,   130,   131,    56,
      32,    56,    19,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    16,    32,    32,    32,    56,
      56,    56,    34,    32,    18,    56,    32,    32,    32,    32,
      32,   193,   147,    46,   108,   178,    -1,   146,   196,    32,
      32,    29,    53,    34,   121,    -1,    56,    -1,    -1,    -1,
      58,    58,    -1,    32,    32,    28,    -1,    57,    57,    57,
      57,    57,    57,    32,    44,    32,    56,    37,    57,    56,
      56,    56,    56,    -1,    50,    58,    57,    -1,    44,    -1,
      57,    56,    56,    -1,    -1,    57,    57
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    45,
      64,    65,    25,    32,    40,    41,    42,    43,    55,    79,
      80,    15,    32,    20,    13,    14,    48,    13,    14,    48,
      32,    11,    12,    13,    14,    32,     0,    54,    59,    56,
      56,    56,    56,    15,    15,    58,    32,    19,    32,    32,
      32,    32,    32,    32,    32,    32,    32,    65,    32,    32,
      32,    32,    32,    32,    81,    81,    80,    16,    82,    67,
      80,    56,    71,    56,    56,    56,    57,    59,    57,    59,
      57,    59,    57,    59,    58,    82,    82,    29,    30,    31,
      33,    51,    56,    61,    80,    83,    84,    86,    87,    88,
      89,    58,    82,    34,    32,    70,    18,    25,    32,    68,
      69,    80,    80,    32,    32,    32,    32,    32,    46,    86,
      88,    53,    34,    35,    36,    37,    38,    39,    47,    52,
      60,    61,    55,    62,    80,    86,    57,    58,    56,    66,
      21,    22,    23,    24,    25,    72,    72,    58,    76,    57,
      57,    57,    57,    57,    57,    32,    57,    84,    86,    86,
      86,    86,    86,    86,    86,    44,    51,    87,    87,    88,
      88,    34,    32,    85,    89,    58,    56,    73,    73,    26,
      27,    49,    69,    77,    78,    57,    86,    57,    58,    56,
      29,    74,    74,    56,    56,    56,    58,    89,    85,    57,
      44,    75,    70,    32,    32,    78,    57,    57,    57,    50,
      28,    56,    32,    85,    56,    57,    32,    57,    57
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
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
  int yytoken;
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

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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
  *++yyvsp = yylval;

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
     `$$ = $1'.

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

/* Line 1455 of yacc.c  */
#line 85 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {

            ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 89 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {

            ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 95 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {

            ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 99 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new ShowDatabase("");
                Tree::setInstance((yyval.tree));
                Tree::run();
            ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 105 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new CreateDatabase((yyvsp[(3) - (3)].string));
                Tree::setInstance((yyval.tree));
                delete (yyvsp[(3) - (3)].string);
                Tree::run();
            ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 112 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new DropDatabase((yyvsp[(3) - (3)].string));
                Tree::setInstance((yyval.tree));
                delete (yyvsp[(3) - (3)].string);
                Tree::run();
            ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 119 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new UseDatabase((yyvsp[(2) - (2)].string));
                Tree::setInstance((yyval.tree));
                delete (yyvsp[(2) - (2)].string);
                Tree::run();
            ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 126 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new ShowDatabase((yyvsp[(3) - (3)].string));
                Tree::setInstance((yyval.tree));
                Tree::run();
            ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 132 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new DescTable("");
                Tree::setInstance((yyval.tree));
                Tree::run();
            ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 138 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new DescTable((yyvsp[(3) - (3)].string));
                Tree::setInstance((yyval.tree));
                Tree::run();
            ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 144 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new Select((yyvsp[(4) - (5)].identList), (yyvsp[(5) - (5)].expr));
                Tree::setInstance((yyval.tree));
                Tree::run();
            ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 150 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new Select((yyvsp[(2) - (5)].attributesTree), (yyvsp[(4) - (5)].identList), (yyvsp[(5) - (5)].expr));
                Tree::setInstance((yyval.tree));
                Tree::run();
            ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 156 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new Select((yyvsp[(2) - (7)].attributesTree), (yyvsp[(4) - (7)].identList), (yyvsp[(5) - (7)].expr), (yyvsp[(7) - (7)].string));
                delete (yyvsp[(7) - (7)].string);
                Tree::setInstance((yyval.tree));
                Tree::run();
            ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 163 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new Insert((yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].identList), (yyvsp[(6) - (6)].insertValueTree));
                Tree::setInstance((yyval.tree));
                delete (yyvsp[(3) - (6)].string);
                Tree::run();
            ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 170 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new Update((yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].setClauseList), (yyvsp[(5) - (5)].expr));
                Tree::setInstance((yyval.tree));
                delete (yyvsp[(2) - (5)].string);
                Tree::run();
            ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 177 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new Delete((yyvsp[(3) - (4)].string), (yyvsp[(4) - (4)].expr));
                Tree::setInstance((yyval.tree));
                delete (yyvsp[(3) - (4)].string);
                Tree::run();
            ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 184 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new CreateTable((yyvsp[(3) - (7)].string), (yyvsp[(5) - (7)].columnsTree), (yyvsp[(6) - (7)].tbOptDecList));
                Tree::setInstance((yyval.tree));
                delete (yyvsp[(3) - (7)].string);
                Tree::run();
            ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 191 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new DropTable((yyvsp[(3) - (3)].string));
                Tree::setInstance((yyval.tree));
                delete (yyvsp[(3) - (3)].string);
                Tree::run();
            ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 198 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new DescTable((yyvsp[(2) - (2)].string));
                Tree::setInstance((yyval.tree));
                delete (yyvsp[(2) - (2)].string);
                Tree::run();
            ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 205 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new CreateIndex((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].attributeTree));
                Tree::setInstance((yyval.tree));
                delete (yyvsp[(3) - (6)].string);
                Tree::run();
            ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 212 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new DropIndex((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].attributeTree));
                Tree::setInstance((yyval.tree));
                delete (yyvsp[(3) - (6)].string);
                Tree::run();
            ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 219 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tree) = new DescTable("");
                Tree::setInstance((yyval.tree));
                Tree::run();
            ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 228 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.insertValueTree) = new ConstValueLists();
                (yyval.insertValueTree)->addConstValues((yyvsp[(2) - (3)].constValuesTree));
            ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 233 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.insertValueTree)->addConstValues((yyvsp[(4) - (5)].constValuesTree));
            ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 240 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.setClauseList) = new SetClauseList();
                (yyval.setClauseList)->addSetClause((yyvsp[(1) - (3)].attributeTree), (yyvsp[(3) - (3)].expr));
            ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 245 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.setClauseList)->addSetClause((yyvsp[(3) - (5)].attributeTree), (yyvsp[(5) - (5)].expr));
            ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 252 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.columnsTree) = new ColumnDecsList();
                (yyval.columnsTree)->addColumn((yyvsp[(1) - (1)].columnTree));
            ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 257 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.columnsTree)->addColumn((yyvsp[(3) - (3)].columnTree));
            ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 264 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.columnTree) = new ColumnNode((yyvsp[(1) - (4)].string), (yyvsp[(2) - (4)].attrType), (yyvsp[(3) - (4)].ivalue), (yyvsp[(4) - (4)].ivalue));
                delete (yyvsp[(1) - (4)].string);

            ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 270 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.columnTree) = new ColumnNode("date", (yyvsp[(2) - (4)].attrType), (yyvsp[(3) - (4)].ivalue), (yyvsp[(4) - (4)].ivalue));
            ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 277 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.identList) = new IdentList();
                (yyval.identList)->addIdent((yyvsp[(1) - (1)].string));
                delete (yyvsp[(1) - (1)].string);
            ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 283 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.identList)->addIdent((yyvsp[(3) - (3)].string));
                delete (yyvsp[(3) - (3)].string);
            ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 290 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.identList) = (yyvsp[(2) - (3)].identList);
            ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 293 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {   (yyval.identList) = NULL;   ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 296 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.attrType)=AttrType::INT;;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 297 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.attrType)=AttrType::STRING;;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 298 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.attrType)=AttrType::VARCHAR;;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 299 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.attrType)=AttrType::FLOAT;;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 300 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.attrType)=AttrType::DATE;;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 304 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.ivalue) = (yyvsp[(2) - (3)].ivalue);;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 305 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.ivalue)=0;;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 308 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.ivalue) = (yyvsp[(1) - (2)].ivalue) | (yyvsp[(2) - (2)].ivalue);;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 309 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.ivalue)=0;;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 312 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.ivalue) = COLUMN_FLAG_NOTNULL;;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 316 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.tbOptDecList) = (yyvsp[(2) - (2)].tbOptDecList);;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 317 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.tbOptDecList) = NULL;;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 322 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tbOptDecList) = new TableConstraintList();
                (yyval.tbOptDecList)->addTbDec((yyvsp[(1) - (1)].tbOptDec));
            ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 327 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tbOptDecList)->addTbDec((yyvsp[(3) - (3)].tbOptDec));
            ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 333 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tbOptDec) = new TableConstraint((yyvsp[(3) - (9)].string), (yyvsp[(6) - (9)].string), (yyvsp[(8) - (9)].string));
                delete (yyvsp[(3) - (9)].string);
                delete (yyvsp[(6) - (9)].string);
                delete (yyvsp[(8) - (9)].string);
            ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 340 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tbOptDec) = new TableConstraint((yyvsp[(3) - (4)].identList));
            ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 344 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.tbOptDec) = new TableConstraint((yyvsp[(3) - (8)].string), (yyvsp[(6) - (8)].constValuesTree));
            ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 350 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributesTree) = new AttributeList();
                (yyval.attributesTree)->addAttribute((yyvsp[(1) - (1)].attributeTree));
            ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 355 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributesTree)->addAttribute((yyvsp[(3) - (3)].attributeTree));
            ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 362 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributeTree) = new AttributeNode((yyvsp[(1) - (1)].string));
                delete (yyvsp[(1) - (1)].string);
            ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 367 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributeTree) = new AttributeNode("date");
            ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 371 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributeTree) =  new AttributeNode((yyvsp[(3) - (4)].string), AggregationType::T_SUM);
                delete (yyvsp[(3) - (4)].string);
            ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 376 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributeTree) =  new AttributeNode((yyvsp[(3) - (4)].string), AggregationType::T_AVG);
                delete (yyvsp[(3) - (4)].string);
            ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 381 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributeTree) =  new AttributeNode((yyvsp[(3) - (4)].string), AggregationType::T_MIN);
                delete (yyvsp[(3) - (4)].string);
            ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 386 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributeTree) =  new AttributeNode((yyvsp[(3) - (4)].string), AggregationType::T_MAX);
                delete (yyvsp[(3) - (4)].string);
            ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 391 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributeTree) = new AttributeNode((yyvsp[(1) - (3)].string), (yyvsp[(3) - (3)].string));
                delete (yyvsp[(1) - (3)].string);
                delete (yyvsp[(3) - (3)].string);
            ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 397 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributeTree) = new AttributeNode((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].string), AggregationType::T_SUM);
                delete (yyvsp[(3) - (6)].string);
                delete (yyvsp[(5) - (6)].string);
            ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 403 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributeTree) = new AttributeNode((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].string), AggregationType::T_AVG);
                delete (yyvsp[(3) - (6)].string);
                delete (yyvsp[(5) - (6)].string);
            ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 409 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributeTree) = new AttributeNode((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].string), AggregationType::T_MAX);
                delete (yyvsp[(3) - (6)].string);
                delete (yyvsp[(5) - (6)].string);
            ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 415 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.attributeTree) = new AttributeNode((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].string), AggregationType::T_MIN);
                delete (yyvsp[(3) - (6)].string);
                delete (yyvsp[(5) - (6)].string);
            ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 424 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.identList) = new IdentList();
                (yyval.identList)->addIdent((yyvsp[(1) - (1)].string));
                delete (yyvsp[(1) - (1)].string);
            ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 430 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.identList)->addIdent((yyvsp[(3) - (3)].string));
                delete (yyvsp[(3) - (3)].string);
            ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 438 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = (yyvsp[(2) - (2)].expr);
            ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 442 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = new Expr(true);
            ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 449 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = (yyvsp[(1) - (1)].expr);
            ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 453 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), LogicOp::AND_OP, (yyvsp[(3) - (3)].expr));
            ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 460 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), CompOp::EQ_OP, (yyvsp[(3) - (3)].expr));
            ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 464 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), CompOp::GT_OP, (yyvsp[(3) - (3)].expr));
            ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 468 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), CompOp::LT_OP, (yyvsp[(3) - (3)].expr));
            ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 472 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), CompOp::GE_OP, (yyvsp[(3) - (3)].expr));
            ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 476 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), CompOp::LE_OP, (yyvsp[(3) - (3)].expr));
            ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 480 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), CompOp::NE_OP, (yyvsp[(3) - (3)].expr));
            ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 484 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), CompOp::LIKE_OP, (yyvsp[(3) - (3)].expr));
            ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 488 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), CompOp::IS_OP, new Expr());
            ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 492 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), CompOp::ISNOT_OP, new Expr());
            ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 499 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.constValuesTree) = new ConstValueList();
                (yyval.constValuesTree)->addConstValue((yyvsp[(1) - (1)].expr));
            ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 504 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
                (yyval.constValuesTree)->addConstValue((yyvsp[(3) - (3)].expr));
            ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 509 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
            (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), ArithOp::ADD_OP, (yyvsp[(3) - (3)].expr));
        ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 512 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
            (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), ArithOp::SUB_OP, (yyvsp[(3) - (3)].expr));
        ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 515 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.expr)=(yyvsp[(1) - (1)].expr);;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 518 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
            (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), ArithOp::MUL_OP, (yyvsp[(3) - (3)].expr));
        ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 521 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
           (yyval.expr) = new Expr((yyvsp[(1) - (3)].expr), ArithOp::DIV_OP, (yyvsp[(3) - (3)].expr));
        ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 524 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.expr)=(yyvsp[(1) - (1)].expr);;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 528 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {(yyval.expr) = (yyvsp[(1) - (1)].expr);;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 529 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
        (yyval.expr) = new Expr((yyvsp[(1) - (1)].attributeTree));
    ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 532 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
        (yyval.expr) = new Expr((yyvsp[(2) - (2)].expr), ArithOp::MINUS_OP);
    ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 535 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
        (yyval.expr) = (yyvsp[(2) - (3)].expr);
    ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 541 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    { (yyval.expr) = new Expr((yyvsp[(1) - (1)].ivalue)); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 542 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    { (yyval.expr) = new Expr((yyvsp[(1) - (1)].fvalue)); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 543 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    { (yyval.expr) = new Expr((yyvsp[(1) - (1)].string)); delete (yyvsp[(1) - (1)].string); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 544 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    { (yyval.expr) = new Expr((yyvsp[(1) - (1)].string), true); delete (yyvsp[(1) - (1)].string); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 545 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"
    {
        (yyval.expr) = new Expr();
    ;}
    break;



/* Line 1455 of yacc.c  */
#line 2509 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.tab.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
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

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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

  *++yyvsp = yylval;


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

#if !defined(yyoverflow) || YYERROR_VERBOSE
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
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 549 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.y"


void yyerror(const char *msg) {
    printf("YACC error: %s\n", msg);
}

char start_parse(const char *expr_input)
{
    char ret;
    if(expr_input){
        YY_BUFFER_STATE my_string_buffer = yy_scan_string(expr_input);
        yy_switch_to_buffer( my_string_buffer ); // switch flex to the buffer we just created
        ret = yyparse();
        yy_delete_buffer(my_string_buffer );
    }else{
        ret = yyparse();
    }
    return ret;
}


