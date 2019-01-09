%option caseless

%{
#include "Tree.h"
#include "parser.tab.hpp"
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#include <cstdio>
#define __STRICT_ANSI__
#else
#include <cstdio>
#endif
#include <cstring>

char *get_text(const char *text, bool strip = false) {
    int length = strlen(text);
    char *buffer;
    if(not strip) {
        buffer = new char[length + 1];
        memcpy(buffer, text, (length + 1) * sizeof(char));
    } else {
        buffer = new char[length - 1];
        memcpy(buffer, text + 1, (length - 1) * sizeof(char));
        buffer[length - 2] = 0;
    }

    return buffer;
}

void yyerror(const char *);

%}


DIGIT           [0-9]
INTEGER         {DIGIT}+
FLOAT           {DIGIT}+\.{DIGIT}*
IDENTIFIER      [a-z][a-z0-9_]*
WHITESPACE      [ \t\n]
DATE_FORMAT     '[0-9]{4}-[0-9]{1,2}-[0-9]{1,2}'
STRING          '(\\\\|\\n|\\r|\\\"|\\0|\\'|[^'\\])*'

%%

","             { /*printf(", "); */ return yytext[0]; }
"("             { /*printf("( "); */ return yytext[0]; }
")"             { /*printf(") "); */ return yytext[0]; }
"."             { /*printf(". "); */ return yytext[0]; }
";"             { /*printf("; "); */ return yytext[0]; }
"+"             { /*printf("+ "); */ return yytext[0]; }
"-"             { /*printf("- "); */ return yytext[0]; }
"/"             { /*printf("/ "); */ return yytext[0]; }
"*"             { /*printf("* "); */ return yytext[0]; }
QUIT            { /*printf("QUIT");*/return QUIT;       }
CREATE          { /*printf("CREATE "); */ return CREATE; }
DROP            { /*printf("DROP "); */ return DROP; }
USE             { /*printf("USE "); */ return USE; }
SHOW            { /*printf("SHOW "); */ return SHOW; }
DATABASES       { /*printf("DATABASES "); */ return DATABASES; }
DATABASE        { /*printf("DATABASE "); */ return DATABASE; }
TABLE           { /*printf("TABLE "); */ return TABLE; }
TABLES          { /*printf("TABLES "); */ return TABLES; }
SELECT          { /*printf("SELECT "); */ return SELECT; }
DELETE          { /*printf("DELETE "); */ return DELETE; }
INSERT          { /*printf("INSERT "); */ return INSERT; }
UPDATE          { /*printf("UPDATE "); */ return UPDATE; }
VALUES          { /*printf("VALUES "); */ return VALUES; }
REFERENCES      { /*printf("REFERENCES "); */ return REFERENCES; }
SET             { /*printf("SET "); */ return SET; }
FROM            { /*printf("FROM "); */ return FROM; }
INTO            { /*printf("INTO "); */ return INTO; }
WHERE           { /*printf("WHERE "); */ return WHERE; }
SUM             { /*printf("SUM "); */ return SUM; }
AVG             { /*printf("AVG "); */ return AVG; }
MIN             { /*printf("MIN "); */ return MIN; }
MAX             { /*printf("MAX "); */ return MAX; }
AND             { /*printf("AND "); */ return AND; }
"NOT NULL"      { /*printf("NOT_NULL "); */ return NOTNULL; }
"PRIMARY KEY"   { /*printf("PRIMARY_KEY "); */ return PRIMARY; }
"FOREIGN KEY"   { /*printf("FOREIGN_KEY "); */ return FOREIGN; }
"GROUP BY"      { /*printf("GROUP BY ")*/ ; return GROUP; }
LIKE            { /*printf("LIKE "); */ return LIKE; }
DESC            { /*printf("DESC "); */ return DESC; }
INDEX           { /*printf("INDEX "); */ return INDEX; }
CHECK           { /*printf("CHECK "); */ return CHECK;}
IN              { /*printf("IN "); */ return IN; }
IS              { /*printf("IS "); */ return IS; }
NULL            { /*printf("NULL "); */ return T_NULL; }
int             { /*printf("int "); */ return KINT; }
float           { /*printf("float "); */ return KFLOAT; }
char            { /*printf("char "); */ return KCHAR; }
varchar         { /*printf("varchar "); */ return KVARCHAR; }
date            { /*printf("date "); */ return KDATE;  }
{FLOAT}         { /*printf("FLOAT "); */ sscanf(yytext, "%f", &yylval.fvalue); return FLOAT; }
{INTEGER}       { /*printf("INTEGER "); */ sscanf(yytext, "%d", &yylval.ivalue); return INTEGER; }
{DATE_FORMAT}   { yylval.string = get_text(yytext, true); return DATE_FORMAT; }
{IDENTIFIER}    { yylval.string = get_text(yytext, false); /*printf("IDENTIFIER(%s) ", yylval.string);*/ return IDENTIFIER; }
"="             { /*printf("= ");*/ return EQ; }
">"             { /*printf("> ");*/ return GT; }
"<"             { /*printf("< ");*/ return LT; }
">="            { /*printf(">= ");*/ return GE; }
"<="            { /*printf("<= ");*/ return LE; }
"<>"            { /*printf("<> ");*/ return NE; }

{STRING}        { yylval.string = get_text(yytext, false); /*printf("STRING(%s) ", yylval.string);*/ return STRING; }
{WHITESPACE}    {}
.               { yyerror((std::string("invalid char ") + yytext).c_str()); }

%%

int yywrap() {
    return 1;
}