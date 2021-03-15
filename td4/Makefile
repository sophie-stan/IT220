ROOT = ../bcl

CPPFLAGS = -I$(ROOT)/include 
CFLAGS   = -Wall -Wextra -std=c99 -g #-Werror
LDFLAGS  = -L$(ROOT)/lib
LDLIBS   = -lbcl -lm

BIN=colorization

all: $(BIN)

clean:
	$(RM) $(BIN) *.ppm

test: all
	./colorization ../data/tree-c.ppm ../data/tree-g.ppm a.ppm && pvisu a.ppm
	./colorization ../data/insect-c.ppm ../data/insect-g.ppm b.ppm && pvisu b.ppm
	./colorization ../data/sat-c.ppm ../data/sat-g.ppm c.ppm && pvisu c.ppm

.PHONY: all clean
