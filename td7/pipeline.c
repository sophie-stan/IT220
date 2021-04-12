/**
 * @file pipeline.c
 * @brief just make the IT-220 challenge awesome ! Remember to comment each pipeline 
 * steps
 * @author Vinh-Thong Ta <vinh-thong.ta@labri.fr>
 */

#include <stdio.h>
#include <stdlib.h>

#include <pnm.h>


void usage (char *s){
  fprintf(stderr, "Usage: %s <ims> <imd> \n", s);
  exit(EXIT_FAILURE);
}

#define param 2
int main(int argc, char *argv[]){
  if (argc != param+1) usage(argv[0]);
  return EXIT_SUCCESS;
}
