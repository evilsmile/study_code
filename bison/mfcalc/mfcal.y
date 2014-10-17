%{
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "calc.h"
int yylex(void);
void yyerror(char const*);
%}

/* union声明了所有的可能的类型的清单,用来取代YYSTYPE. 在这里允许的类型是双精度(为了exp和NUM)
 * 和指向符号表目录项的指针
 */

%union {
	double val; /* For returning number; */
	symrec *tptr; /* For returng symbol-table pointers. */
}
%token <val> NUM /* Simple double precesion number. */
%token <tptr> VAR FNCT /* Variable and Function */
%type <val> exp

%right '='
%left '-' '+'
%left '*' '/'
%left NEG
%right '^'

%%
input: /* empty */
	| input line
;

line: '\n'
	| exp '\n' {printf("\t%.10g\n", $1); }
	| error '\n' { yyerrok; }
;

exp: NUM { $$ = $1; }
	| VAR { $$ = $1->value.var; }
	| VAR '=' exp { $$ = $3; $1->value.var = $3; }
	| FNCT '(' exp ')' { $$ = (*($1->value.fnctptr))($3); }
	| exp '+' exp { $$ = $1 + $3; }
	| exp '-' exp { $$ = $1 - $3; }
	| exp '*' exp { $$ = $1 * $3; }
	| exp '/' exp { 
		if ($3) {
			$$ = $1 / $3; 
		} else {
			$$ = 1;
			fprintf(stderr, "%d.%d-%d.%d: division by zero",
					@3.first_line, @3.first_column,
					@3.last_line, @3.last_column);
		}
	   }
	| exp '^' exp { $$ = pow($1, $3); }
	| '-' exp { $$ = -$2; }
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

	if (c == '.' || isdigit(c)) {
		ungetc(c, stdin);
		scanf("%lf", &yylval.val);
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

	if (isalpha(c)) {
		symrec *s;
		static char *symbuf = 0;
		static int length = 0;
		int i ;

		if (length == 0) {
			length = 40;
			symbuf = (char*)malloc(length + 1);
		}
		i = 0;
		do{
			if (i == length) {
				length *= 2;
				symbuf = (char*)realloc(symbuf, length + 1);
			}
			symbuf[i++] = c;
			c = getchar();
		}while(isalnum(c));
		ungetc(c, stdin);
		symbuf[i] = '\0';

		s = getsym(symbuf);
		if(s == 0) {
			s = putsym(symbuf, VAR);
		}
		yylval.tptr = s;
		return s->type;
	}
	return c;
}

void yyerror(char const* msg)
{
	printf("%s\n", msg);
}

struct init {
	char const *fname;
	double (*fnct)(double);
};

struct init const arith_fncts[] = {
	"sin", sin,
	"cos", cos,
	"atan", atan,
	"ln", log,
	"exp", exp,
	"sqrt", sqrt,
	0, 0
};

symrec *sym_table;
void init_table(void)
{
	int i;
	symrec *ptr;
	for(i = 0; arith_fncts[i].fname != 0; i++){
		ptr = putsym(arith_fncts[i].fname, FNCT);
		ptr->value.fnctptr = arith_fncts[i].fnct;
	}
}

symrec * putsym(char const *sym_name, int sym_type)
{
	symrec *ptr;
	ptr = (symrec*)malloc(sizeof(symrec));
	ptr->name = (char*)malloc(strlen(sym_name) + 1);
	strcpy(ptr->name, sym_name);
	ptr->type = sym_type;
	ptr->value.var = 0;
	ptr->next = (struct symrec*)sym_table;
	sym_table = ptr;
	return ptr;
}

symrec * getsym(char const *sym_name)
{
	symrec *ptr;
	for(ptr = sym_table; ptr != (symrec*)0; ptr = (symrec*)ptr->next){
		if (strcmp(ptr->name, sym_name) == 0){
				return ptr;
		}
	}
	return 0;
}

int main(void)
{
	init_table();
   	return yyparse();
}
