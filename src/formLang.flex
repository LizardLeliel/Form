%option noyywrap

%{
#include <math.h>
#include <inttypes.h>
#include "stack.h"
%}

NUM             [0-9]
UINT            {NUM}+
INT             "-"?{UINT}
WS              [ \n\t\<<EOF>>]
CHARR           [a-zA-Z]
ALPHNUM         {NUM}|{CHARR}
ID              {CHARR}{ALPHNUM}*
VAR             "."{ID}
OP              [+-*/]

%%

{UINT}{WS}          {
                    int32_t n = atoi(yytext);
                    //pushStack(n, &n);
                    printf("Found an unsigned: %u\n", n);
                    }
{VAR}{WS}           {
                    printf("Found a var: %s\n", yytext);
                    }
":"{WS}*{ID}        {
                    printf("Found a function");
                    }
%%
