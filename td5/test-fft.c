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
#include <math.h>

#define ERROR 10
#define NAME_SIZE 50
#define POW 0.15


/**
 * @brief fills the output img with the res and compares it with the input img
 * @param unsigned short* g_imd, the resulting channel
 * @param pnm imd, the ouput image
 * @param pnm ims, the input image
 */
void fill_and_compare(unsigned short* g_imd, pnm imd, pnm ims) {
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
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
}

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
    g_ims = pnm_get_channel(ims, NULL, 0);

    freq_repr = forward(rows, cols, g_ims); // Malloc
    g_imd = backward(rows, cols, freq_repr); // Malloc

    fill_and_compare(g_imd, imd, ims);

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

    unsigned short* g_ims, * g_imd;
    fftw_complex* freq_repr;
    float* as, * ps;

    pnm ims = pnm_load(name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);
    g_ims = pnm_get_channel(ims, NULL, 0);

    freq_repr = forward(rows, cols, g_ims);

    as = malloc(rows * cols * sizeof(float));
    ps = malloc(rows * cols * sizeof(float));
    freq2spectra(rows, cols, freq_repr, as, ps);
    spectra2freq(rows, cols, as, ps, freq_repr);

    g_imd = backward(rows, cols, freq_repr);

    fill_and_compare(g_imd, imd, ims);

    char* _name = basename(name);
    char dest[NAME_SIZE] = "FB-ASPS-";
    pnm_save(imd, PnmRawPpm, strcat(dest, _name));

    pnm_free(imd);
    pnm_free(ims);
    free(g_ims);
    free(g_imd);
    free(as);
    free(ps);
    fftw_free(freq_repr);
}

/**
 * @brief normalize a float channel (rounds and truncates)
 * @param float* tab
 */
void normalize(int size, float* tab) {
    float max = 0.0;
    for (int i = 0; i < size; i++) {
        if (max < tab[i]) {
            max = tab[i];
        }
    }
    if (max == 0.0) max = 1.0;
    for (int i = 0; i < size; i++) {
/*
        tab[i] = logf(tab[i]);
        tab[i] *= 255.0 / logf(max);
*/
        tab[i] = 255 * pow(tab[i] / max, POW);
    }
}

/**
 * @brief test construction of magnitude and phase images in ppm files
 * @param char* name, the input image file name
 */
void test_display(char* name) {
    fprintf(stderr, "test_display: ");

    // Loading inputs
    pnm ims = pnm_load(name); // Alloc
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd_as = pnm_new(cols, rows, PnmRawPpm); // Alloc
    pnm imd_ps = pnm_new(cols, rows, PnmRawPpm); // Alloc
    unsigned short* g_ims = pnm_get_channel(ims, NULL, 0); // Alloc

    // Centering
    //translation(rows, cols, g_ims);

    // Computing result with Fourier transform
    fftw_complex* freq_repr = forward(rows, cols, g_ims); // Alloc
    float* as = malloc(rows * cols * sizeof(float)); // Alloc
    float* ps = malloc(rows * cols * sizeof(float)); // Alloc
    freq2spectra(rows, cols, freq_repr, as, ps);

    // Normalization of the result
    normalize(rows * cols, as);
    normalize(rows * cols, ps);

    // Conversion to unsigned short
    unsigned short* _as = malloc(rows * cols * sizeof(unsigned short));
    unsigned short* _ps = malloc(rows * cols * sizeof(unsigned short));
    for (int i = 0; i < rows * cols; i++) {
        _as[i] = as[i];
        _ps[i] = ps[i];
    }

    // Filling results
    for (int k = 0; k < 3; k++) {
        pnm_set_channel(imd_as, _as, k);
        pnm_set_channel(imd_ps, _ps, k);
    }

    // Saving results
    char* _name = basename(name);
    char as_name[NAME_SIZE] = "AS-";
    char ps_name[NAME_SIZE] = "PS-";
    pnm_save(imd_as, PnmRawPpm, strcat(as_name, _name));
    pnm_save(imd_ps, PnmRawPpm, strcat(ps_name, _name));

    // Memory free
    pnm_free(imd_as);
    pnm_free(imd_ps);
    pnm_free(ims);
    free(g_ims);
    free(as);
    free(_as);
    free(ps);
    free(_ps);
    fftw_free(freq_repr);

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

    // Loading inputs
    pnm ims = pnm_load(name); // Alloc
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd_freq = pnm_new(cols, rows, PnmRawPpm); // Alloc
    pnm imd_as = pnm_new(cols, rows, PnmRawPpm); // Alloc
    unsigned short* g_ims = pnm_get_channel(ims, NULL, 0); // Alloc

    // Computing result with Fourier transform
    fftw_complex* freq_repr = forward(rows, cols, g_ims); // Alloc
    float* as = malloc(rows * cols * sizeof(float)); // Alloc
    float* ps = malloc(rows * cols * sizeof(float)); // Alloc
    freq2spectra(rows, cols, freq_repr, as, ps);

    // Operation on the amplitude
    float max = 0;
    for (int i = 0; i < rows * cols; i++) {
        max = as[i] > max ? as[i] : max;
    }

    //printf("%f\n", max);
    int mid_rows = rows / 2;
    int mid_cols = cols / 2;
    as[cols * (mid_rows - 8) - mid_cols] = 0.25 * max;
    as[cols * (mid_rows + 8) - mid_cols] = 0.25 * max;



    float* copy_of_as = malloc(cols * rows *sizeof(float));
    memcpy(copy_of_as, as, cols * rows * sizeof(float));

    spectra2freq(rows, cols, as, ps, freq_repr);
    unsigned short* g_imd = backward(rows, cols, freq_repr);






    // Normalization of the amplitude
    normalize(rows * cols, copy_of_as);
    // Conversion to unsigned short
    unsigned short* _as = malloc(rows * cols * sizeof(unsigned short));
    for (int i = 0; i < rows * cols; i++) {
        _as[i] = copy_of_as[i];
    }
    // Filling results
    for (int k = 0; k < 3; k++) {
        pnm_set_channel(imd_as, _as, k);
    }







    // Saving results
    char* _name = basename(name);
    char freq_name[NAME_SIZE] = "FREQ-";
    char as_name[NAME_SIZE] = "FAS-";
    pnm_save(imd_freq, PnmRawPpm, strcat(freq_name, _name));
    pnm_save(imd_as, PnmRawPpm, strcat(as_name, _name));

    // Memory free
    pnm_free(imd_freq);
    pnm_free(imd_as);
    pnm_free(ims);
    free(g_ims);
    free(g_imd);
    free(as);
    free(copy_of_as);
    //free(_as);
    free(ps);
    fftw_free(freq_repr);
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
