%option noyywrap

%{
#include <math.h>
#include <inttypes.h>
#include "build.h"
// This can be a program build
int testX;
program_build_t programBuild;
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
                        appendInstruction(&programBuild, print, 0, NULL);
                        }
{INT}{WS}+              {
                        int32_t n = atoi(yytext);
                        struct {
                            data_type_t dt;
                            int32_t dn; 
                        } data;

                        data.dt = f_32int;
                        data.dn = n;

                        appendInstruction(&programBuild, push, sizeof data, &data);
                       
                        //pushStack(f_32int, &n);
                        //printf("Found an unsigned: %u\n", n);
                        }
"+"{WS}+                {
                        appendInstruction(&programBuild, add, 0, NULL);
                        }
"-"{WS}+                {
                        appendInstruction(&programBuild, sub, 0, NULL);
                        }  
"*"{WS}+                {
                        appendInstruction(&programBuild, mul, 0, NULL);
                        }
"/"{WS}+                {
                        appendInstruction(&programBuild, divs, 0, NULL);
                        }
"%"{WS}+                {
                        appendInstruction(&programBuild, mod, 0, NULL);
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
                        
                        getHashID(&(programBuild.tokenHash),
                                  h_functionName, 
                                  strlen(trimmed + 1), 
                                  trimmed + 1);
                        makeNewFunction(&programBuild);
                        }
{FUNCTIONEND}{WS}+      {
                        endFunction(&programBuild);
                        }
{FUNCTION}{WS}+         {
                        char* trimmed = trim(yytext);
                        unsigned int token = 
                            getHashID(&(programBuild.tokenHash),
                                      h_functionName,
                                      strlen(trimmed),
                                      trimmed);

                        appendInstruction(&programBuild,
                                          call, 
                                          sizeof(unsigned int),
                                          &token);
                        }
{BOOLTRUE}{WS}+         {
                        // We have to move this rule up sometime.

                        }
%%

void initializeYYLEXProgramBuilder()
{
    programBuild = prepareBuild();
}

program_context_t finishYYLEXBuild()
{
    return returnProgram(&programBuild);
}

// Turn this into a return build program.
int returnTestX()
{
    testX = 3;
    return testX;
}
