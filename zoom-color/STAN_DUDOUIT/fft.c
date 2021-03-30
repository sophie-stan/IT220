#include <float.h>
#include <stdlib.h>
#include <math.h>

#include <fft.h>

void translation(int rows, int cols, fftw_complex* freq_repr) {
    fftw_complex* p = freq_repr;
    for (int i = 0; i < rows; i ++) {
        for (int j = 0; j < cols; j ++) {
            *p = (i + j) % 2 != 0 ? -*p : *p;
            //printf("%f\n", creal(*p));
        p++;
        }
    }
}

fftw_complex* forward(int rows, int cols, unsigned short* g_img) {
    int size = rows * cols;

    // 1) Complex image construction
    fftw_complex* spatial_repr = fftw_alloc_complex(size);
    for (int i = 0; i < size; i++) {
        double tmp = (double) g_img[i];
        spatial_repr[i] = tmp + I * 0;
    }

    translation(rows, cols, spatial_repr);

    // 2) Structure allocation aimed to receive the result of the transform
    fftw_complex* freq_repr = fftw_alloc_complex(size);


    // 3) Initialisation + Fourier transform
    fftw_plan plan = fftw_plan_dft_2d(rows, cols, spatial_repr, freq_repr,
                                      FFTW_FORWARD,
                                      FFTW_ESTIMATE);
    fftw_execute(plan);

    // 4) Memory free
    fftw_free(spatial_repr);
    fftw_destroy_plan(plan);
    fftw_cleanup();

    return freq_repr;
}

unsigned short*
backward(int normalisation, int rows, int cols, fftw_complex* freq_repr) {
    int size = rows * cols;

    // 1) Structure allocation aimed to receive the result of the inv transform
    fftw_complex* spatial_repr = fftw_alloc_complex(size);

    // 2) Initialisation + Fourier transform
    fftw_plan plan = fftw_plan_dft_2d(rows, cols, freq_repr, spatial_repr,
                                      FFTW_BACKWARD,
                                      FFTW_ESTIMATE);
    fftw_execute(plan);

    // 3) Real part extracted
    // Normalization of the result (fftw3_execute doesn't normalize)
    for (int i = 0; i < size; i++) {
        spatial_repr[i] =
                (spatial_repr[i] * normalisation * normalisation) / size;
    }

    translation(rows, cols, spatial_repr);

    // Conversion of floats to unsigned short
    unsigned short* g_img = malloc(size * sizeof(unsigned short));
    for (int i = 0; i < size; i++) {
        g_img[i] = (unsigned short) creal(spatial_repr[i]);
        if (g_img[i] > 255) {
            g_img[i] = 0;
        }
    }

    // 4) Memory free
    fftw_free(spatial_repr);
    fftw_destroy_plan(plan);
    fftw_cleanup();
    return g_img;
}

