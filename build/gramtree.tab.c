/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "gramtree.y"

#include<unistd.h>
#include<stdio.h>
#include "Ast.h"
#include "cds.h"
#include "c_container_auxiliary.h"
#line 9 "gramtree.y"

int yylex();

#line 82 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "gramtree.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INTEGER = 3,                    /* INTEGER  */
  YYSYMBOL_FLOAT = 4,                      /* FLOAT  */
  YYSYMBOL_TYPE = 5,                       /* TYPE  */
  YYSYMBOL_CONST = 6,                      /* CONST  */
  YYSYMBOL_STRUCT = 7,                     /* STRUCT  */
  YYSYMBOL_RETURN = 8,                     /* RETURN  */
  YYSYMBOL_IF = 9,                         /* IF  */
  YYSYMBOL_ELSE = 10,                      /* ELSE  */
  YYSYMBOL_WHILE = 11,                     /* WHILE  */
  YYSYMBOL_ID = 12,                        /* ID  */
  YYSYMBOL_SPACE = 13,                     /* SPACE  */
  YYSYMBOL_SEMI = 14,                      /* SEMI  */
  YYSYMBOL_COMMA = 15,                     /* COMMA  */
  YYSYMBOL_ASSIGNOP = 16,                  /* ASSIGNOP  */
  YYSYMBOL_BREAK = 17,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 18,                  /* CONTINUE  */
  YYSYMBOL_GREAT = 19,                     /* GREAT  */
  YYSYMBOL_GREATEQUAL = 20,                /* GREATEQUAL  */
  YYSYMBOL_LESS = 21,                      /* LESS  */
  YYSYMBOL_LESSEQUAL = 22,                 /* LESSEQUAL  */
  YYSYMBOL_NOTEQUAL = 23,                  /* NOTEQUAL  */
  YYSYMBOL_EQUAL = 24,                     /* EQUAL  */
  YYSYMBOL_PLUS = 25,                      /* PLUS  */
  YYSYMBOL_MINUS = 26,                     /* MINUS  */
  YYSYMBOL_STAR = 27,                      /* STAR  */
  YYSYMBOL_DIV = 28,                       /* DIV  */
  YYSYMBOL_MOD = 29,                       /* MOD  */
  YYSYMBOL_AND = 30,                       /* AND  */
  YYSYMBOL_OR = 31,                        /* OR  */
  YYSYMBOL_DOT = 32,                       /* DOT  */
  YYSYMBOL_NOT = 33,                       /* NOT  */
  YYSYMBOL_LP = 34,                        /* LP  */
  YYSYMBOL_RP = 35,                        /* RP  */
  YYSYMBOL_LB = 36,                        /* LB  */
  YYSYMBOL_RB = 37,                        /* RB  */
  YYSYMBOL_LC = 38,                        /* LC  */
  YYSYMBOL_RC = 39,                        /* RC  */
  YYSYMBOL_AERROR = 40,                    /* AERROR  */
  YYSYMBOL_EOL = 41,                       /* EOL  */
  YYSYMBOL_YYACCEPT = 42,                  /* $accept  */
  YYSYMBOL_Program = 43,                   /* Program  */
  YYSYMBOL_ExtDefList = 44,                /* ExtDefList  */
  YYSYMBOL_ExtDef = 45,                    /* ExtDef  */
  YYSYMBOL_ExtDecList = 46,                /* ExtDecList  */
  YYSYMBOL_Specifire = 47,                 /* Specifire  */
  YYSYMBOL_StructSpecifire = 48,           /* StructSpecifire  */
  YYSYMBOL_OptTag = 49,                    /* OptTag  */
  YYSYMBOL_Tag = 50,                       /* Tag  */
  YYSYMBOL_VarDec = 51,                    /* VarDec  */
  YYSYMBOL_FunDec = 52,                    /* FunDec  */
  YYSYMBOL_VarList = 53,                   /* VarList  */
  YYSYMBOL_ParamDec = 54,                  /* ParamDec  */
  YYSYMBOL_Compst = 55,                    /* Compst  */
  YYSYMBOL_TACList = 56,                   /* TACList  */
  YYSYMBOL_TAC = 57,                       /* TAC  */
  YYSYMBOL_StmtList = 58,                  /* StmtList  */
  YYSYMBOL_Stmt = 59,                      /* Stmt  */
  YYSYMBOL_DefList = 60,                   /* DefList  */
  YYSYMBOL_Def = 61,                       /* Def  */
  YYSYMBOL_DecList = 62,                   /* DecList  */
  YYSYMBOL_Dec = 63,                       /* Dec  */
  YYSYMBOL_InitList = 64,                  /* InitList  */
  YYSYMBOL_Exp = 65,                       /* Exp  */
  YYSYMBOL_Args = 66                       /* Args  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   380

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  42
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  25
/* YYNRULES -- Number of rules.  */
#define YYNRULES  79
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  147

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   296


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
      35,    36,    37,    38,    39,    40,    41
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    41,    41,    41,    51,    52,    65,    66,    67,    71,
      72,    81,    82,    83,    95,    96,    98,    99,   101,   108,
     109,   110,   114,   115,   119,   120,   124,   145,   148,   149,
     152,   156,   157,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   181,   182,   185,   188,   189,   195,   196,   197,
     200,   201,   202,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   233,   234
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INTEGER", "FLOAT",
  "TYPE", "CONST", "STRUCT", "RETURN", "IF", "ELSE", "WHILE", "ID",
  "SPACE", "SEMI", "COMMA", "ASSIGNOP", "BREAK", "CONTINUE", "GREAT",
  "GREATEQUAL", "LESS", "LESSEQUAL", "NOTEQUAL", "EQUAL", "PLUS", "MINUS",
  "STAR", "DIV", "MOD", "AND", "OR", "DOT", "NOT", "LP", "RP", "LB", "RB",
  "LC", "RC", "AERROR", "EOL", "$accept", "Program", "ExtDefList",
  "ExtDef", "ExtDecList", "Specifire", "StructSpecifire", "OptTag", "Tag",
  "VarDec", "FunDec", "VarList", "ParamDec", "Compst", "TACList", "TAC",
  "StmtList", "Stmt", "DefList", "Def", "DecList", "Dec", "InitList",
  "Exp", "Args", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-121)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-30)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      32,  -121,     8,    11,    14,  -121,    32,    52,  -121,  -121,
     -14,    19,  -121,  -121,  -121,    -1,  -121,    46,   -10,    23,
      50,    32,    24,  -121,    84,   111,     2,  -121,    57,    57,
      35,    32,  -121,    57,    42,    68,  -121,  -121,    45,   127,
     127,   127,   127,   100,   290,  -121,   180,    47,     2,    64,
    -121,  -121,    70,    76,  -121,  -121,    49,  -121,    32,   116,
     344,   344,     0,   236,   100,   -11,   290,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,    80,   127,  -121,  -121,  -121,   127,    59,    60,  -121,
      81,    87,  -121,  -121,    64,   143,  -121,    57,  -121,  -121,
     199,    72,  -121,    90,   100,  -121,   290,   338,   338,   338,
     338,   338,   338,   344,   344,     0,     0,     0,   326,   308,
    -121,   217,   162,   127,   127,  -121,  -121,  -121,  -121,  -121,
     127,  -121,   100,    96,  -121,  -121,   254,   272,  -121,    -5,
      64,    64,  -121,   103,  -121,    64,  -121
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,    11,     0,    17,     0,     3,     5,     0,    12,    13,
      18,     0,    15,     1,     4,    19,     7,     0,    47,     0,
       9,    43,     0,     6,     0,     0,    43,     8,     0,     0,
       0,    43,    23,     0,     0,    25,    76,    77,    75,     0,
       0,     0,     0,     0,    48,    21,     0,     0,    43,    32,
      19,    10,     0,    46,    14,    42,    26,    22,     0,     0,
      69,    68,    70,     0,     0,     0,    52,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    20,    27,    28,     0,     0,     0,    34,
       0,     0,    35,    30,    32,     0,    44,     0,    24,    72,
      79,     0,    67,     0,     0,    49,    53,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    54,    55,
      74,     0,     0,     0,     0,    37,    38,    31,    33,    45,
       0,    71,     0,    50,    73,    36,     0,     0,    78,     0,
       0,     0,    51,    39,    41,     0,    40
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -121,  -121,   110,  -121,    93,     5,  -121,  -121,  -121,    91,
    -121,    69,  -121,   113,    92,  -121,    41,  -120,    -9,  -121,
      58,   -26,   -62,   -24,    13
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     4,     5,     6,    17,    29,     8,    11,    12,    18,
      19,    34,    35,    92,    47,    48,    93,    94,    49,    31,
      52,    20,    65,    95,   101
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      44,    46,   103,    53,   104,     7,    24,     1,     2,     3,
     104,     7,    30,     9,    13,    60,    61,    62,    63,    66,
     143,   144,    55,    10,   -16,   146,    25,    33,   105,     1,
       2,     3,    81,    22,   142,   100,    82,     1,     2,     3,
      66,   -29,   133,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,    21,   121,    32,
      23,    26,   122,    33,    15,    28,    16,    36,    37,    50,
     139,    53,    86,    87,    54,    88,    38,    57,    89,    59,
      66,    90,    91,    58,    96,    25,    84,    36,    37,    39,
      40,    97,   120,   123,   124,   125,    38,    41,    42,   136,
     137,   126,    26,    36,    37,   132,   100,   131,    66,    39,
      40,   104,    38,   145,    36,    37,    14,    41,    42,    36,
      37,    51,    43,    38,    56,    39,    40,    98,    38,     0,
      36,    37,    27,    41,    42,   127,    39,    40,    64,    38,
      85,    39,    40,   138,    41,    42,     0,     0,    45,    41,
      42,    99,    39,    40,     0,   129,     0,   128,     0,    67,
      41,    42,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,   135,     0,    67,    82,
       0,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,     0,    67,     0,    82,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,     0,   130,    67,    82,    83,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,     0,    67,     0,    82,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
       0,     0,    67,    82,   134,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,     0,
      67,   102,    82,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,     0,    67,   140,
      82,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,     0,    67,   141,    82,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,     0,     0,     0,    82,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,     0,
      81,     0,     0,     0,    82,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,     0,     0,    81,     0,
       0,     0,    82,    74,    75,    76,    77,    78,     0,     0,
      81,    76,    77,    78,    82,     0,    81,     0,     0,     0,
      82
};

