ROOT = ../bcl

CPPFLAGS = -I$(ROOT)/include -I.
CFLAGS   = -Wall -Wextra -Werror -std=c99 
LDFLAGS  = -L$(ROOT)/lib 
LDLIBS   = -lbcl -lm

BIN = \
	move\
	shear

.PHONY: all
all: $(BIN)

shear: domain.o

.PHONY: clean cleanall
clean:
	$(RM) *.o 
cleanall: clean
	$(RM) $(BIN)
