ROOT=../bcl

CPPFLAGS = -I. -I$(ROOT)/include 
CFLAGS   = -Wall -Wextra -Werror -pedantic -std=c99 
LDFLAGS  = -L$(ROOT)/lib
LDLIBS   = -lbcl -lfftw3 -lm

BIN = test-fft

.PHONY: all
all: $(BIN)

test-fft: test-fft.o fft.o

.PHONY: clean 
clean:
	$(RM) *.o *~ *.ppm
.PHONY: cleanall
cleanall: clean
	$(RM) $(BIN) 


