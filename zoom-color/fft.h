/**
 * @file fft.h
 * @brief header for fft module that implements functions for Fourier direct
 *        and inverse transforms and, amplitude and phase spectrum        
 */

#ifndef FFT_H
#define FFT_H

#include <complex.h>
#include <fftw3.h>
#include <bcl.h>

/**
 * @brief perform a discrete 2D Fourier transform from grayscale image
 * @param int rows: the input image height
 * @param int cols: the input image widht
 * @param unsigned short* gray_image: the grayscale input image
 * @return the Fourrier transform in frequency domain
 */
extern fftw_complex 
*forward(int rows, int cols, unsigned short *gray_image);

/**
 * @brief perform a discrete inverse 2D Fourier transform from freqency domain
 * @param int rows: the input complex height
 * @param int cols: the input complex width
 * @param fft_complex* freq_repr: the frequency complex input 
 * @return the grayscale image from inverse Fourier transform
 */
extern unsigned short 
*backward(int normalisation, int rows, int cols, fftw_complex *freq_repr);

#endif /* FFT_H */
