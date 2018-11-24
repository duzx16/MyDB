%{
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

int yylex();
void yyerror(const char *);

class Tree;
%}

%union {
    int ivalue;
    float fvalue;
    char *string;
    Tree *tree;
    Select *selectTree;
    Delete *deleteTree;
    AttributesTree *attributesTree;
    AttributeTree *attributeTree;
    RelationsTree *tableListTree;
    WhereClauseTree *whereClauseTree;
    ConditionsTree *conditionsTree;
    ComparisonTree *comparisonTree;
    ConstValuesTree *constValuesTree;
    ConstValueTree *constValueTree;
    ColumnsTree *columnsTree;
    ColumnTree *columnTree;
    InsertValueTree *insertValueTree;
}

/* keyword */
%token SELECT DELETE UPDATE INSERT
%token CREATE DROP USE SHOW TABLES
%token DATABASES DATABASE TABLE
%token STAR FROM WHERE OPERATOR VALUES SET INTO

/* COLUMN DESCPRITION */
%token KINT KFLOAT KVARCHAR


/* number */
%token <ivalue> INTEGER
%token <fvalue> FLOAT
%token <string> STRING IDENTIFIER

/* operator */
%token EQ GT LT GE LE NE

/* aggretation */
%token AVG SUM MIN MAX

%token NOTNULL PRIMARY DESC GROUP LIKE INDEX CHECK IN T_NULL IS AND

%type <tree> command
%type <attributesTree> attributes attrList
%type <attributeTree> attribute
%type <tableListTree> tableList
%type <whereClauseTree> whereclause
%type <conditionsTree> conditions
%type <comparisonTree> comparison
%type <constValuesTree> constvalues
%type <constValueTree> constvalue
%type <columnsTree> columns
%type <columnsTree> columnsTmp
%type <columnTree> column
%type <columnTree> columnInt
%type <insertValueTree> valueLists

%%

sql:
    command
            {

            }
    | sql ';' command
            {

            }
    ;

command:
            {

            }
    | SHOW DATABASES
            {

            }
    | SELECT STAR FROM tableList whereclause
            {
                $$ = new Select($4, $5);
                Tree::setInstance($$);
                Tree::run();
            }
    | SELECT attributes FROM tableList whereclause
            {
                $$ = new Select($2, $4, $5);
                Tree::setInstance($$);
                Tree::run();
            }
    | SELECT attributes FROM tableList whereclause GROUP IDENTIFIER
            {
                $$ = new Select($2, $4, $5, $7);
                delete $7;
                Tree::setInstance($$);
                Tree::run();
            }
    | INSERT INTO IDENTIFIER VALUES valueLists
            {
                $$ = new Insert($3, $5);
                Tree::setInstance($$);
                delete $3;
                Tree::run();
            }
    | UPDATE IDENTIFIER SET attribute EQ constvalue whereclause
            {
                $$ = new Update($2, $4, $6, $7);
                Tree::setInstance($$);
                delete $2;
                Tree::run();
            }
    | DELETE FROM IDENTIFIER whereclause
            {
                $$ = new Delete($3, $4);
                Tree::setInstance($$);
                delete $3;
                Tree::run();
            }
    | CREATE DATABASE IDENTIFIER
            {
                $$ = new CreateDatabase($3);
                Tree::setInstance($$);
                delete $3;
                Tree::run();
            }
    | CREATE TABLE IDENTIFIER '(' columns ')'
            {
                $$ = new CreateTable($3, $5);
                Tree::setInstance($$);
                delete $3;
                Tree::run();
            }
    | DROP DATABASE IDENTIFIER
            {
                $$ = new DropDatabase($3);
                Tree::setInstance($$);
                delete $3;
                Tree::run();
            }
    | DROP TABLE IDENTIFIER
            {
                $$ = new DropTable($3);
                Tree::setInstance($$);
                delete $3;
                Tree::run();
            }
    | USE IDENTIFIER
            {
                $$ = new UseDatabase($2);
                Tree::setInstance($$);
                delete $2;
                Tree::run();
            }
    | DESC IDENTIFIER
            {
                $$ = new DescTable($2);
                Tree::setInstance($$);
                delete $2;
                Tree::run();
            }
    | CREATE INDEX IDENTIFIER '(' attribute ')'
            {
                $$ = new CreateIndex($3, $5);
                Tree::setInstance($$);
                delete $3;
                Tree::run();
            }
    | DROP INDEX IDENTIFIER '(' attribute ')'
            {
                $$ = new DropIndex($3, $5);
                Tree::setInstance($$);
                delete $3;
                Tree::run();
            }
    | DESC
            {
                $$ = new DescTable("");
                Tree::setInstance($$);
                Tree::run();
            }
    | SHOW TABLES
            {
                $$ = new DescTable("");
                Tree::setInstance($$);
                Tree::run();
            }
    ;

