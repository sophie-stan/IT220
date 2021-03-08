#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>

#include "domain.h"

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif


void process(char dir, float angle, char* ims_name, char* imd_name) {
    float radian = angle * M_PI / 180.0;

    /* For some reasons, all 180 multiples (except 0) take a loooot
     * more time than other angle values.
     * Because tangent is  periodic, I stated that
     */
    if (fmod(angle, 180) == 0) {
        radian = 0;
    }

    pnm ims = pnm_load(ims_name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    int rows_bis, cols_bis;

    /* Computing the output's size */
    if (dir == 'h') {
        rows_bis = rows;
        cols_bis = cols + abs((int) floor(rows * tan(radian)));
    } else {
        cols_bis = cols;
        rows_bis = rows + abs((int) floor(cols * tan(radian)));
    }
    pnm imd = pnm_new(cols_bis, rows_bis, PnmRawPpm);

    int is_tan_positive = (tan(radian) >= 0);
    // Let us run through imd (the OUPUT !!)
    for (int i_bis = 0; i_bis < rows_bis; i_bis++) {
        for (int j_bis = 0; j_bis < cols_bis; j_bis++) {
            for (int k = 0; k < 3; k++) {
                unsigned short value;
                /* Let's assume (i',j') are the coordinates in imd
                 * (y, x) are the coordinates in ims
                 *
                 * For an horizontal shear, i'=i=y
                 * To go from ims to imd:
                 * x --> x + tan(theta) * y
                 *
                 * But then the inverse function would be:
                 * j' --> j' - tan(theta) * i' = x
                 *
                 */
                if (dir == 'h') {
                    float x = j_bis - fabs(tan(radian))
                                      * (is_tan_positive ? i_bis : (rows_bis -
                                                                    i_bis));
                    value = bilinear_interpolation(x, i_bis, ims, k);
                } else {
                    float y = i_bis - fabs(tan(radian)) *
                                      (is_tan_positive ? j_bis : (cols_bis -
                                                                  j_bis));
                    value = bilinear_interpolation(j_bis, y, ims, k);
                }
                pnm_set_component(imd, i_bis, j_bis, k, value);
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

#define PARAM 4

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1)
        usage(argv[0]);

    char dir = argv[1][0];
    if (dir != 'h' && dir != 'v') {
        printf("wrong direction\n");
        exit(EXIT_FAILURE);
    }

    float angle = atof(argv[2]); // degree
    if ((int) floor(angle) % 180 == 90) {
        printf("wrong angle value\n");
        exit(EXIT_FAILURE);
    }

    char* ims_name = argv[3];
    char* imd_name = argv[4];

    process(dir, angle, ims_name, imd_name);

    return EXIT_SUCCESS;
}
