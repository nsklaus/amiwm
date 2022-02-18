/* A Bison parser, made by GNU Bison 3.7.6.  */

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
#define YYBISON 30706

/* Bison version string.  */
#define YYBISON_VERSION "3.7.6"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "gram.y"

#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "prefs.h"
#include "drawinfo.h"
#include "screen.h"
#include "icc.h"
#include "style.h"
extern void set_sys_palette(void);
extern void set_mwb_palette(void);
extern void set_schwartz_palette(void);
extern void set_custom_palette(char *fn);
extern void add_toolitem(char *, char *, char *, int);
extern Scrn *openscreen(char *, Window);
extern void create_module(Scrn *, char *, char *);
extern char *default_colors[NUMDRIPENS];
extern char *default_screenfont, *label_font_name;
extern Display *dpy;
#ifndef HAVE_ALLOCA
#define alloca malloc
#endif
#ifndef HAVE_STRDUP
char *strdup(s) char *s;
{
  char *ptr=(char *)malloc(strlen(s)+1);
  if(ptr) strcpy(ptr, s);
  return ptr;
}
#endif

static void append_to(char **x, char *y)
{
  if(*x==NULL)
    *x=y;
  else {
    char *t = malloc(strlen(y)+strlen(*x)+2);
    sprintf(t, "%s\n%s", *x, y);
    free(*x);
    free(y);
    *x=t;
  }
}

static int ti_level=0;

