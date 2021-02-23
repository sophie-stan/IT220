/**
 * @file test-bcl.c
 * @author Vinh-Thong Ta <ta@labri.fr>
 * @brief ???*
 */

#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

void process(const size_t rows, const size_t cols) {
    // Creates an image: PnmRawPpm is a type
    pnm imd = pnm_new(cols, rows, PnmRawPpm);
    // Gets a reference to image buffer (r1g1b1r2g2b2..)
    unsigned short* data = pnm_get_image(imd);


    unsigned short* p = data; // another pointer on the image
    size_t size = rows * cols * 3;

    /* White template: a.ppm */
    while (size--) *p++ = pnm_maxval; // by default pnm_maxval = 255
    // Saves ppm image (buffer) to file name: Contains 3 channels
    pnm_save(imd, PnmRawPpm, "a.ppm");

    /* Computes the borders of the square and removes red component in it */
    size_t i0 = (size_t) rows / 4;
    size_t i1 = 3 * i0;
    size_t j0 = (size_t) cols / 4;
    size_t j1 = 3 * j0;
    for (size_t i = i0; i < i1; i++) {
        for (size_t j = j0; j < j1; j++) {
            p = data + pnm_offset(imd, i,
                                  j); // Computes image offset at (row, col) pos
            // p  = data + (i*(cols*3)+j*3);
            *p = pnm_maxval - *p; // 255 - 255 at red channel
            p++; // useless
            //*p = pnm_maxval - *p;
        }
    }
    pnm_save(imd, PnmRawPpm, "b.ppm");

    /* Removes all the blue */
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            for (size_t k = 0; k < 3; k++) {
                // Gets a pixel value located at (i,j) from a specific channel
                const unsigned short val = pnm_get_component(imd, i, j, k);
                if (val == pnm_maxval) {
                    // Set a pixel value located at (i,j) to the specific channel blue
                    pnm_set_component(imd, i, j, 2, 0);
                }
            }
        }
    }
    pnm_save(imd, PnmRawPpm, "c.ppm");

    /* Extracts a specified channel from a pnm image and copy it to buffer. If the buffer
    * parameter is NULL, the memory is dynamically allocated (and should be freed) */
    unsigned short* channel = pnm_get_channel(imd, NULL, 1);
    for (size_t i = 0; i < rows * cols; i++) channel[i] = 0;
    pnm_set_channel(imd, channel, 1); // Set a specified channel to pnm image with buffer
    pnm_save(imd, PnmRawPpm, "d.ppm");

    pnm_free(imd);
    free(channel);
}

void usage(const char* s) {
    fprintf(stderr, "Usage: %s <rows> <cols> \n", s);
    exit(EXIT_FAILURE);
}

#define PARAM 2

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1) usage(argv[0]);
    size_t rows = atoi(argv[1]);
    size_t cols = atoi(argv[2]);
    process(rows, cols);
    return EXIT_SUCCESS;
}
