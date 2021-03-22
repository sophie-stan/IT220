/**
 * @file test-fft.c
 * @brief test the behaviors of functions in fft module
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

#include <fft.h>

#define ERROR 5

/**
 * @brief test the forward and backward functions
 * @param char* name, the input image file name
 */
void
test_forward_backward(char* name)
{
  fprintf(stderr, "test_forward_backward: ");
  (void)name;
  fprintf(stderr, "OK\n");
}

/**
 * @brief test image reconstruction from of magnitude and phase spectrum
 * @param char *name: the input image file name
 */
void
test_reconstruction(char* name)
{
    fprintf(stderr, "test_reconstruction: ");

    pnm ims = pnm_load(name);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    int size = rows*cols;

    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    char * _name = basename(name);

    //on créé unsigned short *gray_image avec ims
    unsigned short* g_ims = malloc(size*sizeof(unsigned short));
    for (unsigned short i = 0; i < rows; i++) {
        for (unsigned short j = 0; j < cols; j++) {
            *(g_ims + (i+j)*sizeof(unsigned short)) = 
            (unsigned short) pnm_get_component(ims, i, j, 0);
        }
    }

    //on créé freq_repr_in ce qui nous met dans l'espace des fréquences
    fftw_complex* freq_repr_in = malloc(size*sizeof(fftw_complex));
    freq_repr_in = forward(rows, cols, g_ims);
    
    float* as_out = malloc(size*sizeof(float));
    float* ps_out = malloc(size*sizeof(float));

    //on passe de l'espace des fréquences à celui des phases/amplitudes
    freq2spectra(rows, cols, freq_repr_in, as_out, ps_out);

    //on revient edans l'espace des frequences
    fftw_complex* freq_repr_out = malloc(size*sizeof(fftw_complex));

    spectra2freq(rows, cols, as_out, ps_out, freq_repr_out);

    //on revient à une matrice grise
    unsigned short* g_imd = backward(rows, cols, freq_repr_out);

    //on repasse dand pnm
    for (unsigned short i = 0; i < rows; i++) {
        for (unsigned short j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++) {
                pnm_set_component(imd, i, j, k,
                (unsigned short) *g_imd
                + (i+j)*sizeof(unsigned short));
            }
        }
    }

    //on test l'égalité terme à terme
    for (unsigned short i = 0; i < rows; i++) {
        for (unsigned short j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++) {
                if( (pnm_get_component(ims, i, j, k) 
                    -
                    pnm_get_component(ims, i, j, k)) < ERROR)
                    exit(EXIT_FAILURE);
            }
        }
    }    

    char fb_asps[100] = "FB-ASPS-";
    //printf("ninon %s : %zu efe\n",name,strlen(name));
    strcat(fb_asps,_name);
    pnm_save(imd, PnmRawPpm, fb_asps);

    //pnm_free(imd);
    pnm_free(ims);

    fprintf(stderr, "OK\n");
}

/**
 * @brief test construction of magnitude and phase images in ppm files
 * @param char* name, the input image file name
 */
void
test_display(char* name)
{
  fprintf(stderr, "test_display: ");
  (void)name;
  fprintf(stderr, "OK\n");
}

/**
 * @brief test the modification of magnitude by adding a periodic functions
          on both vertical and horizontal axis, and 
 *        construct output images
 * @param char* name, the input image file name
 */
void
test_add_frequencies(char* name)
{
  fprintf(stderr, "test_add_frequencies: ");
  (void)name;
  fprintf(stderr, "OK\n");
}

void
run(char* name)
{
  test_forward_backward(name);
  test_reconstruction(name);
  test_display(name);
  test_add_frequencies(name);
}

void 
usage(const char *s)
{
  fprintf(stderr, "Usage: %s <ims> \n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 1
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);
  run(argv[1]);
  return EXIT_SUCCESS;
}
