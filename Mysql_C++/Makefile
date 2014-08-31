CXXFLAGS= -I/usr/include/mysql -L/usr/lib/mysql -g
LDFLAGS=-lmysqlclient
access_test:access_test.cpp
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
