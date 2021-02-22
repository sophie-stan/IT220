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

    /*
    pnm ims0 = pnm_load(in0);
    pnm ims1 = pnm_load(in1);
    pnm ims2 = pnm_load(in2);

    int rows = pnm_get_height(ims0);
    int cols = pnm_get_width(ims0);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    unsigned short* channel0 = pnm_get_channel(ims0, NULL, 0);
    unsigned short* channel1 = pnm_get_channel(ims1, NULL, 1);
    unsigned short* channel2 = pnm_get_channel(ims2, NULL, 2);

    pnm_set_channel(imd, channel0, 0);
    pnm_set_channel(imd, channel1, 1);
    pnm_set_channel(imd, channel2, 2);

    pnm_save(imd, PnmRawPpm, out);

    pnm_free(ims0);
    pnm_free(ims1);
    pnm_free(ims2);
    pnm_free(imd);
    free(channel0);
    free(channel1);
    free(channel2);
     */
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
