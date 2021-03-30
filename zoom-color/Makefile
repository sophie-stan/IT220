ROOT=../bcl

CPPFLAGS = -I$(ROOT)/include -I. 
CFLAGS   = -Wall -Wextra  -pedantic -std=c99 #-Werror
LDFLAGS  = -L$(ROOT)/lib 
LDLIBS   = -lbcl -lm -lfftw3

BIN=copy padding filter

.PHONY: all
all: $(BIN)

padding: fft.o

test: padding copy filter
	./filter 15 box ../data/lena-small-rect.ppm a.ppm && pvisu a.ppm &&\
	./filter 15 tent ../data/lena-small-rect.ppm b.ppm && pvisu b.ppm &&\
	./filter 15 bell ../data/lena-small-rect.ppm c.ppm && pvisu c.ppm &&\
	./filter 15 mitch ../data/lena-small-rect.ppm d.ppm && pvisu d.ppm


.PHONY: clean cleanall
clean:
	$(RM) $(BIN) *.o *.ppm
#cleanall: clean
#	$(RM) $(BIN)

