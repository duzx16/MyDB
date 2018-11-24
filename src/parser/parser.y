%{
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#include <cstdio>
#define __STRICT_ANSI__
#else
#include <cstdio>
#endif

#include <stdlib.h>
#include <vector>
#include "../Constants.h"
#include "Tree.h"

int yylex();
void yyerror(const char *);

class Tree;
%}

%union {
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
    WhereClauseTree *whereClauseTree;
    ConditionsTree *conditionsTree;
    Comparison *comparisonTree;
    ConstValueList *constValuesTree;
    ConstValueNode *constValueTree;
    ColumnDecsList *columnsTree;
    ColumnNode *columnTree;
    ConstValueLists *insertValueTree;
    SetClauseList *setClauseList;
}

/* keyword */
%token SELECT DELETE UPDATE INSERT
%token CREATE DROP USE SHOW TABLES
%token DATABASES DATABASE TABLE
%token STAR FROM WHERE OPERATOR VALUES SET INTO

/* COLUMN DESCPRITION */
%token KINT KCHAR KFLOAT KVARCHAR KDATE PRIMARY FOREIGN REFERENCES


/* number */
%token <ivalue> INTEGER
%token <fvalue> FLOAT
%token <string> STRING IDENTIFIER

/* operator */
%token EQ GT LT GE LE NE

/* aggretation */
%token AVG SUM MIN MAX

%token NOTNULL DESC GROUP LIKE INDEX CHECK IN T_NULL IS AND

