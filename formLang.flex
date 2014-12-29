%option noyywrap

%{
#include <math.h>
%}

NUM				[0-9]
INT				{NUM}+
UINT				"-"?{INT}
WS				[ \n\t\<<EOF>>]
CHARR				[a-zA-Z]
ALPHNUM			{NUM}|{CHARR}
ID				{CHARR}{ALPHNUM}*
VAR          			"."{ID}

%%

{INT}{WS}			{
				printf("Found an int: %d\n", atoi(yytext));
				}
{VAR}{WS}			{
				printf("Found a var: %s\n", yytext);
				}
%%
