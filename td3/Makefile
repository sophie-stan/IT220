ROOT = ../bcl

CPPFLAGS = -I$(ROOT)/include 
CFLAGS   = -Wall -Wextra -std=c99 #-Werror 
LDFLAGS  = -L$(ROOT)/lib
LDLIBS   = -lbcl -lm

BIN=color-transfer

all: $(BIN)

clean:
	$(RM) $(BIN) *.ppm

.PHONY: all clean