#line 119 "y.tab.c"

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

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ERRORTOKEN = 258,              /* ERRORTOKEN  */
    LEFTBRACE = 259,               /* LEFTBRACE  */
    RIGHTBRACE = 260,              /* RIGHTBRACE  */
    YES = 261,                     /* YES  */
    NO = 262,                      /* NO  */
    RIGHT = 263,                   /* RIGHT  */
    BOTTOM = 264,                  /* BOTTOM  */
    BOTH = 265,                    /* BOTH  */
    NONE = 266,                    /* NONE  */
    MAGICWB = 267,                 /* MAGICWB  */
    SYSTEM = 268,                  /* SYSTEM  */
    SCHWARTZ = 269,                /* SCHWARTZ  */
    ALWAYS = 270,                  /* ALWAYS  */
    AUTO = 271,                    /* AUTO  */
    MANUAL = 272,                  /* MANUAL  */
    SEPARATOR = 273,               /* SEPARATOR  */
    T_DETAILPEN = 274,             /* T_DETAILPEN  */
    T_BLOCKPEN = 275,              /* T_BLOCKPEN  */
    T_TEXTPEN = 276,               /* T_TEXTPEN  */
    T_SHINEPEN = 277,              /* T_SHINEPEN  */
    T_SHADOWPEN = 278,             /* T_SHADOWPEN  */
    T_FILLPEN = 279,               /* T_FILLPEN  */
    T_FILLTEXTPEN = 280,           /* T_FILLTEXTPEN  */
    T_BACKGROUNDPEN = 281,         /* T_BACKGROUNDPEN  */
    T_HIGHLIGHTTEXTPEN = 282,      /* T_HIGHLIGHTTEXTPEN  */
    T_BARDETAILPEN = 283,          /* T_BARDETAILPEN  */
    T_BARBLOCKPEN = 284,           /* T_BARBLOCKPEN  */
    T_BARTRIMPEN = 285,            /* T_BARTRIMPEN  */
    FASTQUIT = 286,                /* FASTQUIT  */
    SIZEBORDER = 287,              /* SIZEBORDER  */
    DEFAULTICON = 288,             /* DEFAULTICON  */
    ICONDIR = 289,                 /* ICONDIR  */
    ICONPALETTE = 290,             /* ICONPALETTE  */
    SCREENFONT = 291,              /* SCREENFONT  */
    ICONFONT = 292,                /* ICONFONT  */
    TOOLITEM = 293,                /* TOOLITEM  */
    FORCEMOVE = 294,               /* FORCEMOVE  */
    SCREEN = 295,                  /* SCREEN  */
    MODULE = 296,                  /* MODULE  */
    MODULEPATH = 297,              /* MODULEPATH  */
    INTERSCREENGAP = 298,          /* INTERSCREENGAP  */
    AUTORAISE = 299,               /* AUTORAISE  */
    FOCUS = 300,                   /* FOCUS  */
    FOLLOWMOUSE = 301,             /* FOLLOWMOUSE  */
    CLICKTOTYPE = 302,             /* CLICKTOTYPE  */
    SLOPPY = 303,                  /* SLOPPY  */
    CUSTOMICONSONLY = 304,         /* CUSTOMICONSONLY  */
    TITLEBARCLOCK = 305,           /* TITLEBARCLOCK  */
    TITLECLOCKFORMAT = 306,        /* TITLECLOCKFORMAT  */
    OPAQUEMOVE = 307,              /* OPAQUEMOVE  */
    OPAQUERESIZE = 308,            /* OPAQUERESIZE  */
    SCREENMENU = 309,              /* SCREENMENU  */
    STYLE = 310,                   /* STYLE  */
    CLASS = 311,                   /* CLASS  */
    TITLE = 312,                   /* TITLE  */
    ICONTITLE = 313,               /* ICONTITLE  */
    ICON = 314,                    /* ICON  */
    SHORTLABELICONS = 315,         /* SHORTLABELICONS  */
    STRING = 316,                  /* STRING  */
    NUMBER = 317                   /* NUMBER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 50 "gram.y"

    int num;
    char *ptr;

#line 236 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ERRORTOKEN = 3,                 /* ERRORTOKEN  */
  YYSYMBOL_LEFTBRACE = 4,                  /* LEFTBRACE  */
  YYSYMBOL_RIGHTBRACE = 5,                 /* RIGHTBRACE  */
  YYSYMBOL_YES = 6,                        /* YES  */
  YYSYMBOL_NO = 7,                         /* NO  */
  YYSYMBOL_RIGHT = 8,                      /* RIGHT  */
  YYSYMBOL_BOTTOM = 9,                     /* BOTTOM  */
  YYSYMBOL_BOTH = 10,                      /* BOTH  */
  YYSYMBOL_NONE = 11,                      /* NONE  */
  YYSYMBOL_MAGICWB = 12,                   /* MAGICWB  */
  YYSYMBOL_SYSTEM = 13,                    /* SYSTEM  */
  YYSYMBOL_SCHWARTZ = 14,                  /* SCHWARTZ  */
  YYSYMBOL_ALWAYS = 15,                    /* ALWAYS  */
  YYSYMBOL_AUTO = 16,                      /* AUTO  */
  YYSYMBOL_MANUAL = 17,                    /* MANUAL  */
  YYSYMBOL_SEPARATOR = 18,                 /* SEPARATOR  */
  YYSYMBOL_T_DETAILPEN = 19,               /* T_DETAILPEN  */
  YYSYMBOL_T_BLOCKPEN = 20,                /* T_BLOCKPEN  */
  YYSYMBOL_T_TEXTPEN = 21,                 /* T_TEXTPEN  */
  YYSYMBOL_T_SHINEPEN = 22,                /* T_SHINEPEN  */
  YYSYMBOL_T_SHADOWPEN = 23,               /* T_SHADOWPEN  */
  YYSYMBOL_T_FILLPEN = 24,                 /* T_FILLPEN  */
  YYSYMBOL_T_FILLTEXTPEN = 25,             /* T_FILLTEXTPEN  */
  YYSYMBOL_T_BACKGROUNDPEN = 26,           /* T_BACKGROUNDPEN  */
  YYSYMBOL_T_HIGHLIGHTTEXTPEN = 27,        /* T_HIGHLIGHTTEXTPEN  */
  YYSYMBOL_T_BARDETAILPEN = 28,            /* T_BARDETAILPEN  */
  YYSYMBOL_T_BARBLOCKPEN = 29,             /* T_BARBLOCKPEN  */
  YYSYMBOL_T_BARTRIMPEN = 30,              /* T_BARTRIMPEN  */
  YYSYMBOL_FASTQUIT = 31,                  /* FASTQUIT  */
  YYSYMBOL_SIZEBORDER = 32,                /* SIZEBORDER  */
  YYSYMBOL_DEFAULTICON = 33,               /* DEFAULTICON  */
  YYSYMBOL_ICONDIR = 34,                   /* ICONDIR  */
  YYSYMBOL_ICONPALETTE = 35,               /* ICONPALETTE  */
  YYSYMBOL_SCREENFONT = 36,                /* SCREENFONT  */
  YYSYMBOL_ICONFONT = 37,                  /* ICONFONT  */
  YYSYMBOL_TOOLITEM = 38,                  /* TOOLITEM  */
  YYSYMBOL_FORCEMOVE = 39,                 /* FORCEMOVE  */
  YYSYMBOL_SCREEN = 40,                    /* SCREEN  */
  YYSYMBOL_MODULE = 41,                    /* MODULE  */
  YYSYMBOL_MODULEPATH = 42,                /* MODULEPATH  */
  YYSYMBOL_INTERSCREENGAP = 43,            /* INTERSCREENGAP  */
  YYSYMBOL_AUTORAISE = 44,                 /* AUTORAISE  */
  YYSYMBOL_FOCUS = 45,                     /* FOCUS  */
  YYSYMBOL_FOLLOWMOUSE = 46,               /* FOLLOWMOUSE  */
  YYSYMBOL_CLICKTOTYPE = 47,               /* CLICKTOTYPE  */
  YYSYMBOL_SLOPPY = 48,                    /* SLOPPY  */
  YYSYMBOL_CUSTOMICONSONLY = 49,           /* CUSTOMICONSONLY  */
  YYSYMBOL_TITLEBARCLOCK = 50,             /* TITLEBARCLOCK  */
  YYSYMBOL_TITLECLOCKFORMAT = 51,          /* TITLECLOCKFORMAT  */
  YYSYMBOL_OPAQUEMOVE = 52,                /* OPAQUEMOVE  */
  YYSYMBOL_OPAQUERESIZE = 53,              /* OPAQUERESIZE  */
  YYSYMBOL_SCREENMENU = 54,                /* SCREENMENU  */
  YYSYMBOL_STYLE = 55,                     /* STYLE  */
  YYSYMBOL_CLASS = 56,                     /* CLASS  */
  YYSYMBOL_TITLE = 57,                     /* TITLE  */
  YYSYMBOL_ICONTITLE = 58,                 /* ICONTITLE  */
  YYSYMBOL_ICON = 59,                      /* ICON  */
  YYSYMBOL_SHORTLABELICONS = 60,           /* SHORTLABELICONS  */
  YYSYMBOL_STRING = 61,                    /* STRING  */
  YYSYMBOL_NUMBER = 62,                    /* NUMBER  */
  YYSYMBOL_YYACCEPT = 63,                  /* $accept  */
  YYSYMBOL_amiwmrc = 64,                   /* amiwmrc  */
  YYSYMBOL_stmts = 65,                     /* stmts  */
  YYSYMBOL_stmt = 66,                      /* stmt  */
  YYSYMBOL_toolsubmenu = 67,               /* toolsubmenu  */
  YYSYMBOL_toolitems = 68,                 /* toolitems  */
  YYSYMBOL_toolitem = 69,                  /* toolitem  */
  YYSYMBOL_stylespec = 70,                 /* stylespec  */
  YYSYMBOL_styleitems = 71,                /* styleitems  */
  YYSYMBOL_styleitem = 72,                 /* styleitem  */
  YYSYMBOL_string = 73,                    /* string  */
  YYSYMBOL_truth = 74,                     /* truth  */
  YYSYMBOL_sizeborder = 75,                /* sizeborder  */
  YYSYMBOL_focuspolicy = 76,               /* focuspolicy  */
  YYSYMBOL_dri_pen = 77,                   /* dri_pen  */
  YYSYMBOL_forcemove_policy = 78           /* forcemove_policy  */
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
typedef yytype_int8 yy_state_t;

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

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   126

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  63
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  16
/* YYNRULES -- Number of rules.  */
#define YYNRULES  75
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  111

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   317


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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    81,    81,    84,    85,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   120,   121,   124,   127,   128,
     131,   132,   133,   136,   149,   150,   153,   154,   155,   156,
     159,   162,   163,   166,   167,   168,   169,   170,   173,   174,
     175,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   192,   193,   194
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
  "\"end of file\"", "error", "\"invalid token\"", "ERRORTOKEN",
  "LEFTBRACE", "RIGHTBRACE", "YES", "NO", "RIGHT", "BOTTOM", "BOTH",
  "NONE", "MAGICWB", "SYSTEM", "SCHWARTZ", "ALWAYS", "AUTO", "MANUAL",
  "SEPARATOR", "T_DETAILPEN", "T_BLOCKPEN", "T_TEXTPEN", "T_SHINEPEN",
  "T_SHADOWPEN", "T_FILLPEN", "T_FILLTEXTPEN", "T_BACKGROUNDPEN",
  "T_HIGHLIGHTTEXTPEN", "T_BARDETAILPEN", "T_BARBLOCKPEN", "T_BARTRIMPEN",
  "FASTQUIT", "SIZEBORDER", "DEFAULTICON", "ICONDIR", "ICONPALETTE",
  "SCREENFONT", "ICONFONT", "TOOLITEM", "FORCEMOVE", "SCREEN", "MODULE",
  "MODULEPATH", "INTERSCREENGAP", "AUTORAISE", "FOCUS", "FOLLOWMOUSE",
  "CLICKTOTYPE", "SLOPPY", "CUSTOMICONSONLY", "TITLEBARCLOCK",
  "TITLECLOCKFORMAT", "OPAQUEMOVE", "OPAQUERESIZE", "SCREENMENU", "STYLE",
  "CLASS", "TITLE", "ICONTITLE", "ICON", "SHORTLABELICONS", "STRING",
  "NUMBER", "$accept", "amiwmrc", "stmts", "stmt", "toolsubmenu",
  "toolitems", "toolitem", "stylespec", "styleitems", "styleitem",
  "string", "truth", "sizeborder", "focuspolicy", "dri_pen",
  "forcemove_policy", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317
};
#endif

