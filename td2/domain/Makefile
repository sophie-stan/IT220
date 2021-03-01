ROOT = ../bcl

CPPFLAGS = -I$(ROOT)/include -I.
CFLAGS   = -Wall -Wextra -std=c99 -Werror 
LDFLAGS  = -L$(ROOT)/lib 
LDLIBS   = -lbcl -lm

BIN = \
	move\
	shear\
	scroll\
	flip


.PHONY: all
all: $(BIN)

shear: domain.o

.PHONY: clean cleanall
clean:
	$(RM) *.o 
cleanall: clean
	$(RM) $(BIN) *.ppm
