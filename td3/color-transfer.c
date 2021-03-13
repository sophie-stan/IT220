/**
 * @file color-transfert
 * @brief transfert color from source image to target image.
 *        Method from Reinhard et al. : 
 *        Erik Reinhard, Michael Ashikhmin, Bruce Gooch and Peter Shirley, 
 *        'Color Transfer between Images', IEEE CGA special issue on 
 *        Applied Perception, Vol 21, No 5, pp 34-41, September - October 2001
 *
 * Link for the article which inspired the following exercise:
 * https://vta.vvv.enseirb-matmeca.fr/IT220/20.21/RAGS-01.pdf
 */

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <bcl.h>

#define D 3

float RGB2LMS[D][D] = {
        {0.3811, 0.5783, 0.0402},
        {0.1967, 0.7244, 0.0782},
        {0.0241, 0.1288, 0.8444}
};

float LMS2RGB[D][D] = {
        {4.4679,  -3.5873, 0.1193},
        {-1.2186, 2.3809,  -0.1624},
        {0.0497,  -0.2439, 1.2045}
};

float RGB2LAB[D][D] = {
        {0.5774, 0.5774,  0.5774},
        {0.4082, 0.4082,  -0.8165},
        {0.7071, -0.7071, 0}
};

float LAB2RGB[D][D] = {
        {0.5774, 0.4082,  0.7071},
        {0.5774, 0.4082,  -0.7071},
        {0.5774, -0.8165, 0}
};

// Allocates memory for a 3D matrix of floats and dimensions rows * cols * 3
float*** malloc_img(int rows, int cols) {
    float*** data = (float***) malloc(rows * sizeof(float*));
    for (int i = 0; i < rows; i++) {
        data[i] = (float**) malloc(cols * sizeof(float*));
        for (int j = 0; j < cols; j++) {
            data[i][j] = (float*) malloc(3 * sizeof(float));
            for (int k = 0; k < 3; k++) {
                data[i][j][k] = 0;
            }
        }
    }
    return data;
}

// Frees the memory allocated, and pointed to by data
void free_img(int rows, int cols, float*** data) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            free(data[i][j]);
        }
        free(data[i]);
    }
    free(data);
}

// Fills data with the content of ims
void fill_img(int rows, int cols, float*** data, pnm ims) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++) {
                data[i][j][k] = pnm_get_component(ims, i, j, k);
            }
        }
    }
}

// Computes the matrix product: res = matrix * channels
void matrix_prod_channels(float matrix[3][3], float channels[3], float res[3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            res[i] += matrix[i][j] * channels[j];
        }
    }
}

// Rounds all values of data + truncates if it is out of bound
void uniform(int rows, int cols, float*** data) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++) {
                data[i][j][k] = round(data[i][j][k]);
                if (data[i][j][k] > 255) {
                    data[i][j][k] = 255;
                } else if (data[i][j][k] < 0) {
                    data[i][j][k] = 0;
                }
            }
        }
    }
}

void process(char* ims_name, char* imt_name, char* imd_name) {
    pnm ims = pnm_load(ims_name);
    int rows_ims = pnm_get_height(ims);
    int cols_ims = pnm_get_width(ims);
    pnm imd = pnm_new(rows_ims, cols_ims, PnmRawPpm);

    //float* ims_data = malloc(sizeof(float) * rows_ims * cols_ims * 3);
    //float (* ims_col)[rows_ims] = (float (*)[rows_ims]) ims_data;

    float*** data_ims = malloc_img(rows_ims, cols_ims);
    fill_img(rows_ims, cols_ims, data_ims, ims);


    // RGB to LMS
    float*** data_ims_lms = malloc_img(rows_ims, cols_ims);
    for (int i = 0; i < rows_ims; i++) {
        for (int j = 0; j < cols_ims; j++) {
            matrix_prod_channels(RGB2LMS, data_ims[i][j], data_ims_lms[i][j]);
        }
    }

    // LMS goes back to RGB
    float*** data_ims_rgb = malloc_img(rows_ims, cols_ims);
    for (int i = 0; i < rows_ims; i++) {
        for (int j = 0; j < cols_ims; j++) {
            matrix_prod_channels(LMS2RGB, data_ims_lms[i][j],
                                 data_ims_rgb[i][j]);
        }
    }

    // To compare the initial image with the one which travelled
    for (int i = 0; i < rows_ims; i++) {
        for (int j = 0; j < cols_ims; j++) {
            for (int k = 0; k < 3; k++) {
                pnm_set_component(imd, i, j, k, data_ims_rgb[i][j][k]);
            }
        }
    }


    pnm_save(imd, PnmRawPpm, imd_name);

    free_img(rows_ims, cols_ims, data_ims_rgb);
    free_img(rows_ims, cols_ims, data_ims_lms);
    free_img(rows_ims, cols_ims, data_ims);
    pnm_free(ims);
}

void usage(char* s) {
    fprintf(stderr, "Usage: %s <ims> <imt> <imd> \n", s);
    exit(EXIT_FAILURE);
}

#define PARAM 3

int main(int argc, char* argv[]) {
    if (argc != PARAM + 1) usage(argv[0]);
    process(argv[1], argv[2], argv[3]);
    return EXIT_SUCCESS;
}
