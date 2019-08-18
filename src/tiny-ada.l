%{
    typedef struct node_block* node;
    typedef struct node_pair* node_list;
    typedef int integer; 

    #include "tiny-ada.tab.h"
    #include <iostream>
    using namespace std;
    integer Line = 1;
%}
%option noyywrap
%option nounput

A  [_a-z]
N  [0-9]
AN [_a-z0-9]


%%
declare         { return Declare; }
begin           { return Begin; }
end             { return End; }
and             { return And; }
not             { return Not; }
if              { return If; }
then            { return Then; }
else            { return Else; }
elsif           { return Elsif; }
while           { return While; }
loop            { return Loop; }
[+*<:;()]       { yylval.in = yytext[0]; return yytext[0]; }
{A}{AN}*        { yylval.st = strdup(yytext); return Identifier; }
{N}+            { yylval.in = atoi(yytext); return Integer_literal; }
:=              { return Assign; }
"--".*          {  }
[ \t]           {  }
\n              { Line++; }
.               { cerr << "Lexical error line " << Line << yytext << endl; }
