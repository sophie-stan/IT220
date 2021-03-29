/**
 * @file zero_padding.c
 * @brief resize a given image with an increasing factor with
 *        frequency zero-padding method.
 *        The resize factor is considered as an interger.
 * @author Vinh-Thong Ta <vinh-thong.ta@labri.fr>
 * @date 2010-2011  
 */

#include <stdio.h>
#include <stdlib.h>

#include <pnm.h>
#include "fft.h"

void process(int factor, char* ims_name, char* imd_name) {
    // Loading inputs
    pnm ims = pnm_load(ims_name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);

    int rows_bis = rows * factor;
    int cols_bis = cols * factor;

    pnm imd = pnm_new(cols_bis, rows_bis, PnmRawPpm);
    unsigned short* g_ims = pnm_get_channel(ims, NULL, 0);

    // Computing result with Fourier transform
    fftw_complex* freq_repr = forward(rows, cols, g_ims);

    // Creating the zoom freq repr and filling with zero on the borders
    fftw_complex* zoom_freq_repr = fftw_alloc_complex(rows_bis * cols_bis);
    printf("%d\n", (cols_bis - cols) / 2);
    printf("%d\n", rows_bis / 2 + rows / 2);

    int size = 0;
    int mid_rows = rows / 2;
    int mid_cols = cols / 2;
    int mid_rows_bis = rows_bis / 2;
    int mid_cols_bis = cols_bis / 2;
    fftw_complex* p = zoom_freq_repr;
    for (int i = 0; i < rows_bis; i++) {
        for (int j = 0; j < cols_bis; j++) {
            if ((i >= (mid_rows_bis - mid_rows) && i < (mid_rows_bis + mid_rows)
                 && j >= (mid_cols_bis - mid_cols) && j < (mid_cols_bis + mid_cols))) {
                *p++ = freq_repr[size];
                size++;
            } else {
                *p++ = 0;
            }
        }
    }
    printf("size: %d\n", size);

    // Going back home
    unsigned short* g_imd = backward(factor, rows_bis, cols_bis,
                                     zoom_freq_repr);

    // Filling results
    for (int k = 0; k < 3; k++) {
        pnm_set_channel(imd, g_imd, k);
    }

    // Saving results
    pnm_save(imd, PnmRawPpm, imd_name);

    // Memory free
    pnm_free(ims);
    pnm_free(imd);
    free(g_ims);
    free(g_imd);
    fftw_free(freq_repr);
    fftw_free(zoom_freq_repr);

}

void usage(char* s) {
    fprintf(stderr, "Usage: %s <factor> <ims> <imd> \n", s);
    exit(EXIT_FAILURE);
}

#define param 3

int main(int argc, char* argv[]) {
    if (argc != param + 1) usage(argv[0]);
    int factor = atoi(argv[1]);
    char* ims = argv[2];
    char* imd = argv[3];
    process(factor, ims, imd);

    return EXIT_SUCCESS;
}