#define YYPACT_NINF (-46)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-3)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -46,    14,    66,   -46,   -46,   -46,   -46,   -46,   -46,   -46,
     -46,   -46,   -46,   -46,   -46,   -46,   -46,    33,    24,   -45,
     -45,    -1,   -45,   -45,   -12,     3,   -20,   -45,   -45,   -40,
      33,   -10,    33,    33,   -17,    33,    33,    33,    25,    33,
     -46,   -46,   -46,   -46,   -45,   -46,   -46,   -46,   -46,   -46,
     -46,   -46,   -46,   -46,   -46,   -46,   -46,   -46,   -46,   -46,
     -46,   -46,   -46,   -46,    -2,   -46,   -46,   -46,   -46,   -45,
     -46,   -15,   -46,   -46,   -46,   -46,   -46,   -46,   -46,   -46,
     -46,   -45,   -46,   -46,   -46,   -46,   -46,   -46,     5,    12,
     -46,   -46,   -45,   -46,   -46,   -46,   -46,   -12,   -46,   -46,
     -45,   -45,   -45,   -45,   -46,   -46,   -45,   -46,   -46,   -46,
     -46
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       4,     0,     0,     1,     5,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       3,    39,     6,    45,     0,    51,    52,     8,    57,    53,
      54,    55,    56,     9,    50,    10,    11,    13,    12,    14,
      15,    17,    18,    42,     0,    73,    74,    75,    19,     0,
      20,    24,    22,    25,    26,    58,    60,    59,    29,    30,
      32,     0,    33,    27,    28,    35,    43,    31,     0,     0,
      16,    37,    40,    21,    23,    34,     7,     0,    38,    36,
       0,     0,     0,     0,    44,    41,     0,    46,    47,    48,
      49
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -46,   -46,   -46,   -46,   -46,   -46,   -41,   -46,   -46,   -46,
     -19,    -9,   -46,   -46,   -46,   -46
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     2,    40,    41,    88,    42,    43,    89,   104,
      92,    47,    53,    78,    44,    68
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      55,    56,    91,    61,    62,    64,    63,    70,    71,    72,
      96,    57,    58,    59,     3,    82,    54,    99,    65,    66,
      67,    74,    73,    79,    80,    90,    83,    84,    85,    86,
      87,    48,    49,    50,    51,    52,    75,    76,    77,    45,
      46,    54,    69,    97,    54,    81,    94,    98,     0,    54,
      93,     0,     0,     0,     0,     0,     0,     0,     0,    54,
      60,     0,    95,     0,     0,     0,    -2,     4,   100,   101,
     102,   103,     0,   105,     0,     0,     0,     0,   106,     0,
       0,   107,   108,   109,   110,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,     0,     0,     0,    32,    33,    34,    35,    36,
      37,    38,     0,     0,     0,     0,    39
};

