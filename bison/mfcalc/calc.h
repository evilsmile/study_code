#ifndef _CALC_H__
#define _CALC_H__
typedef double (*func_t)(double);

struct symrec{
	char *name;
	int type;
	union{
		double var;
		func_t fnctptr;
	}value;
	struct symrec *next;
};

typedef struct symrec symrec;

extern symrec *sym_table;

symrec *putsym(char const*, int);
symrec *getsym(char const *);

#endif
