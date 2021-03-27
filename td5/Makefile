ROOT=../bcl

CPPFLAGS = -I. -I$(ROOT)/include 
CFLAGS   = -Wall -Wextra -pedantic -std=c99 #-Werror
LDFLAGS  = -L$(ROOT)/lib
LDLIBS   = -lbcl -lfftw3 -lm

BIN = test-fft

.PHONY: all
all: $(BIN)

test-fft: test-fft.o fft.o

test: test-fft
	./test-fft ../data/lena-gray.ppm
show: test
	pvisu FREQ-lena-gray.ppm

.PHONY: clean 
clean:
	$(RM) *.o *~ *.ppm
.PHONY: cleanall
cleanall: clean
	$(RM) $(BIN) 