%type <tree> command
%type <ivalue> type_width
%type <attributesTree> attributes
%type <attributeTree> attribute
%type <identList> tableList
%type <whereClauseTree> whereclause
%type <conditionsTree> conditions
%type <comparisonTree> comparison
%type <constValuesTree> constvalues
%type <constValueTree> constvalue
%type <columnsTree> column_decs
%type <columnTree> column
%type <insertValueTree> valueLists
%type <setClauseList> setList
%type <attrType> column_type
%type <ivalue> column_constraints column_constraint
%type <identList> column_list;
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
    | CREATE DATABASE IDENTIFIER
            {
                $$ = new CreateDatabase($3);
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
    | USE IDENTIFIER
            {
                $$ = new UseDatabase($2);
                Tree::setInstance($$);
                delete $2;
                Tree::run();
            }
    | SHOW TABLES
            {
                $$ = new DescTable("");
                Tree::setInstance($$);
                Tree::run();
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
    | UPDATE IDENTIFIER SET setList whereclause
            {
                $$ = new Update($2, $4, $5);
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
    | CREATE TABLE IDENTIFIER '(' column_decs ')'
            {
                $$ = new CreateTable($3, $5);
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
    ;

valueLists:
    '(' constvalues ')'
            {
                $$ = new ConstValueLists();
                $$->addConstValues($2);
            }
    | valueLists ',' '(' constvalues ')'
            {
                $$->addConstValues($4);
            }
    ;

setList:
    attribute EQ constvalue
            {
                $$ = new SetClauseList();
                $$->addSetClause($1, $3);
            }
    | setList ',' attribute EQ constvalue
            {
                $$->addSetClause($3, $5);
            }

column:
    IDENTIFIER column_type type_width column_constraints
            {
                $$ = new ColumnNode($1, $2, $3, $4);
                delete $1;

            }
    | FOREIGN '(' IDENTIFIER ')' REFERENCES IDENTIFIER '(' IDENTIFIER ')'
            {
                //$$ = new ColumnNode($2, $3, $4);
            }
    | PRIMARY '(' column_list ')'
            {
                //$$ = new ColumnNode($3);
            }
    ;

column_list:
    IDENTIFIER
            {
                $$ = new IdentList();
                $$->addIdent($1);
                delete $1;
            }
    | column_list ',' IDENTIFIER
            {
                $$->addIdent($3);
                delete $3;
            }

column_type:
    KINT {$$=AttrType::INT;}
    | KCHAR  {$$=AttrType::STRING;}
    | KVARCHAR  {$$=AttrType::VARCHAR;}
    | KFLOAT {$$=AttrType::FLOAT;}
    | KDATE {$$=AttrType::DATE;}
    ;

type_width:
    '(' INTEGER ')' {$$ = $2;}
    | {$$=0;}
    ;

column_constraints: column_constraints column_constraint {$$ = $1 | $2;}
            | {$$=0;}
            ;

column_constraint: NOTNULL {$$ = COLUMN_FLAG_NOTNULL;}
            ;


column_decs:
    column
            {
                $$ = new ColumnDecsList();
                $$->addColumn($1);
            }
    | column_decs ',' column
            {
                $$->addColumn($3);
            }
    ;

attributes:
    attribute
            {
                $$ = new AttributeList();
                $$->addAttribute($1);
            }
    | attributes ',' attribute
            {
                $$->addAttribute($3);
            }
    ;

attribute:
    IDENTIFIER
            {
                $$ = new AttributeNode($1);
                delete $1;
            }
    | SUM '(' IDENTIFIER ')'
            {
                $$ =  new AttributeNode($3, AggregationType::T_SUM);
                delete $3;
            }
    | AVG '(' IDENTIFIER ')'
            {
                $$ =  new AttributeNode($3, AggregationType::T_AVG);
                delete $3;
            }
    | MIN '(' IDENTIFIER ')'
            {
                $$ =  new AttributeNode($3, AggregationType::T_MIN);
                delete $3;
            }
    | MAX '(' IDENTIFIER ')'
            {
                $$ =  new AttributeNode($3, AggregationType::T_MAX);
                delete $3;
            }
    | IDENTIFIER '.' IDENTIFIER
            {
                $$ = new AttributeNode($1, $3);
                delete $1;
                delete $3;
            }
    | SUM '(' IDENTIFIER '.' IDENTIFIER ')'
            {
                $$ = new AttributeNode($3, $5, AggregationType::T_SUM);
                delete $3;
                delete $5;
            }
    | AVG '(' IDENTIFIER '.' IDENTIFIER ')'
            {
                $$ = new AttributeNode($3, $5, AggregationType::T_AVG);
                delete $3;
                delete $5;
            }
    | MAX '(' IDENTIFIER '.' IDENTIFIER ')'
            {
                $$ = new AttributeNode($3, $5, AggregationType::T_MAX);
                delete $3;
                delete $5;
            }
    | MIN '(' IDENTIFIER '.' IDENTIFIER ')'
            {
                $$ = new AttributeNode($3, $5, AggregationType::T_MIN);
                delete $3;
                delete $5;
            }
    ;

tableList:
    IDENTIFIER
            {
                $$ = new IdentList();
                $$->addIdent($1);
                delete $1;
            }
    | tableList ',' IDENTIFIER
            {
                $$->addIdent($3);
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
                $$ = new Comparison($1, CompOp::EQ_OP, $3);
            }
    | attribute GT constvalue
            {
                $$ = new Comparison($1, CompOp::GT_OP, $3);
            }
    | attribute LT constvalue
            {
                $$ = new Comparison($1, CompOp::LT_OP, $3);
            }
    | attribute GE constvalue
            {
                $$ = new Comparison($1, CompOp::GE_OP, $3);
            }
    | attribute LE constvalue
            {
                $$ = new Comparison($1, CompOp::LE_OP, $3);
            }
    | attribute NE constvalue
            {
                $$ = new Comparison($1, CompOp::NE_OP, $3);
            }
    | attribute LIKE constvalue
            {
                $$ = new Comparison($1, CompOp::LIKE_OP, $3);
            }
    | attribute IS T_NULL
            {
                $$ = new Comparison($1);
            }
    | attribute EQ attribute
            {
                $$ = new Comparison($1, CompOp::EQ_OP, $3);
            }
    | attribute GT attribute
            {
                $$ = new Comparison($1, CompOp::GT_OP, $3);
            }
    | attribute LT attribute
            {
                $$ = new Comparison($1, CompOp::LT_OP, $3);
            }
    | attribute GE attribute
            {
                $$ = new Comparison($1, CompOp::GE_OP, $3);
            }
    | attribute LE attribute
            {
                $$ = new Comparison($1, CompOp::LE_OP, $3);
            }
    | attribute NE attribute
            {
                $$ = new Comparison($1, CompOp::NE_OP, $3);
            }
    ;

constvalues:
    constvalue
            {
                $$ = new ConstValueList();
                $$->addConstValue($1);
            }
    | constvalues ',' constvalue
            {
                $$->addConstValue($3);
            }
    ;

constvalue:
    INTEGER         { $$ = new ConstValueNode($1); }
    | FLOAT         { $$ = new ConstValueNode($1); }
    | STRING        { $$ = new ConstValueNode($1); delete $1; }
    | T_NULL          { $$ = new ConstValueNode(0); $$->setNull(); }
    ;

%%

void yyerror(const char *msg) {
    printf("YACC error: %s\n", msg);
}
