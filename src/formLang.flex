%option noyywrap

%{
#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#include "build.h"

// Utility functions
bool isWhiteSpace(char c);
char* trim(char* string);

// What will be returned
program_build_t programBuild;
%}

NUM             [0-9]
UINT            {NUM}+
INT             "-"?{UINT}
WS              [ \n\t]
CHARR           [a-zA-Z]
ALPHNUM         {NUM}|{CHARR}
ID              {CHARR}{ALPHNUM}*
VAR             "."{ID}
FUNCTION        {ID}
FUNCTIONBEGIN   ":"{ID}
FUNCTIONEND     ";"
PRINT           "PRINT"
BOOLTRUE        "TRUE"
BOOLFALSE       "FALSE"
OP              [+-*/]

%%

{PRINT}{WS}+            {
                        appendInstruction(&programBuild, print, 0, NULL);
                        }
{BOOLTRUE}{WS}+         {
                        struct
                        {
                            data_type_t dt;
                            int32_t dn;
                        } data;

                        data.dt = f_bool;
                        data.dn = 1;

                        appendInstruction(&programBuild, push, sizeof data, &data);
                        }
{BOOLFALSE}{WS}+        {
                        struct
                        {
                            data_type_t dt;
                            int32_t dn;
                        } data;

                        data.dt = f_bool;
                        data.dn = 0;

                        appendInstruction(&programBuild, push, sizeof data, &data);
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
"<"{WS}+                {
                        appendInstruction(&programBuild, lessthen, 0, NULL);
                        }
"<="{WS}+               {
                        appendInstruction(&programBuild, lesstheneq, 0, NULL);
                        }
">"{WS}+                {
                        appendInstruction(&programBuild, greaterthen, 0, NULL);
                        }
">="{WS}+               {
                        appendInstruction(&programBuild, greatertheneq, 0, NULL);
                        }
"=="{WS}+               {
                        appendInstruction(&programBuild, eq, 0, NULL);
                        }
"!="{WS}+               {
                        appendInstruction(&programBuild, ineq, 0, NULL);
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
%%

void initializeYYLEXProgramBuilder()
{
    programBuild = prepareBuild();
}

program_context_t finishYYLEXBuild()
{
    return returnProgram(&programBuild);
}


// Move these bottom two into formLang.flex
bool isWhiteSpace(char c)
{
    return (c == ' ' || c == '\n' || c == '\t' || c == 26);
}

char* trim(char* string)
{
    char* duplicate = malloc(strlen(string));
    unsigned int index = 0;
    char* reader = string;
    while (isWhiteSpace(*reader))
    {
        ++reader;
    }

    while (!isWhiteSpace(*reader))
    {
        duplicate[index] = *reader;
        ++index; ++reader;
    }
    duplicate[index] = '\0';
    return duplicate;
}