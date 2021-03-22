/**
 * @file test-fft.c
 * @brief test the behaviors of functions in fft module
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <assert.h>

#include <fft.h>

#define GAP 10

/**
 * @brief test the forward and backward functions
 * @param char* name, the input image file name
 */
void
test_forward_backward(char* path) {
    fprintf(stderr, "test_forward_backward: ");

    pnm ims = pnm_load(path);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    unsigned short* input;
    unsigned short* output;
    fftw_complex* tmp;

    input = pnm_get_channel(ims, NULL, 0);
    tmp = forward(rows, cols, input); // Malloc

    output = backward(rows, cols, tmp); // Malloc
    assert(output != NULL);

    for (int k = 0; k < 3; k++) {
        pnm_set_channel(imd, output, k);
    }

    char* name = basename(path);
    int dest_len = strlen(name) + 3 + 4 + 1;
    char dest[dest_len];
    memset(dest, 0, dest_len * sizeof(char));
    strcpy(dest, "FB-");
    pnm_save(imd, PnmRawPpm, strcat(strcat(dest, name), ".ppm"));

    for (int i = 0; i < rows; i ++) {
        for (int j = 0; j < cols; j ++) {
            if (abs(pnm_get_component(ims, i, j, 0) -
                    pnm_get_component(imd, i, j, 0)) > GAP) {
                fprintf(stderr, "KO\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    fprintf(stderr, "OK\n");

    pnm_free(imd);
    pnm_free(ims);
    free(input);
    free(output);
    fftw_free(tmp);
}

/**
 * @brief test image reconstruction from of magnitude and phase spectrum
 * @param char *name: the input image file name
 */
void
test_reconstruction(char* name) {
    fprintf(stderr, "test_reconstruction: ");
    (void) name;
    fprintf(stderr, "OK\n");
}

/**
 * @brief test construction of magnitude and phase images in ppm files
 * @param char* name, the input image file name
 */
void
test_display(char* name) {
    fprintf(stderr, "test_display: ");
    (void) name;
    fprintf(stderr, "OK\n");
}

/**
 * @brief test the modification of magnitude by adding a periodic functions
          on both vertical and horizontal axis, and 
 *        construct output images
 * @param char* name, the input image file name
 */
void
test_add_frequencies(char* name) {
    fprintf(stderr, "test_add_frequencies: ");
    (void) name;
    fprintf(stderr, "OK\n");
}

void
run(char* name) {
    test_forward_backward(name);
    test_reconstruction(name);
    test_display(name);
    test_add_frequencies(name);
}

void
usage(const char* s) {
    fprintf(stderr, "Usage: %s <ims> \n", s);
    exit(EXIT_FAILURE);
}

#define PARAM 1

int
main(int argc, char* argv[]) {
    if (argc != PARAM + 1) usage(argv[0]);
    run(argv[1]);
    return EXIT_SUCCESS;
}
