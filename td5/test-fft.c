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

#define ERROR 10 // error allowed between lena before and after transform
#define NAME_SIZE 50 // image's name
#define POW 0.15  // 0.1 works too
#define FREQ 8 // frequency added
#define RATIO 0.2 // RATIO * max_as is the amplitude of the frequency added


/**
 * @brief fills the output img with the res and compares it with the input img
 * @param unsigned short* g_imd, the resulting channel
 * @param pnm imd, the ouput image
 * @param pnm ims, the input image
 */
void fill_and_compare(unsigned short* g_imd, pnm imd, pnm ims) {
    // Fill imd
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    for (int k = 0; k < 3; k++) {
        pnm_set_channel(imd, g_imd, k);
    }

    // Compare imd and ims
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (abs(pnm_get_component(ims, i, j, 0) -
                    pnm_get_component(imd, i, j, 0)) > ERROR) {
                fprintf(stderr, "KO\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/**
 * @brief test the forward and backward functions
 * @param char* name, the input image file name
 */
void test_forward_backward(char* name) {
    fprintf(stderr, "test_forward_backward: ");

    // Loading inputs
    pnm ims = pnm_load(name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);
    unsigned short* g_ims = pnm_get_channel(ims, NULL, 0);

    // Forward backward
    fftw_complex* freq_repr = forward(rows, cols, g_ims);
    unsigned short* g_imd = backward(rows, cols, freq_repr);

    // Verification
    fill_and_compare(g_imd, imd, ims);

    // Saving results
    char* _name = basename(name);
    char dest[NAME_SIZE] = "FB-";
    pnm_save(imd, PnmRawPpm, strcat(dest, _name));

    // Memory free
    pnm_free(imd);
    pnm_free(ims);
    free(g_ims);
    free(g_imd);
    fftw_free(freq_repr);

    fprintf(stderr, "OK\n");
}

/**
 * @brief test image reconstruction from of magnitude and phase spectrum
 * @param char *name: the input image file name
 */
void test_reconstruction(char* name) {
    fprintf(stderr, "test_reconstruction: ");

    // Loading inputs
    pnm ims = pnm_load(name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);
    unsigned short* g_ims = pnm_get_channel(ims, NULL, 0);

    // --Fourier--> freq --> sprectra --> freq --FourrierInv-->
    fftw_complex* freq_repr = forward(rows, cols, g_ims);
    float* as = malloc(rows * cols * sizeof(float));
    float* ps = malloc(rows * cols * sizeof(float));
    freq2spectra(rows, cols, freq_repr, as, ps);
    spectra2freq(rows, cols, as, ps, freq_repr);
    unsigned short* g_imd = backward(rows, cols, freq_repr);

    // Verification
    fill_and_compare(g_imd, imd, ims);

    // Saving results
    char* _name = basename(name);
    char dest[NAME_SIZE] = "FB-ASPS-";
    pnm_save(imd, PnmRawPpm, strcat(dest, _name));

    // Memory free
    pnm_free(imd);
    pnm_free(ims);
    free(g_ims);
    free(g_imd);
    fftw_free(freq_repr);
    free(as);
    free(ps);

    fprintf(stderr, "OK\n");
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
        tab[i] = 255 * pow(tab[i] / max, POW); // pow is smoother
    }
}

/**
 * @brief test construction of magnitude and phase images in ppm files
 * @param char* name, the input image file name
 */
void test_display(char* name) {
    fprintf(stderr, "test_display: ");

    // Loading inputs
    pnm ims = pnm_load(name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd_as = pnm_new(cols, rows, PnmRawPpm);
    pnm imd_ps = pnm_new(cols, rows, PnmRawPpm);
    unsigned short* g_ims = pnm_get_channel(ims, NULL, 0);

    // Fourier transform
    fftw_complex* freq_repr = forward(rows, cols, g_ims);
    float* as = malloc(rows * cols * sizeof(float));
    float* ps = malloc(rows * cols * sizeof(float));
    freq2spectra(rows, cols, freq_repr, as, ps);

    // Normalization of the result
    normalize(rows * cols, as);
    normalize(rows * cols, ps);

    // Conversion of the result (floats) to unsigned short
    unsigned short* _as = malloc(rows * cols * sizeof(unsigned short));
    unsigned short* _ps = malloc(rows * cols * sizeof(unsigned short));
    for (int i = 0; i < rows * cols; i++) {
        _as[i] = as[i];
        _ps[i] = ps[i];
    }

    // Filling the pnm images as and ps with the results
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
    fftw_free(freq_repr);
    free(as);
    free(_as);
    free(ps);
    free(_ps);

    fprintf(stderr, "OK\n");
}

/**
 * @brief test the modification of magnitude by adding a periodic functions
          on both vertical and horizontal axis, and 
 *        construct output images
 * @param char* name, the input image file name
 */
void test_add_frequencies(char* name) {
    fprintf(stderr, "test_add_frequencies: ");

    // Loading inputs
    pnm ims = pnm_load(name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd_freq = pnm_new(cols, rows, PnmRawPpm);
    pnm imd_as = pnm_new(cols, rows, PnmRawPpm);
    unsigned short* g_ims = pnm_get_channel(ims, NULL, 0);

    // Computing result with Fourier transform
    fftw_complex* freq_repr = forward(rows, cols, g_ims);
    float* as = malloc(rows * cols * sizeof(float));
    float* ps = malloc(rows * cols * sizeof(float));
    freq2spectra(rows, cols, freq_repr, as, ps);

    /** Operation on the amplitude **/
    float max = 0;
    for (int i = 0; i < rows * cols; i++) {
        max = as[i] > max ? as[i] : max;
    }
    int mid_rows = rows / 2;
    int mid_cols = cols / 2;

    // Adding vertical frequency of 0.25 * max amplitude
    as[cols * (mid_rows - FREQ + 1) - mid_cols] = RATIO * max;
    as[cols * (mid_rows + FREQ + 1) - mid_cols] = RATIO * max;

    // Adding horizontal frequency of 0.25 * max amplitude
    as[cols * (mid_rows + 1) - mid_cols - FREQ] = RATIO * max;
    as[cols * (mid_rows + 1) - mid_cols + FREQ] = RATIO * max;

    /** Back to home **/
    spectra2freq(rows, cols, as, ps, freq_repr);
    unsigned short* g_imd = backward(rows, cols, freq_repr);

    // Normalization and conversion to unsigned short of the amplitude
    // (in order to be printed)
    normalize(rows * cols, as);
    unsigned short* _as = malloc(rows * cols * sizeof(unsigned short));
    for (int i = 0; i < rows * cols; i++) {
        _as[i] = as[i];
    }

    // Filling results
    for (int k = 0; k < 3; k++) {
        pnm_set_channel(imd_as, _as, k);
        pnm_set_channel(imd_freq, g_imd, k);
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
    free(ps);
    free(_as);
    fftw_free(freq_repr);

    fprintf(stderr, "OK\n");
}

void
run(char* name) {
    //test_forward_backward(name);
    //test_reconstruction(name);
    //test_display(name);
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
