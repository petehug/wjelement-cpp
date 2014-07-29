CC=g++
MAKEDIR=@mkdir -p $(@D)
BUILDDIR=unix/release

LIBPATH=-L/opt/development/apal6/libs/src/wjelement/unix/release -L/opt/development/apal6/libs/src/wjelement-cpp/unix/release -L/opt/development/apal6/libs/curl-7.35.0/lib -L/opt/development/apal6/libs/boost-1.42.0/lib/
LIBS= -lwjelement++ -lwjelement -lcurl -lboost_regex

#CFLAGS=-c -Wall -O3 -I../include -I../../wjelement/include -I../../boost_1_42_0 -I../../curl-7.35.0/include
CFLAGS=-c -Wall -g -DDEBUG -I../include -I../../wjelement/include -I../../boost_1_42_0 -I../../curl-7.35.0/include
LDFLAGS=$(LIBPATH) $(LIBS)

all: $(BUILDDIR)/wjelement-test

$(BUILDDIR)/wjelement-test: $(BUILDDIR)/wjelement-test.o $(BUILDDIR)/Exception.o
	$(MAKEDIR)
	$(CC) $(BUILDDIR)/wjelement-test.o $(BUILDDIR)/Exception.o -o $(BUILDDIR)/wjelement-test $(LDFLAGS)

$(BUILDDIR)/wjelement-test.o: wjelement-test.cpp
	$(MAKEDIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/Exception.o: Exception.cpp
	$(MAKEDIR)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(BUILDDIR)/*
