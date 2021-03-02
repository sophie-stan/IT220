#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>

#include "domain.h"

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif


void process(int x_0, int y_0, float angle, char* ims_name, char* imd_name) {
    float radian = angle * M_PI / 180.0;

    pnm ims = pnm_load(ims_name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    if (x_0 < 0 || x_0 > cols || y_0 < 0 || y_0 > rows) {
        printf("x or y: wrong value\n");
        exit(EXIT_FAILURE);
    }
    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    for (int i_bis = 0; i_bis < rows; i_bis ++) {
        for (int j_bis = 0; j_bis < cols; j_bis ++) {
            for (int k = 0; k < 3; k ++) {

            }
        }
    }

    pnm_save(imd, PnmRawPpm, imd_name);
    pnm_free(ims);
    pnm_free(imd);
}

static void usage(char* s) {
    fprintf(stderr, "%s <dir>{h ,v} <angle> <ims> <imd>\n", s);
    exit(EXIT_FAILURE);
}

#define PARAM 5

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1)
        usage(argv[0]);

    int x_0 = atoi(argv[1]);
    int y_0 = atoi(argv[2]);

    float angle = atof(argv[3]); // degree

    char* ims_name = argv[4];
    char* imd_name = argv[5];

    process(x_0, y_0, angle, ims_name, imd_name);

    return EXIT_SUCCESS;
}
