#include <float.h>
#include <stdlib.h>
#include <math.h>

#include <fft.h>

fftw_complex *
forward(int rows, int cols, unsigned short* g_img)
{
  (void)rows;
  (void)cols;
  (void)g_img;
  return NULL;
}

unsigned short *
backward(int rows, int cols, fftw_complex* freq_repr)
{
  (void)rows;
  (void)cols;
  (void)freq_repr;
  return NULL;
}

void
freq2spectra(int rows, int cols, fftw_complex* freq_repr, float* as, float* ps) 
{
    for (unsigned int i = 0; i < (unsigned int) rows*cols*sizeof(fftw_complex); i++) {
        fftw_complex c = *freq_repr+i;
        double re = creal(c);
        double im = cimag(c);
        as[i] = sqrtf(re * re + im * im);
        ps[i] = atanf(im / re);
    }
}

void 
spectra2freq(int rows, int cols, float* as, float* ps, fftw_complex* freq_repr)
{
    for (unsigned int i = 0; i < (unsigned int) rows*cols*sizeof(fftw_complex); i++) {
        double re = as[i] * cosf(ps[i]);
        double im = as[i] * sinf(ps[i]);
        *(freq_repr+i) = re +  I * im;
    }
}
