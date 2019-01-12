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
#include "parser.yy.cpp"

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
    ConstValueList *constValuesTree;
    ColumnDecsList *columnsTree;
    ColumnNode *columnTree;
    ConstValueLists *insertValueTree;
    SetClauseList *setClauseList;
    TableConstraint *tbOptDec;
    TableConstraintList *tbOptDecList;
    Expr * expr;
}

/* keyword */
%token QUIT
%token SELECT DELETE UPDATE INSERT
%token CREATE DROP USE SHOW TABLES
%token DATABASES DATABASE TABLE
%token FROM WHERE OPERATOR VALUES SET INTO

/* COLUMN DESCPRITION */
%token KINT KCHAR KFLOAT KVARCHAR KDATE PRIMARY FOREIGN REFERENCES


/* number */
%token <ivalue> INTEGER
%token <fvalue> FLOAT
%token <string> STRING IDENTIFIER DATE_FORMAT

/* operator */
%token EQ GT LT GE LE NE

/* aggretation */
%token AVG SUM MIN MAX

%token NOTNULL DESC GROUP LIKE INDEX CHECK IN T_NULL IS AND OR

%type <tree> command
%type <ivalue> type_width
%type <attributesTree> attributes
%type <attributeTree> attribute
%type <identList> tableList column_list column_list_exist
%type <constValuesTree> constvalues
%type <columnsTree> column_decs
%type <columnTree> column_dec
%type <insertValueTree> valueLists
%type <setClauseList> setList
%type <attrType> column_type
%type <ivalue> column_constraints column_constraint
%type <tbOptDec> tb_opt_dec
%type <tbOptDecList> tb_opt_exist tb_opt_decs
%type <expr> expr factor value comparison constvalue whereclause conditions

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
    | QUIT
            {
                YYACCEPT;
            }
    | SHOW DATABASES
            {
                $$ = new ShowDatabase("");
                Tree::setInstance($$);
                Tree::run();
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
    | SHOW DATABASE IDENTIFIER
            {
                $$ = new ShowDatabase($3);
                Tree::setInstance($$);
                Tree::run();
            }
    | SHOW TABLES
            {
                $$ = new DescTable("");
                Tree::setInstance($$);
                Tree::run();
            }
    | SHOW TABLE IDENTIFIER
            {
                $$ = new DescTable($3);
                Tree::setInstance($$);
                Tree::run();
            }
    | SELECT '*' FROM tableList whereclause
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
    | INSERT INTO IDENTIFIER column_list_exist VALUES valueLists
            {
                $$ = new Insert($3, $4, $6);
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
    | CREATE TABLE IDENTIFIER '(' column_decs tb_opt_exist ')'
            {
                $$ = new CreateTable($3, $5, $6);
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
    attribute EQ expr
            {
                $$ = new SetClauseList();
                $$->addSetClause($1, $3);
            }
    | setList ',' attribute EQ expr
            {
                $$->addSetClause($3, $5);
            }
    ;

column_decs:
    column_dec
            {
                $$ = new ColumnDecsList();
                $$->addColumn($1);
            }
    | column_decs ',' column_dec
            {
                $$->addColumn($3);
            }
    ;

column_dec:
    IDENTIFIER column_type type_width column_constraints
            {
                $$ = new ColumnNode($1, $2, $3, $4);
                delete $1;

            }
    | KDATE column_type type_width column_constraints
            {
                $$ = new ColumnNode("date", $2, $3, $4);
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

column_list_exist:
    '(' column_list ')'
            {
                $$ = $2;
            }
    |       {   $$ = NULL;   }

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

tb_opt_exist:
    ',' tb_opt_decs {$$ = $2;}
    | {$$ = NULL;}
    ;

tb_opt_decs:
    tb_opt_dec
            {
                $$ = new TableConstraintList();
                $$->addTbDec($1);
            }
    | tb_opt_decs ',' tb_opt_dec
            {
                $$->addTbDec($3);
            }

tb_opt_dec:
    FOREIGN '(' IDENTIFIER ')' REFERENCES IDENTIFIER '(' IDENTIFIER ')'
            {
                $$ = new TableConstraint($3, $6, $8);
                delete $3;
                delete $6;
                delete $8;
            }
    | PRIMARY '(' column_list ')'
            {
                $$ = new TableConstraint($3);
            }
    | CHECK '(' IDENTIFIER IN '(' constvalues ')' ')'
            {
                $$ = new TableConstraint($3, $6);
            }

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
    | KDATE
            {
                $$ = new AttributeNode("date");
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
                $$ = $2;
            }
    |
            {
                $$ = new Expr(true);
            }
    ;

conditions:
    comparison
            {
                $$ = $1;
            }
    | conditions AND comparison
            {
                $$ = new Expr($1, LogicOp::AND_OP, $3);
            }
    | conditions OR comparison
            {
                $$ = new Expr($1, LogicOp::OR_OP, $3);
            }
    ;

comparison:
    expr EQ expr
            {
                $$ = new Expr($1, CompOp::EQ_OP, $3);
            }
    | expr GT expr
            {
                $$ = new Expr($1, CompOp::GT_OP, $3);
            }
    | expr LT expr
            {
                $$ = new Expr($1, CompOp::LT_OP, $3);
            }
    | expr GE expr
            {
                $$ = new Expr($1, CompOp::GE_OP, $3);
            }
    | expr LE expr
            {
                $$ = new Expr($1, CompOp::LE_OP, $3);
            }
    | expr NE expr
            {
                $$ = new Expr($1, CompOp::NE_OP, $3);
            }
    | expr LIKE expr
            {
                $$ = new Expr($1, CompOp::LIKE_OP, $3);
            }
    | expr IS T_NULL
            {
                $$ = new Expr($1, CompOp::IS_OP, new Expr());
            }
    | expr IS NOTNULL
            {
                $$ = new Expr($1, CompOp::ISNOT_OP, new Expr());
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

expr:   expr '+' factor {
            $$ = new Expr($1, ArithOp::ADD_OP, $3);
        }
    |   expr '-' factor{
            $$ = new Expr($1, ArithOp::SUB_OP, $3);
        }
    |   factor {$$=$1;}
    ;

factor: factor '*' value {
            $$ = new Expr($1, ArithOp::MUL_OP, $3);
        }
    |  factor '/' value {
           $$ = new Expr($1, ArithOp::DIV_OP, $3);
        }
    | value {$$=$1;}
    ;

value:
    constvalue      {$$ = $1;}
    | attribute     {
        $$ = new Expr($1);
    }
    | '-' value     {
        $$ = new Expr($2, ArithOp::MINUS_OP);
    }
    | '(' expr ')'  {
        $$ = $2;
    }
    ;

constvalue:
    INTEGER         { $$ = new Expr($1); }
    | FLOAT         { $$ = new Expr($1); }
    | STRING        { $$ = new Expr($1); delete $1; }
    | DATE_FORMAT   { $$ = new Expr($1, true); delete $1; }
    | T_NULL        {
        $$ = new Expr();
    }

%%

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

