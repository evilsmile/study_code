%{
#include "eng.tab.h"
#define LOOKUP 0
int state = 1;
%}

%%
\n {state = LOOKUP; }
\.\n {state = LOOKUP; return;}

^verb {state = VERB; }
^adj {state = ADJECTIVE; }
^noun { state = NOUN; }
^conj {state = CONJUNCTION; }

[a-zA-Z]+ {
	if(state != LOOKUP){
		add_word(state, yytext);
	}else{
		switch(lookup_word(yytext)){
			case VERB:
				return (VERB);
			case ADJECTIVE:
				return (ADJECTIVE);
			case NOUN:
				return (NOUN);
			case CONJUNCTION:
				return (CONJUNCTION);
			default:
				printf("%s: don't recognize\n", yytext);
		}
	}
}
;
%%

struct word{
	char word_name[20];
	int word_type;
	struct word *next;
};
struct word *word_list;
extern void *malloc();
int add_word(int type, char *word)
{
	struct word *wp;
	if(lookup_word(word) != LOOKUP){
		printf("!warning: word %s already defind\n", word);
		return 0;
	}
	wp = (struct word*)malloc(sizeof(struct word));
	wp->next = word_list;
	strcpy(wp->word_name, word);
	wp->word_type = type;
	word_list = wp;
	return 1;
}

int lookup_word(char *word)
{
	struct word *wp = word_list;
	for(; wp; wp = wp->next){
		if(strcmp(wp->word_name, word) == 0){
			return wp->word_type;
		}
	}
	return  LOOKUP;
}
