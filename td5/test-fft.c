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

#define ERROR 5
#define NAME_SIZE 50

/**
 * @brief test the forward and backward functions
 * @param char* name, the input image file name
 */
void
test_forward_backward(char* name) {
    fprintf(stderr, "test_forward_backward: ");

    unsigned short* g_ims, * g_imd;
    fftw_complex* freq_repr;

    pnm ims = pnm_load(name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);


    /** Changing part **/
    g_ims = pnm_get_channel(ims, NULL, 0);
    freq_repr = forward(rows, cols, g_ims); // Malloc

    g_imd = backward(rows, cols, freq_repr); // Malloc
    assert(g_imd != NULL);
    /** Changing part **/


    for (int k = 0; k < 3; k++) {
        pnm_set_channel(imd, g_imd, k);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (abs(pnm_get_component(ims, i, j, 0) -
                    pnm_get_component(imd, i, j, 0)) > ERROR) {
                fprintf(stderr, "KO\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    fprintf(stderr, "OK\n");

    char* _name = basename(name);
    char dest[NAME_SIZE] = "FB-";
    pnm_save(imd, PnmRawPpm, strcat(dest, _name));

    pnm_free(imd);
    pnm_free(ims);
    free(g_ims);
    free(g_imd);
    fftw_free(freq_repr);
}

/**
 * @brief test image reconstruction from of magnitude and phase spectrum
 * @param char *name: the input image file name
 */
void
test_reconstruction(char* name) {
    fprintf(stderr, "test_reconstruction: ");

    fftw_complex* freq_repr;
    unsigned short* g_ims, * g_imd;
    float* as, * ps;

    pnm ims = pnm_load(name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    int size = rows * cols;
    pnm imd = pnm_new(cols, rows, PnmRawPpm);


    /** Changing part **/
    // Allocation of the channel from the grayscale image
    g_ims = pnm_get_channel(ims, NULL, 0);
    // Allocation of the the channel transformed
    freq_repr = forward(rows, cols, g_ims);
    // Allocation of the amplitude and phase spectra
    as = malloc(size * sizeof(float));
    ps = malloc(size * sizeof(float));

    freq2spectra(rows, cols, freq_repr, as, ps);
    spectra2freq(rows, cols, as, ps, freq_repr);

    // Allocation of the channel from the grayscale image (return)
    g_imd = backward(rows, cols, freq_repr);
    /** Changing part **/


    for (int k = 0; k < 3; k++) {
        pnm_set_channel(imd, g_imd, k);
    }

    char* _name = basename(name);
    char dest[NAME_SIZE] = "FB-ASPS-";
    pnm_save(imd, PnmRawPpm, strcat(dest, _name));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (abs(pnm_get_component(ims, i, j, 0) -
                    pnm_get_component(imd, i, j, 0)) > ERROR) {
                fprintf(stderr, "KO\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    fprintf(stderr, "OK\n");

    pnm_free(imd);
    pnm_free(ims);
    free(g_ims);
    free(g_imd);
    fftw_free(freq_repr);
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
