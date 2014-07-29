CC=g++
INCDIR=include
SRCDIR=src
OUTDIR=unix/release
MAKEDIR=@mkdir -p $(@D)
#CFLAGS=-c -Wall -O3 -I$(INCDIR) -I../wjelement/include -I../boost_1_42_0
CFLAGS=-c -Wall -g -DDEBUG -I$(INCDIR) -I../wjelement/include -I../boost_1_42_0

all: $(OUTDIR)/libwjelement++.a

$(OUTDIR)/libwjelement++.a: $(OUTDIR)/wjelement++.o
	$(MAKEDIR)
	ar rvs $@ $(OUTDIR)/wjelement++.o

$(OUTDIR)/wjelement++.o: $(SRCDIR)/wjelement++.cpp
	$(MAKEDIR)
	$(CC) $(CFLAGS) $< -o $(OUTDIR)/wjelement++.o

clean:
	rm $(OUTDIR)/*
