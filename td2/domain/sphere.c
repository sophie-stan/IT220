#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>

#include "domain.h"

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif


void process(char* ims_name, char* imd_name) {

    pnm ims = pnm_load(ims_name);

    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);

    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    // Running through imd, the ouput (x' y')
    for (int y_bis = 0; y_bis < rows; y_bis++) {
        for (int x_bis = 0; x_bis < cols; x_bis++) {
            for (int k = 0; k < 3; k++) {
                unsigned short value;
                /*
                float longitude = asin(((float) x_bis)/((float) cols));
                float latitude = asin(((float) y_bis)/((float) rows));
                printf("longitude=%f,latitude=%f\n",longitude,latitude);
                float x = (cols*longitude)/(2*M_PI);
                float y = log(tan(M_PI/4.+latitude/2.));
                printf("x=%f,y=%f\n",x,y);
                */
                float x = cols*sin((2.*M_PI*x_bis)/((float) cols));
                float y = cols*sin(2*(atan(exp(y_bis)-M_PI/4.)));

                value = bilinear_interpolation(x, y, ims, k);
                pnm_set_component(imd, y_bis, x_bis, k, value);
            }
        }
    }

    pnm_save(imd, PnmRawPpm, imd_name);
    pnm_free(ims);
    pnm_free(imd);
}

static void usage(char* s) {
    fprintf(stderr, "%s <ims> <imd>\n", s);
    exit(EXIT_FAILURE);
}

#define PARAM 2

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1)
        usage(argv[0]);

    char* ims_name = argv[1];
    char* imd_name = argv[2];

    process(ims_name, imd_name);

    return EXIT_SUCCESS;
}
