%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "def.h"

extern FILE *yyin;
extern char *yytext;
extern int yylex();
void yyerror ( char * );

%}


%union {
    int val;
    char* str;
    struct ttentry* rep;
    struct bool_attr* bool_rep;
    struct list* stmt_rep;
    struct for_attr* for_rep;
}

%token <str> ID INTCONST FLTCONST
%token <str> DOUBLE_EQ NOT_EQ GREATER LESS GREATER_EQ LESS_EQ
%token RTLDIV TO OR AND NOT IF ELSE DOWNTO STEP FOR

%type <stmt_rep> LIST BLOCK STMT ASGN N
%type <bool_rep> BOOL
%type <val> M
%type <str> RELOP
%type <rep> EXPR TERM FACTOR REAL REALCONST RATIONAL COMPLEX RCOMP

%start PROG
%left OR
%left AND
%nonassoc NOT

%%

PROG : LIST {  endprog($1); }
    ;

BLOCK : '{' LIST '}'    { $$ = endblock($2); }
    ;

LIST : LIST M STMT    { $$ = joinlist($1, $2, $3); }
    | { $$ = emptylist(); }
    ;

STMT : ASGN     { $$ = $1; }
    | IF '(' BOOL ')' M BLOCK                               { $$ = ifthen($3, $5, $6); }
    | IF '(' BOOL ')' M BLOCK N ELSE M BLOCK                { $$ = ifelse($3, $5, $6, $7, $9, $10); }
    | FOR '(' ID '=' EXPR { $<for_rep>$ = forinit($3, $5); }
      TO M EXPR { $<for_rep>$ = forcond($<for_rep>6, $8, $9, 1); }
      STEP M EXPR { $<for_rep>$ = forstep($<for_rep>10, $12, $13); }
      ')' M BLOCK       { $$ = forend($<for_rep>14, $16, $17); }
    | FOR '(' ID '=' EXPR { $<for_rep>$ = forinit($3, $5); }
      DOWNTO M EXPR { $<for_rep>$ = forcond($<for_rep>6, $8, $9, 0); }
      STEP M EXPR { $<for_rep>$ = forstep($<for_rep>10, $12, $13); }
      ')' M BLOCK       { $$ = forend($<for_rep>14, $16, $17); }
    ;

BOOL : EXPR RELOP EXPR                                      { $$ = relexpr($1, $2, $3); }
    | BOOL OR M BOOL                                        { $$ = boolor($1, $3, $4); }
    | BOOL AND M BOOL                                       { $$ = booland($1, $3, $4); }
    | NOT BOOL                                              { $$ = boolnot($2); }
    | '(' BOOL ')'                                          { $$ = $2; }
    ;

M : { $$ = getnquad(); };
N : { $$ = nlist(); };

RELOP : DOUBLE_EQ { $$ = $1; }
    | LESS { $$ = $1; }
    | GREATER { $$ = $1; }
    | LESS_EQ { $$ = $1; }
    | GREATER_EQ { $$ = $1; }
    | NOT_EQ { $$ = $1; }
    ;


ASGN : ID '=' EXPR ';' { $$ = assign($1, $3); }
    | ID '[' EXPR ']' '=' EXPR ';' { $$ = arrassign($1, $3, $6); }
    | ID '=' ID '[' EXPR TO EXPR ']' ';'
    {
        $$ = arrToassignment($1, $3, $5, $7);
    }

    | ID '+' EXPR '=' ID '[' EXPR TO EXPR ']' ';'
    {
        $$ = arrToassignment2($1, $5, $7, $9, $3);
    }
    ;

EXPR : EXPR '+' TERM  { $$ = arith($1, "add", $3); }
    | EXPR '-' TERM   { $$ = arith($1, "sub", $3); }
    | TERM          { $$ = $1; }
    ;

TERM : TERM '*' FACTOR  { $$ = arith($1, "mul", $3); }
    | TERM '/' FACTOR   { $$ = arith($1, "div", $3); }
    | TERM '%' FACTOR   { $$ = intrem($1, $3); }
    | FACTOR            { $$ = $1; }
    ;

FACTOR : ID             { $$ = varrep($1); }
    | '(' EXPR ')'      { $$ = $2; }
    | REAL              { $$ = $1; }
    | COMPLEX           { $$ = $1; }
    | ID '[' EXPR ']'       { $$ = arrayref($1, $3); }
    ;

REAL : REALCONST        { $$ = $1; }
    | RATIONAL          { $$ = $1; }
    ;

REALCONST : INTCONST    { $$ = make_const($1, "int"); }
    | FLTCONST          { $$ = make_const($1, "flt"); }
    ;

RATIONAL : RCOMP RTLDIV RCOMP { $$ = mkrational($1, $3); }
    ;

COMPLEX : RCOMP '@' RCOMP   { $$ = mkcomplex($1, $3); }
    ;

RCOMP : REALCONST           { $$ = $1; }
    | ID                    { $$ = varrep($1); }
    | '(' EXPR ')'          { $$ = $2; }
    | ID '[' EXPR ']'       { $$ = arrayref($1, $3); }
    ;

%%

void yyerror(char *msg){
    fprintf(stderr, "*** Error: %s\n", msg);
}
