%option noyywrap

%{
#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#include "build.h"

extern int64_t interpretAsInt(double value);
extern double interpretAsFloat(int64_t value);

// Utility functions
bool isWhiteSpace(char c);
char* trimMatchedString(char* string);


// What will be returned
program_build_t programBuild;
%}

NUM             [0-9]
UINT            {NUM}+
INT             "-"?{UINT}
FLOAT           {UINT}"."{UINT}
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
{FLOAT}                 {
                        double number = atof(yytext);
                        appendInstruction(&programBuild, push, f_32float, 
                            interpretAsInt(number));
                        }
{INT}                   {
                        int64_t n = atoi(yytext);
                        appendInstruction(&programBuild, push, f_32int, n);
                        }
{STRING}                {
                        char* trimmed = trimMatchedString(yytext);

                        unsigned int index = 
                            pushConstantData(&(programBuild.constantDataList),
                                             f_string, trimmed);

                        appendInstruction(&programBuild, push, f_string, index);
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
"&&"                    {
                        appendInstruction(&programBuild, logand, 0, 0);
                        }
"||"                    {
                        appendInstruction(&programBuild, logor, 0, 0);
                        }
"!"                     {
                        appendInstruction(&programBuild, lognot, 0, 0);
                        }
{VAR}                   {
                        printf("Found a var: %s\n", yytext);
                        }
{FUNCTIONBEGIN}         {
                        char* trimmed =  yytext + 1;
                        getHashID(&(programBuild.tokenHash),
                                  h_functionName, 
                                  strlen(trimmed), 
                                  trimmed);
                        makeNewFunction(&programBuild);
                        }
{FUNCTIONEND}           {
                        endFunction(&programBuild);
                        }
{FUNCTION}              {
                        char* trimmed = yytext;

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


char* trimMatchedString(char* string)
{
    size_t stringSize = strlen(string);
    char* reader      = string;

    while(isWhiteSpace(*reader))
    {
        ++reader;
    }

    ++reader;

    if (*reader == '"')
    {
        char* duplicate = malloc(sizeof (char));
        *duplicate = '\0';
        return duplicate;
    }

    char* endReader = string + strlen(string) - 1;
    
    while (isWhiteSpace(*endReader))
    {
        --endReader;
    }

    char* duplicate 
        = calloc((endReader - reader) / sizeof (char) + 1,
                 sizeof (char));
    int index = 0;
    while (reader != endReader)
    {
        duplicate[index] = *reader;
        ++reader;
        ++index;
    }

    return duplicate;

}