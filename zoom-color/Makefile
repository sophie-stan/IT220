ROOT=../bcl

CPPFLAGS = -I$(ROOT)/include -I. 
CFLAGS   = -Wall -Wextra  -pedantic -std=c99 #-Werror
LDFLAGS  = -L$(ROOT)/lib 
LDLIBS   = -lbcl -lm -lfftw3

BIN=copy padding filter-zoom

.PHONY: all
all: $(BIN)

padding: fft.o

test: padding
	./padding 10 ../data/lena-small-rect.ppm a.ppm && pvisu a.ppm\
	&& ./padding 15 ../data/lena-small-rect.ppm b.ppm && pvisu b.ppm

.PHONY: clean cleanall
clean:
	$(RM) $(BIN) *.o *.ppm
#cleanall: clean
#	$(RM) $(BIN)

