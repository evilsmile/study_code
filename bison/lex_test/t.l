%{
#include <stdio.h>
char *yylval = NULL;
%}
%%
.* yylval = yytext; return 1;

\n

%%

int main()
{
	char *fst = (yylex(), yylval);
	char *snd = (yylex(), yylval);
	printf("%s, %s\n " , fst, snd);
			return 0;
}
