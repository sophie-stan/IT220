#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pnm.h>

float box(float x) {
    if (x >= -0.5 && x < 0.5)
        return 1.0;
    else
        return 0.0;
}

float tent(float x) {
    if (x >= -1.0 && x <= 1.0)
        return 1.0 - fabsf(x);
    else
        return 0.0;
}

float bell(float x) {
    if (fabsf(x) <= 0.5)
        return -x * x + 0.75;
    if (fabsf(x) > 0.5 && fabsf(x) <= 1.5)
        return 0.5 * (fabsf(x) - 1.5) * (fabsf(x) - 1.5);
    else
        return 0.0;
}

float mitch(float x) {
    if (x >= (-1.0) && x <= 1.0)
        return 1.1667 * pow(fabsf(x), 3) - 2.0 * x * x + 0.8889;
    if ((x >= -2.0 && x <= -1.0) || (x >= 1 && x <= 2))
        return -0.3889 * pow(fabsf(x), 3) + 2 * x * x - 3.3333 * fabsf(x) +
               1.7778;
    else
        return 0.0;
}

void algo(pnm ims, pnm imd, int factor,
          char* filter_name, int direction,
          int max, int cols, int rows,
          int c) {

    float new_ind;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (direction == 0) new_ind = (float) j / factor;
            else if (direction == 1) new_ind = (float) i / factor;

            float WF = 0;
            if (strcmp(filter_name, "box") == 0)
                WF = 0.5;
            else if (strcmp(filter_name, "tent") == 0)
                WF = 1.0;
            else if (strcmp(filter_name, "bell") == 0)
                WF = 1.5;
            else if (strcmp(filter_name, "mitch") == 0)
                WF = 2.0;

            float left = new_ind - WF;
            float right = new_ind + WF;

            int l_int = floor(left);
            int r_int = floor(right);

            float S = 0.0;

            for (int k = l_int; k <= r_int; k++) {

                float h = 0.0;
                if (strcmp(filter_name, "box") == 0)
                    h = box((float) k - new_ind);
                else if (strcmp(filter_name, "tent") == 0)
                    h = tent((float) k - new_ind);
                else if (strcmp(filter_name, "bell") == 0)
                    h = bell((float) k - new_ind);
                else if (strcmp(filter_name, "mitch") == 0)
                    h = mitch((float) k - new_ind);

                if (direction == 0) {
                    if (k < 0) {
                        S += pnm_get_component(ims, i, 0, c) * h;
                    } else if (k >= max) {
                        S += pnm_get_component(ims, i, max - 1, c) * h;
                    } else {
                        S += pnm_get_component(ims, i, k, c) * h;
                    }
                } else if (direction == 1) {
                    if (k < 0) {
                        S += pnm_get_component(ims, 0, j, c) * h;
                    } else if (k >= max) {
                        S += pnm_get_component(ims, max - 1, j, c) * h;
                    } else {
                        S += pnm_get_component(ims, k, j, c) * h;
                    }
                }
            }

                pnm_set_component(imd, i, j, c, (unsigned short) S);
        }
    }
}

void process(int factor, char* filter_name, char* ims_name, char* imd_name) {
    pnm ims = pnm_load(ims_name);
    int cols = pnm_get_width(ims);
    int rows = pnm_get_height(ims);

    int new_cols = factor * cols;
    int new_rows = factor * rows;

    pnm imt = pnm_new(new_cols, rows, PnmRawPpm);
    pnm imd = pnm_new(new_cols, new_rows, PnmRawPpm);

    // Columns interpolation
    for (int k = 0; k < 3; k ++) {
        algo(ims, imt, factor, filter_name, 0, cols, new_cols, rows, k);
    }

    // Rows interpolation
    for (int k = 0; k < 3; k ++) {
        algo(imt, imd, factor, filter_name, 1, rows, new_cols, new_rows, k);
    }

    pnm_save(imd, PnmRawPpm, imd_name);
    pnm_free(ims);
    pnm_free(imt);
    pnm_free(imd);
}

void usage(char* s) {
    fprintf(stderr, "Usage: %s <factor> <filter> <ims> <imd> \n", s);
    exit(EXIT_FAILURE);
}

#define param 4

int main(int argc, char* argv[]) {
    if (argc != param + 1) usage(argv[0]);
    int factor = atoi(argv[1]);
    process(factor, argv[2], argv[3], argv[4]);
    return EXIT_SUCCESS;
}
