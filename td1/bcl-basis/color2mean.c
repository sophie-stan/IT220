#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

void process(char* in, char* out) {
    pnm ims = pnm_load(in);
    pnm imd = pnm_dup(ims);

    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);

    for (int p = 0; p < rows; p++) {
        for (int q = 0; q < cols; q++) {
            const unsigned short val = (pnm_get_component(ims, p, q, PnmRed) +
                                        pnm_get_component(ims, p, q, PnmGreen) +
                                        pnm_get_component(ims, p, q, PnmBlue)) /
                                       3;
            for (int k = 0; k < 3; k++) {
                pnm_set_component(imd, p, q, k, val);
            }
        }
    }
    pnm_save(imd, PnmRawPpm, out);

    pnm_free(ims);
    pnm_free(imd);
}

void
usage(char* s) {
    fprintf(stderr, "Usage: %s %s", s, "<ims> <imd>\n");
    exit(EXIT_FAILURE);
}

#define PARAM 2

int
main(int argc, char* argv[]) {
    if (argc != PARAM + 1) usage(argv[0]);
    char* in = argv[1];
    char* out = argv[2];
    process(in, out);
    return EXIT_SUCCESS;
}   