valueLists:
    '(' constvalues ')'
            {
                $$ = new InsertValueTree($2);
            }
    | valueLists ',' '(' constvalues ')'
            {
                $$->addConstValues($4);
            }
    ;

columns:
    columnsTmp
            {

            }
    | columnsTmp ',' PRIMARY '(' IDENTIFIER ')'
            {
                bool found = $$->setPrimaryKey($5);
                if(!found) {yyerror("Primary Key Set Failed!");}
                delete $5;
            }
    ;

columnsTmp:
    column
            {
                $$ = new ColumnsTree();
                $$->addColumn($1);
            }
    | columnsTmp ',' column
            {
                $$->addColumn($3);
            }
    ;



column:
    columnInt
            {
                $$ = $1;
            }
    | columnInt NOTNULL
            {
                $1->setNotNull(1);
                $$ = $1;
            }
    | IDENTIFIER KFLOAT
            {
                $$ = new ColumnTree($1, AttrType::FLOAT);
                delete $1;
            }
    | IDENTIFIER KFLOAT NOTNULL
            {
                $$ = new ColumnTree($1, AttrType::FLOAT, 4, 0, 1);
                delete $1;
            }
    | IDENTIFIER KVARCHAR '(' INTEGER ')'
            {
                $$ = new ColumnTree($1, AttrType::STRING, $4);
                delete $1;
            }
    | IDENTIFIER KVARCHAR '(' INTEGER ')' NOTNULL
            {
                $$ = new ColumnTree($1, AttrType::STRING, $4, 0, 1);
                delete $1;
            }
    ;

columnInt:
    IDENTIFIER KINT
            {
                $$ = new ColumnTree($1, AttrType::INT);
                delete $1;
            }
    | IDENTIFIER KINT '(' INTEGER ')'
            {
                // TODO
                $$ = new ColumnTree($1, AttrType::INT);
                delete $1;
            }
    ;

attributes:
    attrList
            {
                $$ = $1;
            }
    ;

attrList:
    attribute
            {
                $$ = new AttributesTree();
                $$->addAttribute($1);
            }
    | attrList ',' attribute
            {
                $$->addAttribute($3);
            }
    ;

