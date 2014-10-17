/* Reverse polish notation calculator. */

%{
#define YYSTYPE double
#include <stdio.h>
#include <math.h>
int yylex(void);
void yyerror(char const *);

%}

%token NUM
%left '-' '+'
%left '*' '/'
%left NEG
%right '^'

%%
input: /*empty*/
	| input line
;
line: '\n'
	| exp '\n' { printf ("\t%.10g\n", $1);}
	| error '\n' { yyerrok; printf("\n");}

;
exp: NUM {$$ = $1;}
	| exp '+' exp { $$ = $1 + $3; }
	| exp '-' exp{ $$ = $1 - $3; }
	| exp '*' exp { $$ = $1 * $3; }
	| exp '/' exp
		{
			if ($3)
			   	$$ = $1 / $3;
			else{
				$$ = 1;
				fprintf(stderr, "%d.%d-%d.%d: divsion by zero",
						@3.first_line, @3.first_column,
						@3.last_line, @3.last_column);
			}
	   	}

	| exp '^' exp { $$ = pow($1, $3); }
	| '-' exp %prec NEG { $$ = -$2; }
	| '(' exp ')' { $$ = $2; }
;

%%
int yylex(void)
{
	int c;
	while((c = getchar()) == ' ' || c == '\t'){
		++yylloc.last_column;
	}
	yylloc.first_line = yylloc.last_line;
	yylloc.first_column = yylloc.last_column;

	if (isdigit(c)) {
		yylval = c - '0';
		++yylloc.last_column;
		while ( isdigit(c = getchar()) ) {
			++yylloc.last_column;
			yylval = yylval * 10 + c - '0';
		}
		ungetc(c, stdin);
		return NUM;
	}
		
	if (c == EOF) {
		return 0;
	}

	if (c == '\n') {
		++yylloc.last_line;
		yylloc.last_column = 0;
	} else {
		++yylloc.last_column;
	}
	return c;
}
 
void yyerror(char const *msg)
{
	fprintf(stderr, "%s - -\n", msg);
}

int main()
{
	yylloc.first_line = yylloc.last_line = 1;
	yylloc.first_column = yylloc.last_column = 0;
	return yyparse();
}
