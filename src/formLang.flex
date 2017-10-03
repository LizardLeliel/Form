%option noyywrap

%{
#include <math.h>
#include <inttypes.h>
#include "stack.h"
#include "build.h"
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
PRINT           "PRINT"

%%

{INT}{WS}+          {
                    int32_t n = atoi(yytext);
                    struct {
                        dataType_t dt;
                        int32_t dn;
                    } data;

                    data.dt = f_32int;
                    data.dn = n;

                    appendInstruction(push, sizeof data, &data);
                   
                    //pushStack(f_32int, &n);
                    //printf("Found an unsigned: %u\n", n);
                    }
"+"{WS}+            {
                    appendInstruction(add, 0, NULL);
                    }
{VAR}{WS}+          {
                    //int32_t = *(int32_t*)&strtof(yytext);
                    //pushStack(f_32float, &n);
                    printf("Found a var: %s\n", yytext);
                    }
":"{WS}*{ID}+       {
                    printf("Found a function");
                    }
{PRINT}{WS}+        {
                    appendInstruction(print, 0, NULL);
                    //printf("printing");
                    }

%%
