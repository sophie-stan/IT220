#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>

#include "domain.h"

void process(char* ims_name, char* imd_name) {
    pnm ims = pnm_load(ims_name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    unsigned short value = bilinear_interpolation(3.4, 7.8, ims, 0);
    (void) value;


    pnm_save(imd, PnmRawPpm, imd_name);
    pnm_free(ims);
    pnm_free(imd);
}

static void usage(char* s) {
    fprintf(stderr, "%s <dir>{h ,v} <angle> <ims> <imd>\n", s);
    exit(EXIT_FAILURE);
}

#define PARAM 4

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1)
        usage(argv[0]);


    char* ims_name = argv[3];
    char* imd_name = argv[4];

    process(ims_name, imd_name);

    /*
       just for the test, completely useless for the shear program
    */
    for (int channel = 0; channel < 3; channel++) {
        unsigned short v = bilinear_interpolation(0.0, 0.0, NULL, channel);
        (void) v;
    }

    return EXIT_SUCCESS;
}
