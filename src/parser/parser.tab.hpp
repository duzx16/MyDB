
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

/* Line 1676 of yacc.c  */
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



/* Line 1676 of yacc.c  */
#line 128 "C:/Users/wyfcy/Desktop/MyDB/src/parser/parser.tab.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


