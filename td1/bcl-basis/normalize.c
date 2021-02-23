#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

float
normalize(const float max, const float min, const float vmax, const float vmin,
          const float value) {
    return ((max - min) / (vmax - vmin)) * value +
           (min * vmax - max * vmin) / (vmax - vmin);
}

void transform(const float min, const float max, char* in, char* out) {
    pnm ims = pnm_load(in);
    pnm imd = pnm_dup(ims);

    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);

    float Max[3];
    float Min[3];

    for (int k = 0; k < 3; k++) {
        Max[k] = (float) pnm_get_component(ims, 0, 0, k);
        Min[k] = (float) pnm_get_component(ims, 0, 0, k);
    }

    float Val;
    for (int p = 0; p < rows; p++) {
        for (int q = 0; q < cols; q++) {
            for (int k = 0; k < 3; k++) {
                Val = (float) pnm_get_component(ims, p, q, k);
                if (Val < Min[k])
                    Min[k] = Val;
                if (Val > Max[k])
                    Max[k] = Val;
            }
        }
    }

    for (int p = 0; p < rows; p++) {
        for (int q = 0; q < cols; q++) {
            for (int k = 0; k < 3; k++) {
                const float val = (float) pnm_get_component(ims, p, q, k);
                const float ip = (float) normalize(max, min, Max[k], Min[k],
                                                   val);
                pnm_set_component(imd, p, q, k, ip);
            }
        }
    }
    pnm_save(imd, PnmRawPpm, out);

    pnm_free(ims);
    pnm_free(imd);
}


void usage(char* s) {
    fprintf(stderr, "Usage: %s %s", s, "<min> <max> <ims> <imd>\n");
    exit(EXIT_FAILURE);
}

#define PARAM 4

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1) usage(argv[0]);
    float min = (float) atof(argv[1]);
    float max = (float) atof(argv[2]);
    char* in = argv[3];
    char* out = argv[4];
    transform(min, max, in, out);
    return EXIT_SUCCESS;
}   
