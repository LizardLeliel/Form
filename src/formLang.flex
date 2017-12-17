%option noyywrap

%{
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <stdbool.h>

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
IF              "IF"
THEN            "THEN"
ELIF            "ELIF"
ELSE            "ELSE"
ENDIF           "ENDIF"
OP              [+-*/]


%%

{WS}+                   {
                        // Eat whitespace   
                        }
{PRINT}                 {
                        appendInstruction(&programBuild, i_print, 0, 0);
                        }
{BOOLTRUE}              {
                        appendInstruction(&programBuild, i_push, f_bool, 1);
                        }
{BOOLFALSE}             {
                        appendInstruction(&programBuild, i_push, f_bool, 0);
                        }
{FLOAT}                 {
                        double number = atof(yytext);
                        appendInstruction(&programBuild, i_push, f_32float, 
                            interpretAsInt(number));
                        }
{INT}                   {
                        int64_t n = atoi(yytext);
                        appendInstruction(&programBuild, i_push, f_32int, n);
                        }
{STRING}                {
                        char* trimmed = trimMatchedString(yytext);

                        unsigned int index = 
                            pushConstantData(&(programBuild.constantDataList),
                                             f_string, trimmed);

                        appendInstruction(&programBuild, i_push, f_string, index);
                        }
{IF}                    {
                        // If in a current if sequence, set a new scope
                        // If not in a current if sequence, start one.
                        //  Also increment sequence by one.
                        // There must eventually be an endif.
                        // Consider disabling it for the conditional between
                        //  then and elif. 
                        if_sequence_tracker_t* tracker = getTracker(&programBuild);

                        if (tracker->scope == 0)
                        {
                            tracker->sequence    += 1;
                            // Function number already taken care of
                            tracker->elifSequence = 0;
                            tracker->scope       += 1;
                            tracker->thenFlag     = false;
                            tracker->elseFlag     = false;
                            tracker->nextID      += 1;

                            // Start a new one
                        }
                        else
                        {
                            puts("Warning: scoped ifs not implemented");
                            // push into one, rewrite some tracker
                            //  values.
                        }

                        }
{THEN}                  {
                        // Needs to check to see if there is a matching
                        //  if, or else if. (but no then)
                        if_sequence_tracker_t* tracker = getTracker(&programBuild);

                        if (tracker->scope == 0)
                        {
                            // also add line number to this error messageS
                            puts("Then found with no previous if");
                            exit(1);
                        }
                        if (tracker->thenFlag == true)
                        {
                            puts("Then found when expecting elif");
                            exit(1);
                        }
                        if (tracker->elseFlag == true)
                        {
                            puts("Then found inside an else statement");
                            exit(1);
                        }

                        tracker->thenFlag = true;
                        }
{ELIF}                  {
                        // Needs to check if there is a matching then, and
                        //  in an if sequence
                        if_sequence_tracker_t* tracker = getTracker(&programBuild);
                        
                        if (tracker->scope == 0)
                        {
                            // also add line number to this error messageS
                            puts("elif found with no previous if");
                            exit(1);
                        }
                        if (tracker->thenFlag == false)
                        {
                            puts("Elif must have a corresponding then keyword");
                            exit(1);
                        }
                        // This is technically redundant
                        if (tracker->elseFlag == true)
                        {
                            puts("elif inside else statement");
                            exit(1);
                        }

                        tracker->thenFlag      = false;
                        // tracker->elseFlag    = true;
                        tracker->elifSequence += 1;

                        }
{ELSE}                  {
                        // Needs to check if there is a previous if
                        //  or elif (but not then)
                        if_sequence_tracker_t* tracker = getTracker(&programBuild);
                        
                        if (tracker->scope == 0)
                        {
                            // also add line number to this error messageS
                            puts("Else found with no previous if");
                            exit(1);
                        }
                        if (tracker->thenFlag == true)
                        {
                            puts("Found else when expecting an elif");
                            exit(1);
                        }
                        tracker->elseFlag = true;
                        }
{ENDIF}                 {
                        // Needs to check if there is a matching if,
                        // no then. (check scope)
                        if_sequence_tracker_t* tracker = getTracker(&programBuild);
                        
                        if (tracker->scope == 0)
                        {
                            // also add line number to this error messageS
                            puts("elif found with no previous if");
                            exit(1);
                        }

                        if (tracker->thenFlag == true)
                        {
                            puts("Found endif when expecting an elif");
                            exit(0);
                        }

                        tracker->elseFlag      = false;
                        tracker->elifSequence  = 0;
                        tracker->scope        -= 1;
                        // If not at scope 1, pop previous scope.
                        } 
"+"                     {
                        appendInstruction(&programBuild, i_add, 0, 0);
                        }
"-"                     {
                        appendInstruction(&programBuild, i_sub, 0, 0);
                        }  
"*"                     {
                        appendInstruction(&programBuild, i_mul, 0, 0);
                        }
"/"                     {
                        appendInstruction(&programBuild, i_div, 0, 0);
                        }
"%"                     {
                        appendInstruction(&programBuild, i_mod, 0, 0);
                        }
"<"                     {
                        appendInstruction(&programBuild, i_lessthen, 0, 0);
                        }
"<="                    {
                        appendInstruction(&programBuild, i_lesstheneq, 0, 0);
                        }
">"                     {
                        appendInstruction(&programBuild, i_greaterthen, 0, 0);
                        }
">="                    {
                        appendInstruction(&programBuild, i_greatertheneq, 0, 0);
                        }
"=="                    {
                        appendInstruction(&programBuild, i_eq, 0, 0);
                        }
"!="                    {
                        appendInstruction(&programBuild, i_ineq, 0, 0);
                        }
"&&"                    {
                        appendInstruction(&programBuild, i_logand, 0, 0);
                        }
"||"                    {
                        appendInstruction(&programBuild, i_logor, 0, 0);
                        }
"!"                     {
                        appendInstruction(&programBuild, i_lognot, 0, 0);
                        }
{VAR}                   {
                        printf("Found a var: %s\n", yytext);
                        }
{FUNCTIONBEGIN}         {
                        // yytext + 1 to cut off the ':'
                        char* trimmed =  yytext + 1;

                        if (!createHashBucket(
                                  &(programBuild.tokenHash),
                                  h_functionName, 
                                  strlen(trimmed), 
                                  trimmed,
                                  true))
                        {
                            printf("Function \"%s\" already defined\n", 
                                   trimmed);
                            exit(1);
                        }

                        makeNewFunction(&programBuild);
                        }
{FUNCTIONEND}           {
                        endFunction(&programBuild);
                        // Also check: see if not in
                        //  invalid if sequence state.
                        }
{FUNCTION}              {
                        char* trimmed = yytext;

                        unsigned int token = 
                            getHashValue(&(programBuild.tokenHash),
                                         h_functionName,
                                         strlen(trimmed),
                                         trimmed);

                        appendInstruction(&programBuild,
                                          i_call, 
                                          0,
                                          token);
                        }
<<EOF>>                 {
                        // Also check: see if not in
                        //  invalid if sequence state.
                        if (programBuild.onMain == false)
                        {
                            puts("Reached EOF while parsing function");
                            exit(1);
                        }
                        if_sequence_tracker_t* tracker = getTracker(&programBuild);

                        if (tracker->scope != 0)
                        {
                            puts("Reached end of file while expecting an elif or endif");
                            exit(1);
                        }

                        return 0;
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

bool isWhiteSpace(char c)
{
    return (c == ' ' || c == '\n' || c == '\t' || c == 26);
}

char* trimMatchedString(char* string)
{
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