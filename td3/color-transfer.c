/**
 * @file color-transfert
 * @brief transfert color from source image to target image.
 *        Method from Reinhard et al. : 
 *        Erik Reinhard, Michael Ashikhmin, Bruce Gooch and Peter Shirley, 
 *        'Color Transfer between Images', IEEE CGA special issue on 
 *        Applied Perception, Vol 21, No 5, pp 34-41, September - October 2001
 */

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <bcl.h>

#define D 3

float RGB2LMS[D][D] = {
  {0.3811, 0.5783, 0.0402}, 
  {0.1967, 0.7244, 0.0782},  
  {0.0241, 0.1288, 0.8444}
};

float LMS2RGB[D][D] = {
  {4.4679, -3.5873, 0.1193}, 
  {-1.2186, 2.3809, -0.1624},  
  {0.0497, -0.2439, 1.2045}
};

void rgb2lalphabeta(){

}

void lalphabeta2rgb(){

}


void
process(char *ims_name, char *imt_name, char* imd_name){
    pnm ims = pnm_load(ims_name);
    int rows_ims = pnm_get_height(ims);
    int cols_ims = pnm_get_width(ims);
    
    pnm imt = pnm_load(imt_name);
    int rows_imt = pnm_get_height(imt);
    int cols_imt = pnm_get_width(imt);
    
    pnm imd = pnm_new(rows_ims, cols_ims, PnmRawPpm);



}

void
usage (char *s){
  fprintf(stderr, "Usage: %s <ims> <imt> <imd> \n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int
main(int argc, char *argv[]){
  if (argc != PARAM+1) usage(argv[0]);
  process(argv[1], argv[2], argv[3]);
  return EXIT_SUCCESS;
}
