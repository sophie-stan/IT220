/**
 * @file zero_padding.c
 * @brief resize a given image with an increasing factor with
 *        frequency zero-padding method.
 *        The resize factor is considered as an interger.
 * @author Vinh-Thong Ta <vinh-thong.ta@labri.fr>
 * @date 2010-2011  
 */

#include <stdio.h>
#include <stdlib.h>

#include <pnm.h>
#include "fft.h"


void usage (char *s){
  fprintf(stderr, "Usage: %s <factor> <ims> <imd> \n", s);
  exit(EXIT_FAILURE);
}

#define param 3
int main(int argc, char *argv[]){
  if (argc != param+1) usage(argv[0]);
  unsigned short* foo = backward(0, 0, 0, NULL);
  (void)foo;
  return EXIT_SUCCESS;
}
