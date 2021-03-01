#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>

#include "domain.h"

//void
//process(...){
//
//}

static void usage(char* s){
  fprintf(stderr,"%s <dir>{h ,v} <angle> <ims> <imd>\n",s);
  exit(EXIT_FAILURE);
}

#define PARAM 4
int main(int argc, char* argv[]){
  if(argc != PARAM+1) 
    usage(argv[0]);

  /* 
     just for the test, completely useless for the shear program 
  */
  for(int channel=0; channel<3; channel++){
    unsigned short v = bilinear_interpolation(0.0, 0.0, NULL, channel);
    (void)v;
  }

  return EXIT_SUCCESS;
}
