#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

/*
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
*/

int min(int a, int b) {
    return a < b ? a : b;
}

void
process(const int i, const int j, const size_t rows, const size_t cols,
        char* in, char* out) {

    /* Prepares the result */
    pnm imd = pnm_new(cols, rows, PnmRawPpm);
    unsigned short* data = pnm_get_image(imd);
    unsigned short* p = data; // another pointer on the image
    size_t size = rows * cols * 3;
    while (size--) *p++ = pnm_maxval;

    pnm ims = pnm_load(in);
    int prev_cols = pnm_get_width(ims);
    int prev_rows = pnm_get_height(ims);
    int min_cols = min(prev_cols - i, cols);
    int min_rows = min(prev_rows - j, rows);

    for (int i1 = 0; i1 < min_rows; i1++) {
        for (int j1 = 0; j1 < min_cols; j1++) {
            for (int k = 0; k < 3; k++) {
                const unsigned short val = pnm_get_component(ims, i + i1, j + j1, k);
                pnm_set_component(imd, i1, j1, k, val);
            }
        }
    }


    pnm_save(imd, PnmRawPpm, out);

    pnm_free(ims);
    pnm_free(imd);
}


void usage(char* s) {
    fprintf(stderr, "Usage: %s %s", s, "<i> <j> <rows> <cols> <ims> <imd>\n");
    exit(EXIT_FAILURE);
}

#define PARAM 6

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1) usage(argv[0]);
    size_t i = atoi(argv[1]);
    size_t j = atoi(argv[2]);
    const int rows = atoi(argv[3]);
    const int cols = atoi(argv[4]);
    char* in = argv[5];
    char* out = argv[6];
    process(i, j, rows, cols, in, out);
    return EXIT_SUCCESS;
}
