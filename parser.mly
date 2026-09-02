%{
open Ast
%}

%token LETREC
%token LET
%token IN
%token AND
%token OR
%token ASGN
%token <string> BINDING
%token <int> INT
%token NUMPLUS
%token NUMMINUS
%token NUMMUL
%token NUMDIV
%token BSL
%token BSR
%token BAND
%token BOR
%token BNOT
%token POPEN
%token PCLOSE
%token EOF

%start <Ast.expr> prog

%%

prog:
	| e = expr; EOF { e }
	;

expr:
        | 
