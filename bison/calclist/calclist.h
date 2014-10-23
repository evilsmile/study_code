#ifndef CALCLIST_H
#define CALCLIST_H

#define MAX_ROUTE_ITEM 256
#include<string>
#include<stdint.h>
/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(const char *s, ...);

int yylex (void);

/* nodes in the Abstract Syntax Tree */
typedef struct calclist_ast {
	int nodetype;
	struct calclist_ast* l;
	struct calclist_ast* r;
} calclist_ast_t;

typedef struct calclist_numval {
	int nodetype;			/* type NUMBER */
	int number;
} calclist_numval_t;

typedef struct calclist_stringval {
	int nodetype;			/* type STRING */
	char string[256];
} calclist_stringval_t;

typedef struct calclist_keyval {
	int nodetype;			/* type KEY */
	char key[256];
} calclist_keyval_t;


typedef struct calclist_context {
	char card_bank[12];			//银行编码缩写
	int card_type;				// 卡类型
	int amount;					// 交易金额
	int product_id;				// 产品id
	int pay_type;				// 支付方式
	int app_type;				
	int card_day_amount;		// 卡日支付成功金额
	int card_month_amount;		// 卡月支付成功金额
	int card_month_count;		// 卡月支付次数
	int uid;						
	char uin[128];
	char term_id[48];			//终端id
	char mobile_id[16];
	char card_id[20];
	char bus_mobile_number[11];
	char bind_mobile_number[11];
	char bus_mobile_attri[20];
	char bus_ip_attri[20];
#if 0	
	char valid_time[32];
	char agent_id[32];
	char mcc_code[8];
	char trade[256];
	char merchant_id[32];
	char card_bank[32];
	int card_type;
	int amount;
	char transaction_type[32];
	char time[16];
	int commission;
	int max_commission;
	char mcc_parent_code[16];
	char mcc_grandpa_code[16];
	int day_limit_amount;
	int from_pos;
#endif
} calclist_context_t;

/* build an AST */
calclist_ast_t* new_calclist_ast(int nodetype, calclist_ast_t* l, calclist_ast_t* r);
calclist_ast_t* new_calclist_num(int d);
calclist_ast_t* new_calclist_string(const char* s);
calclist_ast_t* new_calclist_key(const char* k);

/* calclist_evaluate an AST */
int calclist_eval(calclist_ast_t* a, calclist_context_t* context);

int readInputForLexer(char* buffer, int *numBytesRead, int maxBytesToRead);

int TestRegularNormal(const char *pRegular,int nLen);
int SetRegularNormal(const char * pRegular,int nLen,unsigned int nIndex);
int GetRegularIndexNormal(calclist_context_t & context);
void ClearRegularNormal();

#if 0
int SetRegularMaxCommission(const char * pRegular,int nLen,unsigned int nIndex);
int GetRegularIndexMaxCommission(calclist_context_t & context,int nCurAmount);
void ClearRegularMaxCommission();

int SetRegularNormalOfIcCard(const char * pRegular,int nLen,unsigned int nIndex);
int GetRegularIndexNormalOfIcCard(calclist_context_t & context,int nCurAmount);
void ClearRegularNormalOfIcCard();
int SetRegularMaxCommissionOfIcCard(const char * pRegular,int nLen,unsigned int nIndex);
int GetRegularIndexMaxCommissionOfIcCard(calclist_context_t & context,int nCurAmount);
void ClearRegularMaxCommissionOfIcCard();
#endif
/* delete and free an AST */
void calclist_treefree(calclist_ast_t* );

#endif

