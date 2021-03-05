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

.PHONY: all
all: $(BIN)

shear: domain.o

rotate: domain.o

test-shear: shear
	./shear h 30 ../data/pepper.ppm a.ppm && pvisu a.ppm;
	./shear h -30 ../data/pepper.ppm b.ppm && pvisu b.ppm;
	./shear v 30 ../data/pepper.ppm c.ppm && pvisu c.ppm;
	./shear v -30 ../data/pepper.ppm d.ppm && pvisu d.ppm;

test-rotate: rotate
	./rotate 0 0 50 ../data/pepper.ppm a.ppm && pvisu a.ppm;
	./rotate 0 0 -50 ../data/pepper.ppm b.ppm && pvisu b.ppm;
	./rotate 50 50 -50 ../data/pepper.ppm c.ppm && pvisu c.ppm;
	./rotate 128 64 -30 ../data/pepper.ppm d.ppm && pvisu d.ppm;

.PHONY: clean cleanall
clean:
	$(RM) *.o 
cleanall: clean
	$(RM) $(BIN) *.ppm
