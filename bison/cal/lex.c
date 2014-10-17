#include <ctype.h>

int yylex(void)
{
	int c;
	while((c = getchar()) == ' ' || c == '\t')
		;

	if(c == '.' || isdigit(c)){
		ungetc(c, stdin);
		scanf("%lf", &&yylval);
		return NUM;
	}	
	if(c == EOF)
		return 0;
	return c;
}
