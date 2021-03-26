#include <float.h>
#include <stdlib.h>
#include <math.h>

#include <fft.h>

fftw_complex* forward(int rows, int cols, unsigned short* g_img) {

    // 1) Complex image construction
    fftw_complex* spatial_repr = fftw_malloc(sizeof(fftw_complex) * rows * cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            /* The following code works only for test-display (we need a translation)
             *
             * int tmp = (i + j) % 2 == 0 ? g_img[i * cols + j] : -g_img[i * cols +
                                                                      j];
            spatial_repr[i * cols + j] = tmp + I * 0;*/
            spatial_repr[i * cols + j] = g_img[i * cols + j] + I * 0;
        }
    }

    // 2) Structure allocation aimed to receive the result of the transform
    fftw_complex* freq_repr = fftw_malloc(sizeof(fftw_complex) * rows * cols);

    // 3) Initialisation + Fourier transform
    fftw_plan plan = fftw_plan_dft_2d(rows, cols, spatial_repr, freq_repr, FFTW_FORWARD,
                            FFTW_ESTIMATE);
    fftw_execute(plan);

    // 4) Memory free
    fftw_free(spatial_repr);
    fftw_destroy_plan(plan);
    fftw_cleanup();
    return freq_repr;
}

unsigned short* backward(int rows, int cols, fftw_complex* freq_repr) {

    // 1) Structure allocation aimed to receive the result of the inv transform
    fftw_complex* spatial_repr = fftw_malloc(sizeof(fftw_complex) * rows * cols);

    // 2) Initialisation + Fourier transform
    fftw_plan plan = fftw_plan_dft_2d(rows, cols, freq_repr, spatial_repr, FFTW_BACKWARD,
                            FFTW_ESTIMATE);
    fftw_execute(plan);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Problem with the values of spatial_repr
            // (way too big to fit in an unsigned short...)
            spatial_repr[i * cols + j] = creal(spatial_repr[i * cols + j]);
        }
    }

    // 3) Real part extracted
    unsigned short* g_img = malloc(rows * cols * sizeof(unsigned short));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            //printf("%f\n", real[i * cols + j]);
            unsigned short tmp = (unsigned short) real[i * cols + j];
            //printf("%hu\n", tmp);
            g_img[i * cols + j] =
                    tmp / (rows * cols); // fftw3 doesn't normalize
        }
    }

    // 4) Memory free
    fftw_free(spatial_repr);
    fftw_destroy_plan(plan);
    fftw_cleanup();
    return g_img;
}

void freq2spectra(int rows, int cols, fftw_complex* freq_repr, float* as,
                  float* ps) {
    double re, im;
    fftw_complex* p_freq = freq_repr;
    for (int i = 0; i < rows * cols; i++) {
        re = creal(*p_freq);
        im = cimag(*p_freq++);
        as[i] = sqrtf(re * re + im * im);
        if (re != 0)
            ps[i] = atan2f(im, re);
        else
            ps[i] = pi / 2.;
    }
}

void spectra2freq(int rows, int cols, float* as, float* ps,
                  fftw_complex* freq_repr) {
    double re, im;
    fftw_complex* p_freq = freq_repr;
    for (int i = 0; i < rows * cols; i++) {
        re = as[i] * cosf(ps[i]);
        im = as[i] * sinf(ps[i]);
        (*p_freq++) = re + I * im;
    }
}
