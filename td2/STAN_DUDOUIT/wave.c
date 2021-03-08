#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>

#include "domain.h"

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif


void process(int t_x, int a_x, int t_y, int a_y, char* ims_name, char* imd_name) {


    pnm ims = pnm_load(ims_name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    if (t_x == 0 || t_y == 0) {
        printf("t_x or t_y: wrong value\n");
        exit(EXIT_FAILURE);
    }
    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    // Running through imd, the ouput (x' y')
    for (int y_bis = 0; y_bis < rows; y_bis++) {
        for (int x_bis = 0; x_bis < cols; x_bis++) {
            for (int k = 0; k < 3; k++) {
                unsigned short value;
                float x = x_bis + a_x*sin(2*M_PI*y_bis/t_x);
                float y = y_bis + a_y*sin(2*M_PI*x_bis/t_y);
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
    fprintf(stderr, "%s <tx> <ax> <ty> <ay> <ims> <imd>\n", s);
    exit(EXIT_FAILURE);
}

#define PARAM 6

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1)
        usage(argv[0]);

    int t_x = atoi(argv[1]);
    int a_x = atoi(argv[2]);
    int t_y = atoi(argv[3]);
    int a_y = atoi(argv[4]);

    char* ims_name = argv[5];
    char* imd_name = argv[6];

    process(t_x, a_x, t_y, a_y,ims_name, imd_name);

    return EXIT_SUCCESS;
}
