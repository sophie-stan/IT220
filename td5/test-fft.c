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

// Fills imd with g_imd, and compares imd with ims
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


void normalize(float* tab) {
    float* p = tab;
    while (*p++) {
        if (*p > 0) {
            *p = roundf(logf(*p));
        }
        if (*p < 0) {
            *p = 0;
        }
        /*printf("%f\n", *p);*/
    }
}

/**
 * @brief test construction of magnitude and phase images in ppm files
 * @param char* name, the input image file name
 */
void
test_display(char* name) {
    fprintf(stderr, "test_display: ");
    unsigned short* g_ims;
    fftw_complex* freq_repr;
    float* as, * ps;

    pnm ims = pnm_load(name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd_as = pnm_new(cols, rows, PnmRawPpm); // amplitude
    pnm imd_ps = pnm_new(cols, rows, PnmRawPpm); // phase
    g_ims = pnm_get_channel(ims, NULL, 0);

    freq_repr = forward(rows, cols, g_ims);
    as = malloc(rows * cols * sizeof(float));
    ps = malloc(rows * cols * sizeof(float));
    freq2spectra(rows, cols, freq_repr, as, ps);

    normalize(as);
    normalize(ps);

    for (int k = 0; k < 3; k++) {
        pnm_set_channel(imd_as, as, k);
        pnm_set_channel(imd_ps, ps, k);
    }

    char* _name = basename(name);
    char _as[NAME_SIZE] = "AS-";
    char _ps[NAME_SIZE] = "PS-";
    pnm_save(imd_as, PnmRawPpm, strcat(_as, _name));
    pnm_save(imd_ps, PnmRawPpm, strcat(_ps, _name));

    pnm_free(imd_as);
    pnm_free(imd_ps);
    pnm_free(ims);
    free(g_ims);
    free(as);
    free(ps);
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

    unsigned short* g_ims, * g_imd;
    fftw_complex* freq_repr;
    float* as, * ps;

    pnm ims = pnm_load(name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);
    pnm imd_fas = pnm_new(cols, rows, PnmRawPpm); // amplitude
    g_ims = pnm_get_channel(ims, NULL, 0);

    freq_repr = forward(rows, cols, g_ims);
    as = malloc(rows * cols * sizeof(float));
    ps = malloc(rows * cols * sizeof(float));
    freq2spectra(rows, cols, freq_repr, as, ps);

    // Operation on the amplitude and phase
    float* p_as, * p_ps;
    float max = 0;
    p_as = as;
    p_ps = ps;
    while (*p_as++) {
        max = *p_as > max ? *p_as : max;
    }
    printf("%f\n", max);
    int mid_rows = rows / 2;
    int mid_cols = cols / 2;
    as[cols * mid_rows - mid_cols] = 2 * 0.25 * max;
    as[cols * (mid_rows + 8) - mid_cols] = 0.25 * max;
    as[cols * mid_rows - mid_cols + 8] = 0.25 * max;

    spectra2freq(rows, cols, as, ps, freq_repr);
    g_imd = backward(rows, cols, freq_repr);

    for (int k = 0; k < 3; k++) {
        pnm_set_channel(imd, g_imd, k);
    }

    fprintf(stderr, "OK\n");

    char* _name = basename(name);
    char freq[NAME_SIZE] = "FREQ-";
    char fas[NAME_SIZE] = "FAS-";
    pnm_save(imd, PnmRawPpm, strcat(freq, _name));
    pnm_save(imd, PnmRawPpm, strcat(fas, _name));

    pnm_free(imd);
    pnm_free(imd_fas);
    pnm_free(ims);
    free(g_ims);
    free(g_imd);
    free(as);
    free(ps);
    fftw_free(freq_repr);
}

void
run(char* name) {
    //test_forward_backward(name);
    //test_reconstruction(name);
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
