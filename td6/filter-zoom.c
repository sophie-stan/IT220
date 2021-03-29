#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pnm.h>


void usage (char *s){
  fprintf(stderr, "Usage: %s <factor> <filter> <ims> <imd> \n", s);
  exit(EXIT_FAILURE);
}

#define param 4
int main(int argc, char *argv[]){
  if (argc != param+1) usage(argv[0]);
  return EXIT_SUCCESS;
}
