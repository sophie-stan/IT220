#include <float.h>
#include <stdlib.h>
#include <math.h>

#include <fft.h>

fftw_complex*
forward(int rows, int cols, unsigned short* g_img) {
    fftw_complex* spatial_repr, * p_spatial;
    fftw_complex* freq_repr;
    unsigned short* p_gray;
    fftw_plan plan;

    // 1) Complex image construction
    spatial_repr = fftw_malloc(sizeof(fftw_complex) * rows * cols);
    p_spatial = spatial_repr;
    p_gray = g_img;
    for (int i = 0; i < rows * cols; i++) {
        (*p_spatial++) = (*p_gray++) + I * 0;
    }

    // 2) Structure allocation aimed to receive the result of the transform
    freq_repr = fftw_malloc(sizeof(fftw_complex) * rows * cols);

    // 3) Initialisation + Fourier transform
    plan = fftw_plan_dft_2d(rows, cols, spatial_repr, freq_repr,
                            FFTW_FORWARD,
                            FFTW_ESTIMATE); // This creates mem leak
    fftw_execute(plan);

    // 4) Memory free
    fftw_free(spatial_repr);
    fftw_destroy_plan(plan);
    return freq_repr;
}

unsigned short*
backward(int rows, int cols, fftw_complex* freq_repr) {
    fftw_complex* spatial_repr, * p_spatial;
    fftw_plan plan;
    unsigned short* g_img, * p_gray;

    // 1) Structure allocation aimed to receive the result of the inv transform
    spatial_repr = fftw_malloc(sizeof(fftw_complex) * rows * cols);

    // 2) Initialisation + Fourier transform
    plan = fftw_plan_dft_2d(rows, cols, freq_repr, spatial_repr,
                            FFTW_BACKWARD,
                            FFTW_ESTIMATE); // This creates mem leak
    fftw_execute(plan);

    // 3) Real part extracted
    g_img = malloc(rows * cols * sizeof(unsigned short));
    p_gray = g_img;
    p_spatial = spatial_repr;
    for (int i = 0; i < rows * cols; i++) {
        (*p_gray++) = creal((*p_spatial++)) /
                      (rows * cols); // fftw3 doesn't normalize
    }

    // 4) Memory free
    fftw_free(spatial_repr);
    fftw_destroy_plan(plan);

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
        if(re != 0)
            ps[i] = atan2f(im, re);
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
