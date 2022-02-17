/* A Bison parser, made by GNU Bison 3.7.6.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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

#line 131 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
