#!/bin/bash
TARGET_FILE=calclist.tab.h

#插入头
sed -i "1i#ifndef CALCLIST_TAB_H"  ${TARGET_FILE}
sed -i "2i#define CALCLIST_TAB_H"  ${TARGET_FILE}

#插入尾
cat <<  EOF  >> ${TARGET_FILE}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


#endif
EOF
