%{
#include <stdio.h>
#define YYSTYPE char const *
int yylex(void);
void yyerror(char const *);
static YYSTYPE stmtMerge(YYSTYPE x0, YYSTYPE x1);
%}

%token TYPENAME ID

%right '='
%left '+'

%glr-parse

%%

prog:
	| prog stmt { printf ("/n"); }
	;
#stmt: expr ';' %dprec 1
#	| decl  %dprec 2
stmt: expr ';' %merge <stmtMerge>
	| decl %merge <stmtMerge>
expr: ID { printf("%s ", $$); }
	| TYPENAME '(' expr ')' { printf ("%s <cast> ", $1); }
	| expr '+' expr { printf ("+ "); }
	| expr '=' expr { printf ("= "); }
	;
decl: TYPENAME declarator ';' {printf ("%s <declare> ", $1); }
	| TYPENAME delete '=' expr ';' {printf9 ("%s <init-declare> ", $1); }
	;
declarator: ID {printf("\"%s\" ", $1; }
	| '(' declarator ')'
	;	

%%

static YYSTYPE
stmtMerge (YYSTYPE x0, YYSTYPE x1)
{
	printf ("<OR> ");
   	return ;
}
