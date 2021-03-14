ROOT = ../bcl

CPPFLAGS = -I$(ROOT)/include 
CFLAGS   = -Wall -Wextra -std=c99 -g #-Werror
LDFLAGS  = -L$(ROOT)/lib
LDLIBS   = -lbcl -lm

BIN=color-transfer

all: $(BIN)

clean:
	$(RM) $(BIN) *.ppm

test: all
	./color-transfer ../data/flower.ppm ../data/mountain.ppm a.ppm && pvisu a.ppm
	./color-transfer ../data/ocean.ppm ../data/forest.ppm b.ppm && pvisu b.ppm

.PHONY: all clean

