#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "calclist.h"
#include "calclist.tab.h"

static char* __globalInputText = 0;
static int __globalInputTextLen = 0;
static int __globalReadOffset = 0;
calclist_ast_t* __gencalclistast = 0;
calclist_ast_t* gencalclistast_normal[MAX_ROUTE_ITEM]={NULL};

calclist_ast_t*
new_calclist_ast(int nodetype, calclist_ast_t*l, calclist_ast_t*r)
{
	calclist_ast_t* a = (calclist_ast_t*)malloc(sizeof(struct calclist_ast));

	if (!a) {
		yyerror("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	a->l = l;
	a->r = r;
	return a;
}

calclist_ast_t*
new_calclist_num(int d)
{
	calclist_numval_t* a = (calclist_numval_t*)malloc(sizeof(calclist_numval_t));

	if (!a) {
		yyerror("out of space");
		exit(0);
	}
	a->nodetype = NUMBER;
	a->number = d;
	return (calclist_ast_t*)a;
}

calclist_ast_t*
new_calclist_string(const char* s)
{
	calclist_stringval_t* a = (calclist_stringval_t*)malloc(sizeof(calclist_stringval_t));

	if (!a) {
		yyerror("out of space");
		exit(0);
	}
	a->nodetype = STRING;
	strcpy(a->string, s);
	return (calclist_ast_t*)a;
}

calclist_ast_t*
new_calclist_key(const char* k)
{
	calclist_keyval_t* a = (calclist_keyval_t*)malloc(sizeof(calclist_keyval_t));

	if (!a) {
		yyerror("out of space");
		exit(0);
	}
	a->nodetype = KEY;
	strcpy(a->key, k);
	return (calclist_ast_t*)a;
}
calclist_ast_t*
getkeyvalue(const char* key, calclist_context_t* context) {
	if (strcmp(key, "PRODUCT_ID") == 0) {
		return new_calclist_num(context->product_id);
	} else if (strcmp(key, "AMOUNT") == 0) {
		return new_calclist_num(context->amount);
	} else if (strcmp(key, "CARD_TYPE") == 0) {
		return new_calclist_num(context->card_type);
	} else if (strcmp(key, "CARD_DAY_AMOUNT") == 0) {
		return new_calclist_num(context->card_day_amount);
	} else if (strcmp(key, "CARD_MONTH_AMOUNT") == 0) {
		return new_calclist_num(context->card_month_amount);
	} else if (strcmp(key, "CARD_MONTH_COUNT") == 0) {
		return new_calclist_num(context->card_month_count);
	} else if (strcmp(key, "CARD_BANK") == 0) {
		return new_calclist_string(context->card_bank);
	} else if (strcmp(key, "UID") == 0){
		return new_calclist_num(context->uid);
	} else if (strcmp(key, "UIN") == 0){
		return new_calclist_string(context->uin);
	} else if (strcmp(key, "PAY_TYPE") ==0){
		return new_calclist_num(context->pay_type);
	} else if (strcmp(key, "APP_TYPE") ==0){
		return new_calclist_num(context->app_type);
	} else if (strcmp(key, "TERM_ID") ==0){
		return new_calclist_string(context->term_id);
	} else if (strcmp(key, "MOBILE_ID") == 0){
		return new_calclist_string(context->mobile_id);
	} else if (strcmp(key, "CARD_ID") == 0){
		return new_calclist_string(context->card_id);
	} else if (strcmp(key, "BUS_MOBILE_NUM"/*交易手机号码*/) == 0){
		return new_calclist_string(context->bus_mobile_number);
	} else if (strcmp(key, "BIND_MOBILE_NUM" /*绑定的手机号码*/) == 0){
		return new_calclist_string(context->bind_mobile_number);
	}
	else{
		return new_calclist_string("string");
	}
}

int
calclist_eval(calclist_ast_t* a, calclist_context_t* context){
	if (a->nodetype == NUMBER) { 
		return ((calclist_numval_t *)a)->number;
	} else if (a->nodetype == STRING || a->nodetype == KEY) {
		yyerror("internal error: string/key can't do this %d\n", a->nodetype);
		return -1;
	}

	calclist_ast_t* left = a->l;
	calclist_ast_t* right = a->r;

	calclist_ast_t* leftkeyvalue = 0;
	calclist_ast_t* rightkeyvalue = 0;
	
	if (left->nodetype == KEY) {
		leftkeyvalue = getkeyvalue(((calclist_keyval_t *)left)->key, context);
		left = leftkeyvalue;
	}
	if (right->nodetype == KEY) {
		rightkeyvalue = getkeyvalue(((calclist_keyval_t *)right)->key, context);
		right = rightkeyvalue;
	}

	int v;
	
	//pre do S operator S
	unsigned char haveresult = 0;
	unsigned char haveerror = 0;
	do {
		if (left->nodetype == STRING) {
			if (right->nodetype != STRING) {
				haveerror = 1;
				yyerror("internal error: string can't do this %d\n", a->nodetype);
				break;
			} else {
				haveresult = 1;
				char* left_string = ((calclist_stringval_t*)left)->string;
				char* right_string = ((calclist_stringval_t*)right)->string;
				int ncompare = strcmp(left_string, right_string);
				switch (a->nodetype) {
					case '<':
						v = ncompare == -1;
                        break;
					case '>':
						v = ncompare == 1;
                        break;
					case EQUAL:
						v = ncompare == 0;
						break;
					case NOTEQUAL:
						v = ncompare != 0;
						break;
					case LIKEEQUAL:
						if (ncompare == 0) {
							v = 1;
							break;
						} else {
							v = (strstr(left_string, right_string) > 0) ? 1:0;
							break;
						}
					case GTEQUAL:
						v = ncompare >= 0;
						break;
					case LTEQUAL:
						v = ncompare <= 0;
						break;
					default:
						haveerror = 1;
						yyerror("internal error: string can't do this %d\n", a->nodetype);
						break;
				}
				break;
			}
		}
	} while (0);
	
	if (haveerror == 1) {
		if (leftkeyvalue) {
			free(leftkeyvalue);
			leftkeyvalue = 0;
		}
		if (rightkeyvalue) {
			free(rightkeyvalue);
			rightkeyvalue = 0;
		}
		return -1;
	}

	if (haveresult == 1) {
		if (leftkeyvalue) {
			free(leftkeyvalue);
			leftkeyvalue = 0;
		}
		if (rightkeyvalue) {
			free(rightkeyvalue);
			rightkeyvalue = 0;
		}
		return v;
	}
	//end pre
	
	switch (a->nodetype) {
	case '<':
		v = calclist_eval(left, context) < calclist_eval(right, context);
        break;
	case '>':
		v = calclist_eval(left, context) > calclist_eval(right, context);
        break;
	case '+':
		v = calclist_eval(left, context) + calclist_eval(right, context);
		break;
	case '-':
		v = calclist_eval(left, context) - calclist_eval(right, context);
		break;
	case '*':
		v = calclist_eval(left, context) * calclist_eval(right, context);
		break;
	case '/':
		v = calclist_eval(left, context) / calclist_eval(right, context);
		break;
	case AND:
		v = calclist_eval(left, context) && calclist_eval(right, context);
		break;
	case OR:
		v = calclist_eval(left, context) || calclist_eval(right, context);
		break;
	case EQUAL:
		v = calclist_eval(left, context) == calclist_eval(right, context);
		break;
	case NOTEQUAL:
		v = calclist_eval(left, context) != calclist_eval(right, context);
		break;
	case GTEQUAL:
		v = calclist_eval(left, context) >= calclist_eval(right, context);
		break;
	case LTEQUAL:
		v = calclist_eval(left, context) <= calclist_eval(right, context);
		break;
	case 'M':
		v = -calclist_eval(left, context);
		break;
	default:
		yyerror("internal error: bad node %d\n", a->nodetype);
	}

	if (leftkeyvalue) {
		free(leftkeyvalue);
		leftkeyvalue = 0;
	}
	if (rightkeyvalue) {
		free(rightkeyvalue);
		rightkeyvalue = 0;
	}
	
	return v;
}

void
calclist_treefree(calclist_ast_t*a)
{
	switch (a->nodetype) {
	/* no subtree */
	case NUMBER:
	case STRING:
	case KEY:
		free(a);
		break;

	default:
		if (a->l) {
			calclist_treefree(a->l);
		}
		if (a->r) {
			calclist_treefree(a->r);
		}
		free(a);
	}
}

void
yyerror(const char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}


int
readInputForLexer(char *buffer, int *numBytesRead, int maxBytesToRead) {
	int numBytesToRead = maxBytesToRead;
	int bytesRemaining = __globalInputTextLen-__globalReadOffset;
	int i;
	if (numBytesToRead > bytesRemaining) {
		numBytesToRead = bytesRemaining;
	}
	for ( i = 0; i < numBytesToRead; i++ ) {
	   buffer[i] = __globalInputText[__globalReadOffset+i];
	}
	*numBytesRead = numBytesToRead;
	__globalReadOffset += numBytesToRead;
	return 0;
}

int TestRegularNormal(const char *pRegular,int nLen)
{
	__globalInputText = (char *)pRegular;
	__globalInputTextLen = nLen;
	__globalReadOffset = 0;
	
	if ( yyparse() != 0) {
		return -1;
	}
	return 0;
}
int SetRegularNormal(const char * pRegular,int nLen,unsigned int nIndex)
{
	if (nIndex >= MAX_ROUTE_ITEM) {
		return -1;
	}

	if (gencalclistast_normal[nIndex] != NULL) {
		calclist_treefree(gencalclistast_normal[nIndex]);
		gencalclistast_normal[nIndex] = NULL;
	}
	
	__globalInputText = (char *)pRegular;
	__globalInputTextLen = nLen;
	__globalReadOffset = 0;
	
	if ( yyparse() != 0) {
		return -2;
	}
	gencalclistast_normal[nIndex] = __gencalclistast;

	return 0;
}


int GetRegularIndexNormal(calclist_context_t &ctx)
{
	int nIndex = -1;
	for (int i = 0; i < MAX_ROUTE_ITEM; i++) {
		if (gencalclistast_normal[i] == NULL) {
			break;
		}
		if (calclist_eval(gencalclistast_normal[i], &ctx) == 1) {
			nIndex = i;
			break;
		}
	}

	return nIndex;
}

void ClearRegularNormal()
{
	int i = 0;
	for (i = 0; i < MAX_ROUTE_ITEM; i++) {
		if (gencalclistast_normal[i]== NULL) {
			break;
		}
		
		calclist_treefree(gencalclistast_normal[i]);
		gencalclistast_normal[i] = NULL;
	}
}

int main()
{
	std::string test_str = "TERM_ID \"234\"\n";
	printf("add result %d\n", SetRegularNormal(test_str.c_str(), test_str.length(), 0));

	calclist_context_t context;
   	context.amount = 100;
   	context.card_type = 2;
	strcpy(context.term_id, "12345");

	//printf("result: %d\n", GetRegularIndexNormal(context));

	for (int i = 0; i < MAX_ROUTE_ITEM; i++) {
		if (gencalclistast_normal[i] == NULL) {
			break;
		}
		printf("result: %d\n", calclist_eval(gencalclistast_normal[i], &context));
	}


	return 0;
}
