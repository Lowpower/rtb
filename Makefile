SHELL = /bin/sh
CC = g++

INCLUDES = -I./include -I./gen -I./src -I./include/botan\
-I/usr/local/include  -I/usr/include\
LIB_PATH = -L./lib -L/usr/lib -L/usr/local/lib -L/usr/lib64
LDFLAGS  = -lfcgi -L/usr/local/lib -lpthread -lmysqlclient \
-L/usr/lib64/mysql
CPPFLAGS = -O0 -g -DSHOW_LIMIT -DBOOMER_DEBUG -DNO_FCGI_DEFINES -DHAVE_NETINET_IN_H
#CPPFLAGS = -O3 -DSHOW_LIMIT  -DNO_FCGI_DEFINES -DHAVE_NETINET_IN_H

MOBJ= src/main.o
OBJS = src/log.o \
all: bidder test
test: test/aes_test

%.o: %.cc
        $(CC) $(CPPFLAGS) -c $< -o $@ $(INCLUDES) $(LIB_PATH)

bidder: $(MOBJ) $(OBJS) 
        $(CC) -o $@ $^ $(LIB_PATH) $(LDFLAGS)

clean:
	rm -f $(MOBJ) ${OBJS} boomer
