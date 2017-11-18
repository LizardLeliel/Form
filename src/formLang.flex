%option noyywrap

%{
#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#include "build.h"

// Utility functions
bool isWhiteSpace(char c);
char* trim(char* string);
char* trimMatchedString(char* string);
// unsigned int = 0;

// What will be returned
program_build_t programBuild;
%}

NUM             [0-9]
UINT            {NUM}+
INT             "-"?{UINT}
WS              [ \t\r\n]
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

{WS}+                   {
                                   
                        }
{PRINT}                 {
                        appendInstruction(&programBuild, print, 0, 0);
                        }
{BOOLTRUE}              {
                        appendInstruction(&programBuild, push, f_bool, 1);
                        }
{BOOLFALSE}             {
                        appendInstruction(&programBuild, push, f_bool, 0);
                        }
{INT}                   {
                        int64_t n = atoi(yytext);

                        appendInstruction(&programBuild, push, f_32int, n);

                        //pushConstantData(&(programBuild.constantDataList),
                        //    sizeof n, &n);
                       

                        //pushStack(f_32int, &n);
                        //printf("Found an unsigned: %u\n", n);
                        }
{STRING}                {
                        char* trimmed = trimMatchedString(yytext);
                        //printf("Found string: %p: %s\n", trimmed, trimmed);
                        //puts("Reaches here");

                        unsigned int index = 
                            pushConstantData(&(programBuild.constantDataList),
                                             f_string, trimmed);

                        appendInstruction(&programBuild, push, f_string, index);
                        // appendInstruction(&programBuild, push, &data);
                        }
"+"                     {
                        appendInstruction(&programBuild, add, 0, 0);
                        }
"-"                     {
                        appendInstruction(&programBuild, sub, 0, 0);
                        }  
"*"                     {
                        appendInstruction(&programBuild, mul, 0, 0);
                        }
"/"                     {
                        appendInstruction(&programBuild, divs, 0, 0);
                        }
"%"                     {
                        appendInstruction(&programBuild, mod, 0, 0);
                        }
"<"                     {
                        appendInstruction(&programBuild, lessthen, 0, 0);
                        }
"<="                    {
                        appendInstruction(&programBuild, lesstheneq, 0, 0);
                        }
">"                     {
                        appendInstruction(&programBuild, greaterthen, 0, 0);
                        }
">="                    {
                        appendInstruction(&programBuild, greatertheneq, 0, 0);
                        }
"=="                    {
                        appendInstruction(&programBuild, eq, 0, 0);
                        }
"!="                    {
                        appendInstruction(&programBuild, ineq, 0, 0);
                        }
{VAR}                   {
                        //int64_t = *(int64_t*)&strtof(yytext);
                        //pushStack(f_32float, &n);
                        printf("Found a var: %s\n", yytext);
                        }
{FUNCTIONBEGIN}         {
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
{FUNCTIONEND}           {
                        endFunction(&programBuild);
                        }
{FUNCTION}              {
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
    char* duplicate    = malloc(strlen(string));
    unsigned int index = 0;
    char* reader       = string;

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

char* trimMatchedString(char* string)
{
    size_t stringSize = strlen(string);
    char* duplicate   = malloc(stringSize);
    char* reader      = string;

    while(isWhiteSpace(*reader))
    {
        ++reader;
    }

    ++reader;

    if (*reader == '"')
    {
        *duplicate = '\0';
        return duplicate;
    }

    char* endReader = string + strlen(string) - 1;
    
    while (isWhiteSpace(*endReader))
    {
        --endReader;
    }

    int index = 0;
    while (reader != endReader)
    {
        duplicate[index] = *reader;
        ++reader;
        ++index;
    }

    return duplicate;

}