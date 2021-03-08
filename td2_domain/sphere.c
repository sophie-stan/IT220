#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>

#include "domain.h"

/* We are conscious that sphere should use abilinear interpolation and an inverse
 * transformation. But we already had trouble finding the formula for ims to imd.
 */

void process(char* ims_name, char* imd_name) {
    pnm ims = pnm_load(ims_name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    int x, y;
    unsigned short value;
    double dist, ndist, angle, norm_i, norm_i2, norm_j, norm_j2, i_bis, j_bis;

    /* Let us compute the squared norm on i and j for each points
     * So that we can find the distance...
     */
    for (int i = 0; i < rows; i++) {
        norm_i = ((i * 2.0) / rows) - 1.0;
        norm_i2 = norm_i * norm_i;

        for (int j = 0; j < cols; j++) {
            norm_j = ((j * 2.0) / cols) - 1.0;
            norm_j2 = norm_j * norm_j;

            dist = sqrt(norm_i2 + norm_j2);

            if (dist >= 0 && dist <= 1.0) {
                ndist = (dist + (1.0 - sqrt(1.0 - dist * dist))) / 2.0;

                if (ndist <= 1.0) {
                    angle = atan2(norm_j, norm_i);
                    /* Function atan2() takes two arguments: x-coordinate and
                     * y-coordinate, and calculate the angle in radians for
                     * the quadrant. (equivalent to tan-1(y/x))
                     */
                    i_bis = ndist * cos(angle);
                    j_bis = ndist * sin(angle);

                    x = ((i_bis + 1.0) * (rows - 1)) / 2.0;
                    y = ((j_bis + 1.0) * (cols - 1)) / 2.0;

                    if (x < rows && y < cols && x > 0 && y > 0) {
                        for (int k = 0; k < 3; k ++) {
                            value = pnm_get_component(ims, x, y, k);
                            pnm_set_component(imd, i, j, k, value);
                        }
                    }
                }
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
