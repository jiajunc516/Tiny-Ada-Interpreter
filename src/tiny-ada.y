%left And
%nonassoc '<'
%left '+'
%left '*'
%right Not

%token Declare Begin End
%token Assign /* this is for the lexeme ":=" */
%token And Not
%token If Then Else Elsif While Loop
%token Identifier Integer_literal
%{
    #include <iostream>
    using namespace std;
    #include "tiny-ada.h"
    #define yyerror(s) cerr << "Syntax error: " << s
    int yylex(void);
%}

%union {
    int in;
    char * st;
    node no;
    node_list nl;
}

%type <in> Integer_literal
%type <st> Identifier
%type <no> statement declare_block assignment_statement output_statement
%type <no> if_statement elsif_list while_statement var_decl expr
%type <nl> declaration_list statement_list

%%

start:
        statement
        { cout << "\nEval Result:\n"; $1->eval(); cout << "\nCode (pretty-printer):\n"; $1->pprint();}
;

statement:
         declare_block
    |    assignment_statement
    |    output_statement
    |    if_statement
    |    while_statement
;

declare_block:
         Declare
             declaration_list
         Begin
             statement_list
         End ';'
         { $$ = new declare_node($2, $4); }
;

var_decl:
        Identifier ':' Identifier ';'
        { $$ = new var_decl_node($1, $3); }
;

declaration_list:
         var_decl declaration_list
         { $$ = new node_pair($1, $2); }
    |    EPSILON
         { $$ = NULL; }
;

assignment_statement:
         Identifier Assign expr ';'
         { $$ = new assign_node($1, $3); }
;

output_statement:
         Identifier '(' expr ')' ';'
         { $$ = new output_node($1, $3); }
;

statement_list:
         statement statement_list
         { $$ = new node_pair($1, $2); }
    |    statement
         { $$ = new node_pair($1, NULL); }
;

if_statement:
         If expr Then
             statement_list
             elsif_list
         End If ';'
         { $$ = new if_node($2, $4, $5); }
;

elsif_list:
         Elsif expr Then
             statement_list
         elsif_list
         { $$ = new elsif_node($2, $4, $5); }
    |    Else
             statement_list
         { $$ = new if_node(NULL, $2, NULL); }
    |    EPSILON
         { $$ = NULL; }
        
;

while_statement:
        While expr Loop
            statement_list
        End Loop ';'
        { $$ = new while_node($2, $4); }
;

expr:
         Identifier         { $$ = new ident_node($1); }
    |    '(' expr ')'       { $$ = new paren_node($2); }
    |    Integer_literal    { $$ = new integer_node($1); }
    |    Not expr           { $$ = new unary_node(Not, $2); }
    |    expr And expr      { $$ = new binary_node(And, $1, $3); }
    |    expr '*' expr      { $$ = new binary_node('*', $1, $3); }
    |    expr '+' expr      { $$ = new binary_node('+', $1, $3); }
    |    expr '<' expr      { $$ = new binary_node('<', $1, $3); }
;

EPSILON :
;

%%

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    extern FILE *yyin;
    if (argc > 0)
        if (!(yyin = fopen(argv[1], "r"))) {
            cerr << "Unable to open input file: " << argv[1] << endl;
            exit(1);
        }
    yyparse();
}
