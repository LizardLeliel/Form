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
WS              [ \n\t\r]
CHARR           [a-zA-Z]
ALPHNUM         {NUM}|{CHARR}
ID              {CHARR}{ALPHNUM}*
VAR             "."{ID}
FUNCTION        {ID}
FUNCTIONBEGIN   ":"{ID}
FUNCTIONEND     ";"
STRING          \"(\\.|[^\\"])*\"
PRINT           "PRINT"
BOOLTRUE        "TRUE"
BOOLFALSE       "FALSE"
OP              [+-*/]

%%

{PRINT}{WS}+            {
                        appendInstruction(&programBuild, print, 0, 0);
                        }
{BOOLTRUE}{WS}+         {
                        appendInstruction(&programBuild, push, f_bool, 1);
                        }
{BOOLFALSE}{WS}+        {
                        appendInstruction(&programBuild, push, f_bool, 0);
                        }
{INT}{WS}+              {
                        int64_t n = atoi(yytext);

                        appendInstruction(&programBuild, push, f_32int, n);

                        //pushConstantData(&(programBuild.constantDataList),
                        //    sizeof n, &n);
                       

                        //pushStack(f_32int, &n);
                        //printf("Found an unsigned: %u\n", n);
                        }
{STRING}{WS}+           {
                        char* trimmed = trim(yytext);
                        //printf("Found string: %p: %s\n", trimmed, trimmed);

                        //puts("Reaches here");

                        unsigned int index = 
                            pushConstantData(&(programBuild.constantDataList),
                                             f_string, trimmed);

                        appendInstruction(&programBuild, push, f_string, index);
                        // appendInstruction(&programBuild, push, &data);
                        }
"+"{WS}+                {
                        appendInstruction(&programBuild, add, 0, 0);
                        }
"-"{WS}+                {
                        appendInstruction(&programBuild, sub, 0, 0);
                        }  
"*"{WS}+                {
                        appendInstruction(&programBuild, mul, 0, 0);
                        }
"/"{WS}+                {
                        appendInstruction(&programBuild, divs, 0, 0);
                        }
"%"{WS}+                {
                        appendInstruction(&programBuild, mod, 0, 0);
                        }
"<"{WS}+                {
                        appendInstruction(&programBuild, lessthen, 0, 0);
                        }
"<="{WS}+               {
                        appendInstruction(&programBuild, lesstheneq, 0, 0);
                        }
">"{WS}+                {
                        appendInstruction(&programBuild, greaterthen, 0, 0);
                        }
">="{WS}+               {
                        appendInstruction(&programBuild, greatertheneq, 0, 0);
                        }
"=="{WS}+               {
                        appendInstruction(&programBuild, eq, 0, 0);
                        }
"!="{WS}+               {
                        appendInstruction(&programBuild, ineq, 0, 0);
                        }
{VAR}{WS}+              {
                        //int64_t = *(int64_t*)&strtof(yytext);
                        //pushStack(f_32float, &n);
                        printf("Found a var: %s\n", yytext);
                        }
{FUNCTIONBEGIN}{WS}+    {
                        //:func3+231+print;
                        char* trimmed = trim(yytext + 1);
                        //printf("Trimmed Token: (size: %lu) %s\n", strlen(trimmed), trimmed);
                        // Allow ambigious functions _for now_
                        

                        getHashID(&(programBuild.tokenHash),
                                  h_functionName, 
                                  strlen(trimmed ), 
                                  trimmed);
                        makeNewFunction(&programBuild);
                        }
{FUNCTIONEND}{WS}+      {
                        endFunction(&programBuild);
                        }
{FUNCTION}{WS}+         {
                        char* trimmed = trim(yytext);
                        //printf("Matched function: (size: %lu) %s\n", strlen(trimmed), trimmed);
                        unsigned int token = 
                            getHashID(&(programBuild.tokenHash),
                                      h_functionName,
                                      strlen(trimmed),
                                      trimmed);

                        appendInstruction(&programBuild,
                                          call, 
                                          0,
                                          token);
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