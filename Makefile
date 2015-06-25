SHELL = /bin/sh
CC = g++

INCLUDES = -Iinclude -I/usr/local/include  -I/usr/include -I/usr/include/mysql
LIB_PATH = -L./lib -L/usr/lib -L/usr/local/lib -L/usr/lib64
LDFLAGS  = -lfcgi -L/usr/local/lib -lpthread -lmysqlclient -L/usr/lib64/mysql
CPPFLAGS = -O0 -g --std=c++11 -DBOOMER_DEBUG -DNO_FCGI_DEFINES -DHAVE_NETINET_IN_H
#CPPFLAGS = -O3 -DSHOW_LIMIT  -DNO_FCGI_DEFINES -DHAVE_NETINET_IN_H

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))
all: boomer
%.o: %.cpp
		$(CC) $(CPPFLAGS) -c $< -o $@ $(INCLUDES) $(LIB_PATH)
boomer: $(OBJECTS)
		$(CC) -o $@ $^ $(LIB_PATH) $(LDFLAGS)
clean:
	rm -f $(OBJECTS) boomer
