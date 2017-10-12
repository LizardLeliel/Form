%option noyywrap

%{
#include <math.h>
#include <inttypes.h>
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
FUNCTION        {ID}
FUNCTIONBEGIN   ":"{ID}
FUNCTIONEND     ";"
OP              [+-*/]
PRINT           "PRINT"
BOOLTRUE        "TRUE"
BOOLFALSE       "FALSE"

%%

{PRINT}{WS}+            {
                        appendInstruction(print, 0, NULL);
                        }
{INT}{WS}+              {
                        int32_t n = atoi(yytext);
                        struct {
                            data_type_t dt;
                            int32_t dn; 
                        } data;

                        data.dt = f_32int;
                        data.dn = n;

                        appendInstruction(push, sizeof data, &data);
                       
                        //pushStack(f_32int, &n);
                        //printf("Found an unsigned: %u\n", n);
                        }
"+"{WS}+                {
                        appendInstruction(add, 0, NULL);
                        }
"-"{WS}+                {
                        appendInstruction(sub, 0, NULL);
                        }  
"*"{WS}+                {
                        appendInstruction(mul, 0, NULL);
                        }
"/"{WS}+                {
                        appendInstruction(divs, 0, NULL);
                        }
"%"{WS}+                {
                        appendInstruction(mod, 0, NULL);
                        }
{VAR}{WS}+              {
                        //int32_t = *(int32_t*)&strtof(yytext);
                        //pushStack(f_32float, &n);
                        printf("Found a var: %s\n", yytext);
                        }
{FUNCTIONBEGIN}{WS}+    {
                        //:func3+231+print;
                        char* trimmed = trim(yytext);
                        //printf("Trimmed Token: %s\n", trimmed);
                        // Allow ambigious functions _for now_
                        
                        getHashID(h_functionName, 
                                strlen(trimmed + 1), 
                                trimmed + 1);
                        makeNewFunction();
                        }
{FUNCTIONEND}{WS}+      {
                        endFunction();
                        }
{FUNCTION}{WS}+         {
                        char* trimmed = trim(yytext);
                        unsigned int token = 
                            getHashID(h_functionName,
                                strlen(trimmed),
                                trimmed);

                        appendInstruction(call, sizeof(unsigned int),
                            &token);
                        }
{BOOLTRUE}{WS}+         {
                        // We have to move this rule up sometime.

                        }
%%