static const yytype_int16 yycheck[] =
{
      24,    25,    64,    29,    15,     0,    16,     5,     6,     7,
      15,     6,    21,     5,     0,    39,    40,    41,    42,    43,
     140,   141,    31,    12,    38,   145,    36,    22,    39,     5,
       6,     7,    32,    34,    39,    59,    36,     5,     6,     7,
      64,    39,   104,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    38,    82,    35,
      14,    38,    86,    58,    12,    15,    14,     3,     4,    12,
     132,    97,     8,     9,    39,    11,    12,    35,    14,    34,
     104,    17,    18,    15,    14,    36,    39,     3,     4,    25,
      26,    15,    12,    34,    34,    14,    12,    33,    34,   123,
     124,    14,    38,     3,     4,    15,   130,    35,   132,    25,
      26,    15,    12,    10,     3,     4,     6,    33,    34,     3,
       4,    28,    38,    12,    33,    25,    26,    58,    12,    -1,
       3,     4,    19,    33,    34,    94,    25,    26,    38,    12,
      48,    25,    26,   130,    33,    34,    -1,    -1,    37,    33,
      34,    35,    25,    26,    -1,    97,    -1,    14,    -1,    16,
      33,    34,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    14,    -1,    16,    36,
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    16,    -1,    36,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    15,    16,    36,    37,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    16,    -1,    36,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,    16,    36,    37,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      16,    35,    36,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    16,    35,
      36,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    16,    35,    36,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    36,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
      32,    -1,    -1,    -1,    36,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    -1,    -1,    32,    -1,
      -1,    -1,    36,    25,    26,    27,    28,    29,    -1,    -1,
      32,    27,    28,    29,    36,    -1,    32,    -1,    -1,    -1,
      36
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     5,     6,     7,    43,    44,    45,    47,    48,     5,
      12,    49,    50,     0,    44,    12,    14,    46,    51,    52,
      63,    38,    34,    14,    16,    36,    38,    55,    15,    47,
      60,    61,    35,    47,    53,    54,     3,     4,    12,    25,
      26,    33,    34,    38,    65,    37,    65,    56,    57,    60,
      12,    46,    62,    63,    39,    60,    51,    35,    15,    34,
      65,    65,    65,    65,    38,    64,    65,    16,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    36,    37,    39,    56,     8,     9,    11,    14,
      17,    18,    55,    58,    59,    65,    14,    15,    53,    35,
      65,    66,    35,    64,    15,    39,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      12,    65,    65,    34,    34,    14,    14,    58,    14,    62,
      15,    35,    15,    64,    37,    14,    65,    65,    66,    64,
      35,    35,    39,    59,    59,    10,    59
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    42,    43,    43,    44,    44,    45,    45,    45,    46,
      46,    47,    47,    47,    48,    48,    49,    49,    50,    51,
      51,    51,    52,    52,    53,    53,    54,    55,    56,    56,
      57,    58,    58,    59,    59,    59,    59,    59,    59,    59,
      59,    59,    60,    60,    61,    62,    62,    63,    63,    63,
      64,    64,    64,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    66,    66
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     1,     2,     0,     3,     2,     3,     1,
       3,     1,     1,     2,     5,     2,     1,     0,     1,     1,
       4,     3,     4,     3,     3,     1,     2,     3,     2,     0,
       2,     2,     0,     2,     1,     1,     3,     2,     2,     5,
       7,     5,     2,     0,     3,     3,     1,     1,     3,     5,
       3,     5,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     4,     3,     4,     3,     1,     1,     1,     3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
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
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
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
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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
  case 3: /* Program: ExtDefList  */
#line 41 "gramtree.y"
                    {
    (yyval.a)=newast("Program",1,(yyvsp[0].a));
    
    eval_print((yyval.a),0);    

    // printf("\nprint over\n");

    eval((yyval.a));
}
#line 1284 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 4: /* ExtDefList: ExtDef ExtDefList  */
#line 51 "gramtree.y"
                             {(yyval.a)=newast("ExtDefList",2,(yyvsp[-1].a),(yyvsp[0].a));}
#line 1290 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 5: /* ExtDefList: %empty  */
#line 52 "gramtree.y"
          {(yyval.a)=newast("ExtDefList",0,-1);}
#line 1296 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 6: /* ExtDef: Specifire ExtDecList SEMI  */
#line 65 "gramtree.y"
                                    {(yyval.a)=newast("ExtDef",2,(yyvsp[-2].a),(yyvsp[-1].a));}
#line 1302 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 7: /* ExtDef: Specifire SEMI  */
#line 66 "gramtree.y"
                        {(yyval.a)=newast("ExtDef",1,(yyvsp[-1].a));}
#line 1308 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 8: /* ExtDef: Specifire FunDec Compst  */
#line 67 "gramtree.y"
                                        {(yyval.a)=newast("ExtDef",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1314 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 9: /* ExtDecList: Dec  */
#line 71 "gramtree.y"
               {(yyval.a)=newast("ExtDecList",1,(yyvsp[0].a));}
#line 1320 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 10: /* ExtDecList: Dec COMMA ExtDecList  */
#line 72 "gramtree.y"
                              {(yyval.a)=newast("ExtDecList",2,(yyvsp[-2].a),(yyvsp[0].a));}
#line 1326 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 11: /* Specifire: TYPE  */
#line 81 "gramtree.y"
               {(yyval.a)=newast("Specifire",1,(yyvsp[0].a));}
#line 1332 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 12: /* Specifire: StructSpecifire  */
#line 82 "gramtree.y"
                         {(yyval.a)=newast("Specifire",1,(yyvsp[0].a));}
#line 1338 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 13: /* Specifire: CONST TYPE  */
#line 83 "gramtree.y"
                {(yyval.a)=newast("Specifire",2,(yyvsp[0].a),(yyvsp[-1].a));}
#line 1344 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 14: /* StructSpecifire: STRUCT OptTag LC DefList RC  */
#line 95 "gramtree.y"
                                            {(yyval.a)=newast("StructSpecifire",5,(yyvsp[-4].a),(yyvsp[-3].a),(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1350 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 15: /* StructSpecifire: STRUCT Tag  */
#line 96 "gramtree.y"
                    {(yyval.a)=newast("StructSpecifire",2,(yyvsp[-1].a),(yyvsp[0].a));}
#line 1356 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 16: /* OptTag: ID  */
#line 98 "gramtree.y"
          {(yyval.a)=newast("OptTag",1,(yyvsp[0].a));}
#line 1362 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 17: /* OptTag: %empty  */
#line 99 "gramtree.y"
         {(yyval.a)=newast("OptTag",0,-1);}
#line 1368 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 18: /* Tag: ID  */
#line 101 "gramtree.y"
       {(yyval.a)=newast("Tag",1,(yyvsp[0].a));}
#line 1374 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 19: /* VarDec: ID  */
#line 108 "gramtree.y"
          {(yyval.a)=newast("VarDec",1,(yyvsp[0].a));}
#line 1380 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 20: /* VarDec: VarDec LB Exp RB  */
#line 109 "gramtree.y"
                           {(yyval.a)=newast("VarDec",3,(yyvsp[-3].a),(yyvsp[-2].a),(yyvsp[-1].a));}
#line 1386 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 21: /* VarDec: VarDec LB RB  */
#line 110 "gramtree.y"
                       {(yyval.a)=newast("VarDec",2,(yyvsp[-2].a),(yyvsp[-1].a));}
#line 1392 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 22: /* FunDec: ID LP VarList RP  */
#line 114 "gramtree.y"
                        {(yyval.a)=newast("FunDec",4,(yyvsp[-3].a),(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1398 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 23: /* FunDec: ID LP RP  */
#line 115 "gramtree.y"
                  {(yyval.a)=newast("FunDec",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1404 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 24: /* VarList: ParamDec COMMA VarList  */
#line 119 "gramtree.y"
                               {(yyval.a)=newast("VarList",2,(yyvsp[-2].a),(yyvsp[0].a));}
#line 1410 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 25: /* VarList: ParamDec  */
#line 120 "gramtree.y"
                  {(yyval.a)=newast("VarList",1,(yyvsp[0].a));}
#line 1416 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 26: /* ParamDec: Specifire VarDec  */
#line 124 "gramtree.y"
                          {(yyval.a)=newast("ParamDec",2,(yyvsp[-1].a),(yyvsp[0].a));}
#line 1422 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 27: /* Compst: LC TACList RC  */
#line 145 "gramtree.y"
                     {(yyval.a)=newast("Compst",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1428 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 28: /* TACList: TAC TACList  */
#line 148 "gramtree.y"
                    {(yyval.a)=newast("TACList",2,(yyvsp[-1].a),(yyvsp[0].a));}
#line 1434 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 29: /* TACList: %empty  */
#line 149 "gramtree.y"
      {(yyval.a)=newast("TACList",0,-1);}
#line 1440 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 30: /* TAC: DefList StmtList  */
#line 152 "gramtree.y"
                     {(yyval.a)=newast("TAC",2,(yyvsp[-1].a),(yyvsp[0].a));}
#line 1446 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 31: /* StmtList: Stmt StmtList  */
#line 156 "gramtree.y"
                      {(yyval.a)=newast("StmtList",2,(yyvsp[-1].a),(yyvsp[0].a));}
#line 1452 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 32: /* StmtList: %empty  */
#line 157 "gramtree.y"
          {(yyval.a)=newast("StmtList",0,-1);}
#line 1458 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 33: /* Stmt: Exp SEMI  */
#line 166 "gramtree.y"
              {(yyval.a)=newast("Stmt",1,(yyvsp[-1].a));}
#line 1464 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 35: /* Stmt: Compst  */
#line 168 "gramtree.y"
            {(yyval.a)=newast("Stmt",1,(yyvsp[0].a));}
#line 1470 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 36: /* Stmt: RETURN Exp SEMI  */
#line 169 "gramtree.y"
                           {(yyval.a)=newast("Stmt",2,(yyvsp[-2].a),(yyvsp[-1].a));}
#line 1476 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 37: /* Stmt: BREAK SEMI  */
#line 170 "gramtree.y"
               {(yyval.a)=newast("Stmt",1,(yyvsp[-1].a));}
#line 1482 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 38: /* Stmt: CONTINUE SEMI  */
#line 171 "gramtree.y"
                  {(yyval.a)=newast("Stmt",1,(yyvsp[-1].a));}
#line 1488 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 39: /* Stmt: IF LP Exp RP Stmt  */
#line 172 "gramtree.y"
                       {(yyval.a)=newast("Stmt",5,(yyvsp[-4].a),(yyvsp[-2].a),newast("assistIF",0,-1),(yyvsp[0].a),newast("assistELSE",0,-1));}
#line 1494 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 40: /* Stmt: IF LP Exp RP Stmt ELSE Stmt  */
#line 173 "gramtree.y"
                                 {(yyval.a)=newast("Stmt",7,(yyvsp[-6].a),(yyvsp[-4].a),newast("assistIF",0,-1),(yyvsp[-2].a),newast("assistELSE",0,-1),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1500 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 41: /* Stmt: WHILE LP Exp RP Stmt  */
#line 174 "gramtree.y"
                          {(yyval.a)=newast("Stmt",4,(yyvsp[-4].a),(yyvsp[-2].a),newast("assistWHILE",0,-1),(yyvsp[0].a));}
#line 1506 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 42: /* DefList: Def DefList  */
#line 181 "gramtree.y"
                   {(yyval.a)=newast("DefList",2,(yyvsp[-1].a),(yyvsp[0].a));}
#line 1512 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 43: /* DefList: %empty  */
#line 182 "gramtree.y"
          {(yyval.a)=newast("DefList",0,-1);}
#line 1518 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 44: /* Def: Specifire DecList SEMI  */
#line 185 "gramtree.y"
                           {(yyval.a)=newast("Def",2,(yyvsp[-2].a),(yyvsp[-1].a));}
#line 1524 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 45: /* DecList: Dec COMMA DecList  */
#line 188 "gramtree.y"
                          {(yyval.a)=newast("DecList",2,(yyvsp[-2].a),(yyvsp[0].a));}
#line 1530 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 46: /* DecList: Dec  */
#line 189 "gramtree.y"
         {(yyval.a)=newast("DecList",1,(yyvsp[0].a));}
#line 1536 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 47: /* Dec: VarDec  */
#line 195 "gramtree.y"
           {(yyval.a)=newast("Dec",1,(yyvsp[0].a));}
#line 1542 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 48: /* Dec: VarDec ASSIGNOP Exp  */
#line 196 "gramtree.y"
                             {(yyval.a)=newast("Dec",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1548 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 49: /* Dec: VarDec ASSIGNOP LC InitList RC  */
#line 197 "gramtree.y"
                                       {(yyval.a)=newast("Dec",3,(yyvsp[-4].a),(yyvsp[-3].a),(yyvsp[-1].a));}
#line 1554 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 50: /* InitList: InitList COMMA InitList  */
#line 200 "gramtree.y"
                                 {(yyval.a)=newast("InitList",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1560 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 51: /* InitList: LC InitList COMMA InitList RC  */
#line 201 "gramtree.y"
                                     {(yyval.a)=newast("InitList",2,(yyvsp[-3].a),(yyvsp[-1].a));}
#line 1566 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 52: /* InitList: Exp  */
#line 202 "gramtree.y"
           {(yyval.a)=newast("InitList",1,(yyvsp[0].a));}
#line 1572 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 53: /* Exp: Exp ASSIGNOP Exp  */
#line 205 "gramtree.y"
                    {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1578 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 54: /* Exp: Exp AND Exp  */
#line 206 "gramtree.y"
                    {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1584 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 55: /* Exp: Exp OR Exp  */
#line 207 "gramtree.y"
                   {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1590 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 56: /* Exp: Exp GREAT Exp  */
#line 208 "gramtree.y"
                      {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1596 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 57: /* Exp: Exp GREATEQUAL Exp  */
#line 209 "gramtree.y"
                           {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1602 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 58: /* Exp: Exp LESS Exp  */
#line 210 "gramtree.y"
                     {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1608 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 59: /* Exp: Exp LESSEQUAL Exp  */
#line 211 "gramtree.y"
                          {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1614 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 60: /* Exp: Exp NOTEQUAL Exp  */
#line 212 "gramtree.y"
                         {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1620 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 61: /* Exp: Exp EQUAL Exp  */
#line 213 "gramtree.y"
                      {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1626 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 62: /* Exp: Exp PLUS Exp  */
#line 214 "gramtree.y"
                     {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1632 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 63: /* Exp: Exp MINUS Exp  */
#line 215 "gramtree.y"
                      {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1638 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 64: /* Exp: Exp STAR Exp  */
#line 216 "gramtree.y"
                     {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1644 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 65: /* Exp: Exp DIV Exp  */
#line 217 "gramtree.y"
                    {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1650 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 66: /* Exp: Exp MOD Exp  */
#line 218 "gramtree.y"
                    {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1656 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 67: /* Exp: LP Exp RP  */
#line 219 "gramtree.y"
                  {(yyval.a)=newast("Exp",1,(yyvsp[-1].a));}
#line 1662 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 68: /* Exp: MINUS Exp  */
#line 220 "gramtree.y"
                              {(yyval.a)=newast("Exp",2,(yyvsp[-1].a),(yyvsp[0].a));}
#line 1668 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 69: /* Exp: PLUS Exp  */
#line 221 "gramtree.y"
                            {(yyval.a)=newast("Exp",2,(yyvsp[-1].a),(yyvsp[0].a));}
#line 1674 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 70: /* Exp: NOT Exp  */
#line 222 "gramtree.y"
                 {(yyval.a)=newast("Exp",2,(yyvsp[-1].a),(yyvsp[0].a));}
#line 1680 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 71: /* Exp: ID LP Args RP  */
#line 223 "gramtree.y"
                       {(yyval.a)=newast("Exp",3,newast("assistFuncCall",0,-1),(yyvsp[-3].a),(yyvsp[-1].a));}
#line 1686 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 72: /* Exp: ID LP RP  */
#line 224 "gramtree.y"
                  {(yyval.a)=newast("Exp",2,newast("assistFuncCall",0,-1),(yyvsp[-2].a));}
#line 1692 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 73: /* Exp: Exp LB Exp RB  */
#line 225 "gramtree.y"
                       {(yyval.a)=newast("Exp",4,(yyvsp[-3].a),(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1698 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 74: /* Exp: Exp DOT ID  */
#line 226 "gramtree.y"
                    {(yyval.a)=newast("Exp",3,(yyvsp[-2].a),(yyvsp[-1].a),(yyvsp[0].a));}
#line 1704 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 75: /* Exp: ID  */
#line 227 "gramtree.y"
            {(yyval.a)=newast("Exp",1,(yyvsp[0].a));}
#line 1710 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 76: /* Exp: INTEGER  */
#line 228 "gramtree.y"
                 {(yyval.a)=newast("Exp",1,(yyvsp[0].a));}
#line 1716 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 77: /* Exp: FLOAT  */
#line 229 "gramtree.y"
              {(yyval.a)=newast("Exp",1,(yyvsp[0].a));}
#line 1722 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 78: /* Args: Exp COMMA Args  */
#line 233 "gramtree.y"
                    {(yyval.a)=newast("Args",3,(yyvsp[-2].a),newast("assistArgs",0,-1),(yyvsp[0].a));}
#line 1728 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;

  case 79: /* Args: Exp  */
#line 234 "gramtree.y"
             {(yyval.a)=newast("Args",2,(yyvsp[0].a),newast("assistArgs",0,-1));}
#line 1734 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"
    break;


#line 1738 "/home/ryuk/project/llvm_demo/llvm_demo/build/gramtree.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 236 "gramtree.y"



