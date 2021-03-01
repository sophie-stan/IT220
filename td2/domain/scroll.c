#include <stdlib.h>
#include <stdio.h>
#include <pnm.h>

int max(int a, int b) {
    return a > b ? a : b;
}
int abs(int a) {
    return a >= 0 ? a : -a;
}

void one_process(int dx, int dy, pnm ims, pnm imd) {
    /* Computing the size of the piece of image */
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    int rows_bis = rows - abs(dy);
    int cols_bis = cols - abs(dx);

    /* Computing the coordinates of the leftmost and highest point of ims
     * where the piece of image is truncated.
     * */
    int ims_i = max(0, -dy);
    int ims_j = max(0, -dx);
    /*
     * Computing the coordinates of the leftmost and highest point of imd
     * where the piece of image begins.
     */
    int imd_i = max(0, dy);
    int imd_j = max(0, dx);

    /* Let us run through ims (input) */
    for (int i = 0; i < rows_bis; i ++) {
        for (int j = 0; j < cols_bis; j ++) {
            for (int k = 0; k < 3; k ++) {
                unsigned short value = pnm_get_component(ims, ims_i + i, ims_j + j, k);
                pnm_set_component(imd, imd_i + i, imd_j + j, k, value);
            }
        }
    }
}

void process(int dx, int dy, char* ims_name, char* imd_name) {
    pnm ims = pnm_load(ims_name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    int dx_mod = dx % cols;
    int dy_mod = dy % rows;

    /* First piece of image */
    one_process(dx_mod, dy_mod, ims, imd);

    /*
     * Now there can be 3 cases :
     * 1) dx_mod = 0 && dy_mod = 0 --> nop
     * 2) (dx_mod != 0 && dy_mod == 0) || (dy_mod != 0 && dx_mod == 0) --> one other piece of image
     * 3) (dx_mod != 0 && dy_mod != 0) --> three other pieces of image
     */

    // 2)
    if (dx_mod != 0 && dy_mod == 0) {
        // complementary vector and opposite
        int dx2 = -(cols - abs(dx_mod)) * dx_mod / abs(dx_mod);
        one_process(dx2, 0, ims, imd);
    }
    if (dy_mod != 0 && dx_mod == 0) {
        int dy2 = -(rows - abs(dy_mod)) * dy_mod / abs(dy_mod);
        one_process(0, dy2, ims, imd);
    }

    // 3)
    if (dx_mod != 0 && dy_mod != 0) {
        int dx2 = -(cols - abs(dx_mod)) * dx_mod / abs(dx_mod);
        one_process(dx2, dy, ims, imd);
        int dy2 = -(rows - abs(dy_mod)) * dy_mod / abs(dy_mod);
        one_process(dx, dy2, ims, imd);
        one_process(dx2, dy2, ims, imd);
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
