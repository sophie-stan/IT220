ROOT = ../bcl

CPPFLAGS = -I$(ROOT)/include -I.
CFLAGS   = -Wall -Wextra -std=c99 -g #-Werror 
LDFLAGS  = -L$(ROOT)/lib 
LDLIBS   = -lbcl -lm

BIN = \
	move\
	shear\
	scroll\
	flip\
	rotate\
	wave\
	sphere\

.PHONY: all
all: $(BIN)

shear: domain.o

rotate: domain.o

wave: domain.o

sphere: domain.o

test-shear: shear
	./shear h 30 ../data/pepper.ppm a.ppm && pvisu a.ppm;
	./shear h -30 ../data/pepper.ppm b.ppm && pvisu b.ppm;
	./shear v 30 ../data/pepper.ppm c.ppm && pvisu c.ppm;
	./shear v -30 ../data/pepper.ppm d.ppm && pvisu d.ppm;

test-flip: flip
	./flip h ../data/pepper.ppm a.ppm && pvisu a.ppm;
	./flip v ../data/pepper.ppm b.ppm && pvisu b.ppm;
	./flip vh ../data/pepper.ppm c.ppm && pvisu c.ppm;
	./flip hh ../data/pepper.ppm d.ppm && pvisu d.ppm;

test-rotate: rotate
	./rotate 0 0 50 ../data/pepper.ppm a.ppm && pvisu a.ppm;
	./rotate 0 0 -50 ../data/pepper.ppm b.ppm && pvisu b.ppm;
	./rotate 50 50 -50 ../data/pepper.ppm c.ppm && pvisu c.ppm;
	./rotate 128 64 -30 ../data/pepper.ppm d.ppm && pvisu d.ppm;

test-wave: wave
	./wave 120 12 250 15 ../data/lena-color.ppm a.ppm && pvisu a.ppm;
	./wave 128 20 1 1 ../data/lena-color.ppm b.ppm && pvisu b.ppm;
	./wave 1 1 128 20 ../data/lena-color.ppm c.ppm && pvisu c.ppm;

test-sphere: sphere
	./sphere ../data/earth.ppm a.ppm && pvisu a.ppm

.PHONY: clean cleanall
clean:
	$(RM) *.o 
cleanall: clean
	$(RM) $(BIN) *.ppm
