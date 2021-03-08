#include <stdlib.h>
#include <stdio.h>
#include <pnm.h>

int max(int a, int b) {
    return a > b ? a : b;
}

void process(int dx, int dy, char* ims_name, char* imd_name) {
    pnm ims = pnm_load(ims_name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    /* Black template */
    unsigned short* data = pnm_get_image(imd);
    unsigned short* p = data;
    size_t size = rows * cols * 3;
    while (size--) *p++ = 0;

    /* Filling up only if the given offsets are not too big
     * compared with the image's size
     */
    if (abs(dx) <= cols && abs(dy) <= rows) {
        int rows_bis = rows - abs(dy);
        int cols_bis = cols - abs(dx);
        int ims_i = max(0, -dy);
        int ims_j = max(0, -dx);
        int imd_i = max(0, dy);
        int imd_j = max(0, dx);
        // Let us run through ims (input)
        for (int i = 0; i < rows_bis; i ++) {
            for (int j = 0; j < cols_bis; j ++) {
                for (int k = 0; k < 3; k ++) {
                    unsigned short value = pnm_get_component(ims, ims_i + i, ims_j + j, k);
                    pnm_set_component(imd, imd_i + i, imd_j + j, k, value);
                }
            }
        }
    }



    pnm_save(imd, PnmRawPpm, imd_name);
    pnm_free(ims);
    pnm_free(imd);
}

void usage(char* s) {
    fprintf(stderr, "%s <dx> <dy> <ims> <imd>\n", s);
    exit(EXIT_FAILURE);
}

#define PARAM 4

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1)
        usage(argv[0]);
    int dx = atoi(argv[1]);
    int dy = atoi(argv[2]);
    char* ims_name = argv[3];
    char* imd_name = argv[4];

    process(dx, dy, ims_name, imd_name);

    return EXIT_SUCCESS;
}
