/**
 * @file copy.c
 * @brief resize a given image with an increasing factor by copying existing
 *        pixels. The resize factor is considered as an interger.
 * @author Vinh-Thong Ta <vinh-thong.ta@labri.fr>
 */

#include <stdio.h>
#include <stdlib.h>

#include <pnm.h>


void usage (char *s){
  fprintf(stderr, "Usage: %s <factor> <ims> <imd> \n", s);
  exit(EXIT_FAILURE);
}

#define param 3
int main(int argc, char *argv[]){
  if (argc != param+1) usage(argv[0]);
  return EXIT_SUCCESS;
}
