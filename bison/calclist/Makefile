all:calc

CC=g++
CXXFLAGS=-g
CFLAGS=-g

calc:  calclist.tab.o calclist.lex.o calclistfuncs.o
	$(CC) -g -o $@ $^

calclist.tab.o: calclist.ypp
	bison -v -r all --defines=calclist.tab.h --report-file=x.out -t -d $^
#	mv calclist.tab.c calclist.tab.cpp
	./add_yyparse_declare.sh
	$(CC) -c -g -o $@ calclist.tab.cpp

clean:
	rm -f *.o
