ROOT=../bcl

CPPFLAGS = -I$(ROOT)/include -I. 
CFLAGS   = -Wall -Wextra  -pedantic -std=c99 #-Werror
LDFLAGS  = -L$(ROOT)/lib 
LDLIBS   = -lbcl -lm -lfftw3

BIN=copy padding filter

.PHONY: all
all: $(BIN)

padding: fft.o

copy: fft.o

filter: fft.o

test-copy: copy
	./copy 10 ../data/cameraman.ppm a.ppm && pvisu a.ppm
	./copy 15 ../data/cameraman.ppm b.ppm && pvisu b.ppm

test-padding: padding
	./padding 10 ../data/cameraman.ppm c.ppm && pvisu c.ppm
	./padding 15 ../data/cameraman.ppm d.ppm && pvisu d.ppm

test-filter: filter
	./filter 15 box ../data/cameraman.ppm e.ppm && pvisu e.ppm
	./filter 15 tent ../data/cameraman.ppm f.ppm && pvisu f.ppm
	./filter 15 bell ../data/cameraman.ppm g.ppm && pvisu g.ppm
	./filter 15 mitch ../data/cameraman.ppm h.ppm && pvisu h.ppm
	
.PHONY: clean cleanall
clean:
	$(RM) $(BIN) *.o *.ppm

