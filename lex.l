%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "y.tab.h"
%}


ws [ \t\r\n]
id [a-zA-Z_][a-zA-Z0-9_]*
int_const	[+-]?[0-9]+
float_const [+-]?(([0-9]+[.][0-9]*|[.][0-9]+)([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+)
rtl_div "//"
to "to"
or "||"
and "&&"
not "!"
if "if"
else "else"
downto "downto"
step "step"
double_eq "=="
not_eq "!="
less "<"
greater ">"
less_eq "<="
greater_eq ">="
for "for"


%%

{ws}    {}
;       {return yytext[0];}
\=      {return yytext[0];}
\+      {return yytext[0];}
\-      {return yytext[0];}
\*      {return yytext[0];}
\/      {return yytext[0];}
\/\/    {return RTLDIV;}
\%      {return yytext[0];}
\{      {return yytext[0];}
\}      {return yytext[0];}
\(      {return yytext[0];}
\)      {return yytext[0];}
\[      {return yytext[0];}
\]      {return yytext[0];}
@       {return yytext[0];}
{for}               {return FOR;}
{or}                {return OR;}
{and}               {return AND;}
{not}               {return NOT;}
{if}                {return IF;}
{else}              {return ELSE;}
{downto}            {return DOWNTO;}
{step}              {return STEP;}
{double_eq}         {yylval.str = "eq"; return DOUBLE_EQ;}
{not_eq}            {yylval.str = "ne"; return NOT_EQ;}
{greater}           {yylval.str = "gt"; return GREATER;}
{less}              {yylval.str = "lt"; return LESS;}
{greater_eq}        {yylval.str = "ge"; return GREATER_EQ;}
{less_eq}           {yylval.str = "le"; return LESS_EQ;}
{to}                {return TO;}
{float_const}       {yylval.str = strdup(yytext); return FLTCONST;}
{int_const}         {yylval.str = strdup(yytext); return INTCONST;}
{id}                {yylval.str = strdup(yytext); return ID;}
.                   {fprintf(stderr, "*** Lex error: bad character: %c\n", yytext[0]);}

%%

int yywrap(){
    return 1;
}
