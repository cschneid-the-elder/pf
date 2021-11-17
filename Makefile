
CFLAGS=-Wall
LDFLAGS=-lm

pf: pf.c
	cc $(CFLAGS) $< -o $@ $(LDFLAGS)