attribute:
    IDENTIFIER
            {
                $$ = new AttributeTree($1);
                delete $1;
            }
    | SUM '(' IDENTIFIER ')'
            {
                $$ =  new AttributeTree($3, AggregationType::T_SUM);
                delete $3;
            }
    | AVG '(' IDENTIFIER ')'
            {
                $$ =  new AttributeTree($3, AggregationType::T_AVG);
                delete $3;
            }
    | MIN '(' IDENTIFIER ')'
            {
                $$ =  new AttributeTree($3, AggregationType::T_MIN);
                delete $3;
            }
    | MAX '(' IDENTIFIER ')'
            {
                $$ =  new AttributeTree($3, AggregationType::T_MAX);
                delete $3;
            }
    | IDENTIFIER '.' IDENTIFIER
            {
                $$ = new AttributeTree($1, $3);
                delete $1;
                delete $3;
            }
    | SUM '(' IDENTIFIER '.' IDENTIFIER ')'
            {
                $$ = new AttributeTree($3, $5, AggregationType::T_SUM);
                delete $3;
                delete $5;
            }
    | AVG '(' IDENTIFIER '.' IDENTIFIER ')'
            {
                $$ = new AttributeTree($3, $5, AggregationType::T_AVG);
                delete $3;
                delete $5;
            }
    | MAX '(' IDENTIFIER '.' IDENTIFIER ')'
            {
                $$ = new AttributeTree($3, $5, AggregationType::T_MAX);
                delete $3;
                delete $5;
            }
    | MIN '(' IDENTIFIER '.' IDENTIFIER ')'
            {
                $$ = new AttributeTree($3, $5, AggregationType::T_MIN);
                delete $3;
                delete $5;
            }
    ;

tableList:
    IDENTIFIER
            {
                $$ = new RelationsTree();
                $$->addRelation($1);
                delete $1;
            }
    | tableList ',' IDENTIFIER
            {
                $$->addRelation($3);
                delete $3;
            }
    ;

whereclause:
    WHERE conditions
            {
                $$ = new WhereClauseTree($2);
            }
    |
            {
                $$ = nullptr;
            }
    ;

conditions:
    comparison
            {
                $$ = new ConditionsTree();
                $$->addComparison($1);
            }
    | conditions AND comparison
            {
                $$->addComparison($3);
            }
    ;

comparison:
    attribute EQ constvalue
            {
                $$ = new ComparisonTree($1, CompOp::EQ_OP, $3);
            }
    | attribute GT constvalue
            {
                $$ = new ComparisonTree($1, CompOp::GT_OP, $3);
            }
    | attribute LT constvalue
            {
                $$ = new ComparisonTree($1, CompOp::LT_OP, $3);
            }
    | attribute GE constvalue
            {
                $$ = new ComparisonTree($1, CompOp::GE_OP, $3);
            }
    | attribute LE constvalue
            {
                $$ = new ComparisonTree($1, CompOp::LE_OP, $3);
            }
    | attribute NE constvalue
            {
                $$ = new ComparisonTree($1, CompOp::NE_OP, $3);
            }
    | attribute LIKE constvalue
            {
                $$ = new ComparisonTree($1, CompOp::LIKE_OP, $3);
            }
    | attribute IS T_NULL
            {
                $$ = new ComparisonTree($1);
            }
    | attribute EQ attribute
            {
                $$ = new ComparisonTree($1, CompOp::EQ_OP, $3);
            }
    | attribute GT attribute
            {
                $$ = new ComparisonTree($1, CompOp::GT_OP, $3);
            }
    | attribute LT attribute
            {
                $$ = new ComparisonTree($1, CompOp::LT_OP, $3);
            }
    | attribute GE attribute
            {
                $$ = new ComparisonTree($1, CompOp::GE_OP, $3);
            }
    | attribute LE attribute
            {
                $$ = new ComparisonTree($1, CompOp::LE_OP, $3);
            }
    | attribute NE attribute
            {
                $$ = new ComparisonTree($1, CompOp::NE_OP, $3);
            }
    ;

constvalues:
    constvalue
            {
                $$ = new ConstValuesTree();
                $$->addConstValue($1);
            }
    | constvalues ',' constvalue
            {
                $$->addConstValue($3);
            }
    ;

constvalue:
    INTEGER         { $$ = new ConstValueTree($1); }
    | FLOAT         { $$ = new ConstValueTree($1); }
    | STRING        { $$ = new ConstValueTree($1); delete $1; }
    | T_NULL          { $$ = new ConstValueTree(0); $$->setNull(); }
    ;

%%

void yyerror(const char *msg) {
    printf("YACC error: %s\n", msg);
}
