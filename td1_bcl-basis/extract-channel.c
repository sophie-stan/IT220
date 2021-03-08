#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

void process(const int num, char* in, char* out) {
    pnm ims = pnm_load(in);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);

    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    unsigned short* channel = pnm_get_channel(ims, NULL, num);
    for (int k = 0; k < 3; k++) {
        pnm_set_channel(imd, channel, k);
    }

    pnm_save(imd, PnmRawPpm, out);

    pnm_free(ims);
    pnm_free(imd);
    free(channel);
}


void usage(char* s) {
    fprintf(stderr, "Usage: %s %s", s, "<i> <j> <rows> <cols> <ims> <imd>\n");
    exit(EXIT_FAILURE);
}

#define PARAM 3

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1) usage(argv[0]);
    size_t num = atoi(argv[1]);
    char* in = argv[2];
    char* out = argv[3];
    process(num, in, out);
    return EXIT_SUCCESS;
}