static const yytype_int8 yycheck[] =
{
      19,    20,     4,    22,    23,    24,    18,    26,    27,    28,
       5,    12,    13,    14,     0,    34,    61,     5,    15,    16,
      17,    30,    62,    32,    33,    44,    35,    36,    37,     4,
      39,     7,     8,     9,    10,    11,    46,    47,    48,     6,
       7,    61,    62,    38,    61,    62,    61,    88,    -1,    61,
      69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      61,    -1,    81,    -1,    -1,    -1,     0,     1,    56,    57,
      58,    59,    -1,    92,    -1,    -1,    -1,    -1,    97,    -1,
      -1,   100,   101,   102,   103,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    -1,    -1,    -1,    -1,    60
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    64,    65,     0,     1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    49,    50,    51,    52,    53,    54,    55,    60,
      66,    67,    69,    70,    77,     6,     7,    74,     7,     8,
       9,    10,    11,    75,    61,    73,    73,    12,    13,    14,
      61,    73,    73,    18,    73,    15,    16,    17,    78,    62,
      73,    73,    73,    62,    74,    46,    47,    48,    76,    74,
      74,    62,    73,    74,    74,    74,     4,    74,    68,    71,
      73,     4,    73,    73,    61,    73,     5,    38,    69,     5,
      56,    57,    58,    59,    72,    73,    73,    73,    73,    73,
      73
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    63,    64,    65,    65,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    67,    68,    68,
      69,    69,    69,    70,    71,    71,    72,    72,    72,    72,
      73,    74,    74,    75,    75,    75,    75,    75,    76,    76,
      76,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    78,    78,    78
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     0,     1,     1,     3,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     2,     3,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     2,     3,     3,     2,     0,
       3,     4,     2,     2,     2,     0,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


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
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
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
    goto yyexhaustedlab;
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
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
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
  case 7: /* stmt: toolsubmenu toolitems RIGHTBRACE  */
#line 90 "gram.y"
                                                   { ti_level=0; }
#line 1390 "y.tab.c"
    break;

  case 8: /* stmt: FASTQUIT truth  */
#line 91 "gram.y"
                                 { prefs.fastquit = (yyvsp[0].num); }
#line 1396 "y.tab.c"
    break;

  case 9: /* stmt: SIZEBORDER sizeborder  */
#line 92 "gram.y"
                                        { prefs.sizeborder = (yyvsp[0].num); }
#line 1402 "y.tab.c"
    break;

  case 10: /* stmt: DEFAULTICON string  */
#line 93 "gram.y"
                                     { prefs.defaulticon = (yyvsp[0].ptr); }
#line 1408 "y.tab.c"
    break;

  case 11: /* stmt: ICONDIR string  */
#line 94 "gram.y"
                                 { prefs.icondir = (yyvsp[0].ptr); }
#line 1414 "y.tab.c"
    break;

  case 12: /* stmt: ICONPALETTE SYSTEM  */
#line 95 "gram.y"
                                     { set_sys_palette(); }
#line 1420 "y.tab.c"
    break;

  case 13: /* stmt: ICONPALETTE MAGICWB  */
#line 96 "gram.y"
                                      { set_mwb_palette(); }
#line 1426 "y.tab.c"
    break;

  case 14: /* stmt: ICONPALETTE SCHWARTZ  */
#line 97 "gram.y"
                                       { set_schwartz_palette(); }
#line 1432 "y.tab.c"
    break;

  case 15: /* stmt: ICONPALETTE STRING  */
#line 98 "gram.y"
                                     { set_custom_palette((yyvsp[0].ptr)); }
#line 1438 "y.tab.c"
    break;

  case 16: /* stmt: dri_pen string  */
#line 99 "gram.y"
                                 { default_colors[(yyvsp[-1].num)] = (yyvsp[0].ptr); }
#line 1444 "y.tab.c"
    break;

  case 17: /* stmt: SCREENFONT string  */
#line 100 "gram.y"
                                    { default_screenfont = (yyvsp[0].ptr); }
#line 1450 "y.tab.c"
    break;

  case 18: /* stmt: ICONFONT string  */
#line 101 "gram.y"
                                  { label_font_name = (yyvsp[0].ptr); }
#line 1456 "y.tab.c"
    break;

  case 19: /* stmt: FORCEMOVE forcemove_policy  */
#line 102 "gram.y"
                                             { prefs.forcemove = (yyvsp[0].num); }
#line 1462 "y.tab.c"
    break;

  case 20: /* stmt: SCREEN string  */
#line 103 "gram.y"
                                { openscreen((yyvsp[0].ptr),DefaultRootWindow(dpy)); }
#line 1468 "y.tab.c"
    break;

  case 21: /* stmt: SCREEN NUMBER string  */
#line 104 "gram.y"
                                       { if(((yyvsp[-1].num)==DefaultScreen(dpy)||prefs.manage_all) && (yyvsp[-1].num)<ScreenCount(dpy)) openscreen((yyvsp[0].ptr),RootWindow(dpy,(yyvsp[-1].num))); }
#line 1474 "y.tab.c"
    break;

  case 22: /* stmt: MODULEPATH string  */
#line 105 "gram.y"
                                    { prefs.module_path = (yyvsp[0].ptr); }
#line 1480 "y.tab.c"
    break;

  case 23: /* stmt: MODULE string STRING  */
#line 106 "gram.y"
                                       { create_module((front? front->upfront:NULL), (yyvsp[-1].ptr), (yyvsp[0].ptr)); }
#line 1486 "y.tab.c"
    break;

  case 24: /* stmt: MODULE string  */
#line 107 "gram.y"
                                { create_module((front? front->upfront:NULL), (yyvsp[0].ptr), NULL); }
#line 1492 "y.tab.c"
    break;

  case 25: /* stmt: INTERSCREENGAP NUMBER  */
#line 108 "gram.y"
                                        { prefs.borderwidth=(yyvsp[0].num); }
#line 1498 "y.tab.c"
    break;

  case 26: /* stmt: AUTORAISE truth  */
#line 109 "gram.y"
                                  { prefs.autoraise=(yyvsp[0].num); }
#line 1504 "y.tab.c"
    break;

  case 27: /* stmt: OPAQUEMOVE truth  */
#line 110 "gram.y"
                                   { prefs.opaquemove=(yyvsp[0].num); }
#line 1510 "y.tab.c"
    break;

  case 28: /* stmt: OPAQUERESIZE truth  */
#line 111 "gram.y"
                                     { prefs.opaqueresize=(yyvsp[0].num); }
#line 1516 "y.tab.c"
    break;

  case 29: /* stmt: FOCUS focuspolicy  */
#line 112 "gram.y"
                                    { prefs.focus=(yyvsp[0].num); }
#line 1522 "y.tab.c"
    break;

  case 30: /* stmt: CUSTOMICONSONLY truth  */
#line 113 "gram.y"
                                        { prefs.customiconsonly = (yyvsp[0].num); }
#line 1528 "y.tab.c"
    break;

  case 31: /* stmt: SHORTLABELICONS truth  */
#line 114 "gram.y"
                                        { prefs.shortlabelicons = (yyvsp[0].num); }
#line 1534 "y.tab.c"
    break;

  case 32: /* stmt: TITLEBARCLOCK truth  */
#line 115 "gram.y"
                                      { prefs.titlebarclock = (yyvsp[0].num); }
#line 1540 "y.tab.c"
    break;

  case 33: /* stmt: TITLECLOCKFORMAT string  */
#line 116 "gram.y"
                                          { prefs.titleclockformat = (yyvsp[0].ptr); }
#line 1546 "y.tab.c"
    break;

  case 34: /* stmt: TITLECLOCKFORMAT NUMBER string  */
#line 117 "gram.y"
                                                 { 
					prefs.titleclockinterval=(yyvsp[-1].num); 
					prefs.titleclockformat=(yyvsp[0].ptr); }
#line 1554 "y.tab.c"
    break;

  case 35: /* stmt: SCREENMENU truth  */
#line 120 "gram.y"
                                   { prefs.screenmenu=(yyvsp[0].num); }
#line 1560 "y.tab.c"
    break;

  case 37: /* toolsubmenu: TOOLITEM string LEFTBRACE  */
#line 124 "gram.y"
                                            { add_toolitem((yyvsp[-1].ptr), NULL, NULL, -1); ti_level=1; }
#line 1566 "y.tab.c"
    break;

  case 40: /* toolitem: TOOLITEM string string  */
#line 131 "gram.y"
                                         { add_toolitem((yyvsp[-1].ptr), (yyvsp[0].ptr), NULL, ti_level); }
#line 1572 "y.tab.c"
    break;

  case 41: /* toolitem: TOOLITEM string string string  */
#line 132 "gram.y"
                                                { add_toolitem((yyvsp[-2].ptr), (yyvsp[-1].ptr), (yyvsp[0].ptr), ti_level); }
#line 1578 "y.tab.c"
    break;

  case 42: /* toolitem: TOOLITEM SEPARATOR  */
#line 133 "gram.y"
                                     { add_toolitem(NULL, NULL, NULL, ti_level); }
#line 1584 "y.tab.c"
    break;

  case 43: /* stylespec: STYLE LEFTBRACE  */
#line 136 "gram.y"
                                  { Style *s = malloc(sizeof(Style));
				    memset(s, 0, sizeof(*s));
				    s->next=NULL; s->style_class=NULL;
				    s->style_title=s->style_icon_title=NULL;
				    s->icon_name=NULL;
				    s->icon_pms.cs.colors=NULL;
				    s->icon_pms.cs2.colors=NULL;
				    if(prefs.firststyle)
				      prefs.laststyle->next=s;
				    else
				      prefs.firststyle=s;
				    prefs.laststyle=s; }
#line 1601 "y.tab.c"
    break;

  case 46: /* styleitem: CLASS string  */
#line 153 "gram.y"
                               { append_to(&prefs.laststyle->style_class, (yyvsp[0].ptr));}
#line 1607 "y.tab.c"
    break;

  case 47: /* styleitem: TITLE string  */
#line 154 "gram.y"
                               { append_to(&prefs.laststyle->style_title, (yyvsp[0].ptr));}
#line 1613 "y.tab.c"
    break;

  case 48: /* styleitem: ICONTITLE string  */
#line 155 "gram.y"
                                   { append_to(&prefs.laststyle->style_icon_title, (yyvsp[0].ptr));}
#line 1619 "y.tab.c"
    break;

  case 49: /* styleitem: ICON string  */
#line 156 "gram.y"
                              { prefs.laststyle->icon_name=(yyvsp[0].ptr); }
#line 1625 "y.tab.c"
    break;

  case 50: /* string: STRING  */
#line 159 "gram.y"
                         { (yyval.ptr) = strdup((yyvsp[0].ptr)); }
#line 1631 "y.tab.c"
    break;

  case 51: /* truth: YES  */
#line 162 "gram.y"
                      { (yyval.num) = True; }
#line 1637 "y.tab.c"
    break;

  case 52: /* truth: NO  */
#line 163 "gram.y"
                     { (yyval.num) = False; }
#line 1643 "y.tab.c"
    break;

  case 53: /* sizeborder: RIGHT  */
#line 166 "gram.y"
                        { (yyval.num) = Psizeright; }
#line 1649 "y.tab.c"
    break;

  case 54: /* sizeborder: BOTTOM  */
#line 167 "gram.y"
                         { (yyval.num) = Psizebottom; }
#line 1655 "y.tab.c"
    break;

  case 55: /* sizeborder: BOTH  */
#line 168 "gram.y"
                       { (yyval.num) = Psizeright|Psizebottom; }
#line 1661 "y.tab.c"
    break;

  case 56: /* sizeborder: NONE  */
#line 169 "gram.y"
                       { (yyval.num) = Psizetrans; }
#line 1667 "y.tab.c"
    break;

  case 57: /* sizeborder: NO  */
#line 170 "gram.y"
                     { (yyval.num) = Psizetrans; }
#line 1673 "y.tab.c"
    break;

  case 58: /* focuspolicy: FOLLOWMOUSE  */
#line 173 "gram.y"
                              { (yyval.num) = FOC_FOLLOWMOUSE; }
#line 1679 "y.tab.c"
    break;

  case 59: /* focuspolicy: SLOPPY  */
#line 174 "gram.y"
                         { (yyval.num) = FOC_SLOPPY; }
#line 1685 "y.tab.c"
    break;

  case 60: /* focuspolicy: CLICKTOTYPE  */
#line 175 "gram.y"
                              { (yyval.num) = FOC_CLICKTOTYPE; }
#line 1691 "y.tab.c"
    break;

  case 61: /* dri_pen: T_DETAILPEN  */
#line 178 "gram.y"
                              { (yyval.num) = DETAILPEN; }
#line 1697 "y.tab.c"
    break;

  case 62: /* dri_pen: T_BLOCKPEN  */
#line 179 "gram.y"
                             { (yyval.num) = BLOCKPEN; }
#line 1703 "y.tab.c"
    break;

  case 63: /* dri_pen: T_TEXTPEN  */
#line 180 "gram.y"
                            { (yyval.num) = TEXTPEN; }
#line 1709 "y.tab.c"
    break;

  case 64: /* dri_pen: T_SHINEPEN  */
#line 181 "gram.y"
                             { (yyval.num) = SHINEPEN; }
#line 1715 "y.tab.c"
    break;

  case 65: /* dri_pen: T_SHADOWPEN  */
#line 182 "gram.y"
                              { (yyval.num) = SHADOWPEN; }
#line 1721 "y.tab.c"
    break;

  case 66: /* dri_pen: T_FILLPEN  */
#line 183 "gram.y"
                            { (yyval.num) = FILLPEN; }
#line 1727 "y.tab.c"
    break;

  case 67: /* dri_pen: T_FILLTEXTPEN  */
#line 184 "gram.y"
                                { (yyval.num) = FILLTEXTPEN; }
#line 1733 "y.tab.c"
    break;

  case 68: /* dri_pen: T_BACKGROUNDPEN  */
#line 185 "gram.y"
                                  { (yyval.num) = BACKGROUNDPEN; }
#line 1739 "y.tab.c"
    break;

  case 69: /* dri_pen: T_HIGHLIGHTTEXTPEN  */
#line 186 "gram.y"
                                     { (yyval.num) = HIGHLIGHTTEXTPEN; }
#line 1745 "y.tab.c"
    break;

  case 70: /* dri_pen: T_BARDETAILPEN  */
#line 187 "gram.y"
                                 { (yyval.num) = BARDETAILPEN; }
#line 1751 "y.tab.c"
    break;

  case 71: /* dri_pen: T_BARBLOCKPEN  */
#line 188 "gram.y"
                                { (yyval.num) = BARBLOCKPEN; }
#line 1757 "y.tab.c"
    break;

  case 72: /* dri_pen: T_BARTRIMPEN  */
#line 189 "gram.y"
                               { (yyval.num) = BARTRIMPEN; }
#line 1763 "y.tab.c"
    break;

  case 73: /* forcemove_policy: ALWAYS  */
#line 192 "gram.y"
                          { (yyval.num) = FM_ALWAYS; }
#line 1769 "y.tab.c"
    break;

  case 74: /* forcemove_policy: AUTO  */
#line 193 "gram.y"
                        { (yyval.num) = FM_AUTO; }
#line 1775 "y.tab.c"
    break;

  case 75: /* forcemove_policy: MANUAL  */
#line 194 "gram.y"
                          { (yyval.num) = FM_MANUAL; }
#line 1781 "y.tab.c"
    break;


#line 1785 "y.tab.c"

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
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
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
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 197 "gram.y"

extern char *progname;
extern int ParseError;
int yyerror(s) char *s;
{
    fprintf (stderr, "%s: error in input file:  %s\n", progname, s ? s : "");
    ParseError = 1;
    return 0;
}
