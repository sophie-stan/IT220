#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

void process(char* in0, char* in1, char* in2, char* out) {
    unsigned short* channel = NULL;
    char* names[3] = {in0, in1, in2};
    pnm ims = pnm_load(in0);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm_free(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    for (int k = 0; k < 3; k ++) {
        ims = pnm_load(names[k]);
        channel = pnm_get_channel(ims, NULL, k);
        pnm_set_channel(imd, channel, k);
        free(channel);
        pnm_free(ims);
    }

    pnm_save(imd, PnmRawPpm, out);
    pnm_free(imd);
}


void usage(char* s) {
    fprintf(stderr, "Usage: %s %s", s, "<i> <j> <rows> <cols> <ims> <imd>\n");
    exit(EXIT_FAILURE);
}

#define PARAM 4

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1) usage(argv[0]);
    char* ims0 = argv[1];
    char* ims1 = argv[2];
    char* ims2 = argv[3];
    char* out = argv[4];
    process(ims0, ims1, ims2, out);
    return EXIT_SUCCESS;
}
