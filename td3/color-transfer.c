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

float LMS2LAB[D][D] = {
        {0.5774, 0.5774,  0.5774},
        {0.4082, 0.4082,  -0.8165},
        {0.7071, -0.7071, 0.0}
};

float LAB2LMS[D][D] = {
        {0.5774, 0.4082,  0.7071},
        {0.5774, 0.4082,  -0.7071},
        {0.5774, -0.8165, 0.0}
};

// Allocates memory for a 3D matrix of floats and dimensions rows * cols * 3
float*** malloc_3D_matrix(int rows, int cols) {
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
void free_3D_matrix(int rows, int cols, float*** data) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            free(data[i][j]);
        }
        free(data[i]);
    }
    free(data);
}

// Fills data with the content of ims
void fill_3D_matrix(int rows, int cols, float*** data, pnm img) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++) {
                data[i][j][k] = pnm_get_component(img, i, j, k);
            }
        }
    }
}

// Computes the matrix product: res = matrix * channels
void matrix_prod_channels(float matrix[3][3], float input[3], float output[3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            output[i] += matrix[i][j] * input[j];
        }
    }
}

// Switches input from space according to t
float*** switch_space(int rows, int cols, float*** input, int t) {
    float*** output = malloc_3D_matrix(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            switch (t) {
                case 0: // RGB to LMS
                    matrix_prod_channels(RGB2LMS, input[i][j], output[i][j]);
                    break;
                case 1: // LMS to LAB
                    for (int k = 0; k < 3; k++) {
                        if (input[i][j][k] <= 0) {
                            input[i][j][k] = 0.0001;
                        } else {
                            input[i][j][k] = log10(input[i][j][k]);
                        }
                    }
                    matrix_prod_channels(LMS2LAB, input[i][j], output[i][j]);
                    break;
                case 2: // LAB to LMS
                    matrix_prod_channels(LAB2LMS, input[i][j], output[i][j]);
                    for (int k = 0; k < 3; k++) {
                        output[i][j][k] = pow(10, output[i][j][k]);
                    }
                    break;
                case 3: // LMS to RGB
                    matrix_prod_channels(LMS2RGB, input[i][j], output[i][j]);
            }
        }
    }
    return output;
}

// Computes the mean of the 3 channels of data, and stores it in the means tab
void compute_means(int rows, int cols, float*** data, float* means) {
    int total_points = rows * cols;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++) {
                means[k] += data[i][j][k];
            }
        }
    }
    for (int k = 0; k < 3; k++) {
        means[k] /= total_points;
    }
}

// Computes the deviations of the 3 channels of data, and stores it in the deviations tab
void compute_deviations(int rows, int cols, float*** data, float* means,
                        float* deviations) {
    int total_points = rows * cols;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++) {
                deviations[k] += pow(data[i][j][k] - means[k], 2);
            }
        }
    }
    for (int k = 0; k < 3; k++) {
        deviations[k] = sqrt(deviations[k] / total_points);
    }
}

// Computes lab
void compute_lab(int rows, int cols, float*** data, float* means) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < 3; k++) {
                data[i][j][k] = data[i][j][k] - means[k];
            }
        }
    }
}

// Rounds all values of data + truncates if it is out of bound
void normalize(int rows, int cols, float*** data) {
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

// Translates img into lab space
float*** phase1(pnm img, int rows, int cols) {
    float*** rgb = malloc_3D_matrix(rows, cols);
    fill_3D_matrix(rows, cols, rgb, img);

    float*** lms = switch_space(rows, cols, rgb, 0);
    free_3D_matrix(rows, cols, rgb);

    float*** lab = switch_space(rows, cols, lms, 1);
    free_3D_matrix(rows, cols, lms);

    return lab;
}


void process(char* ims_name, char* imt_name, char* imd_name) {
    /********** INITIALIZATION **********/
    pnm ims = pnm_load(ims_name);
    pnm imt = pnm_load(imt_name);
    int rows_ims = pnm_get_height(ims);
    int cols_ims = pnm_get_width(ims);
    int rows_imt = pnm_get_height(imt);
    int cols_imt = pnm_get_width(imt);

    pnm imd = pnm_new(cols_imt, rows_imt, PnmRawPpm); // On applique ims sur imt

    /********** GO **********/
    float*** data_ims_lab = phase1(ims, rows_ims, cols_ims); // DA (1.1)
    float*** data_imt_lab = phase1(imt, rows_imt, cols_imt); // DA (1.2)

    /********** STATISTICS **********/
    // Computes means
    float means_ims[3] = {};
    float means_imt[3] = {};
    compute_means(rows_ims, cols_ims, data_ims_lab, means_ims);
    compute_means(rows_imt, cols_imt, data_imt_lab, means_imt);


    // Computes deviations
    float deviations_ims[3] = {};
    float deviations_imt[3] = {};
    compute_deviations(rows_ims, cols_ims, data_ims_lab, means_ims,
                       deviations_ims);
    compute_deviations(rows_imt, cols_imt, data_imt_lab, means_imt,
                       deviations_imt);

    // Computes lab* //after  Computes deviations
    compute_lab(rows_imt, cols_imt, data_imt_lab, means_imt);
    compute_lab(rows_ims, cols_ims, data_ims_lab, means_ims);


    float*** res = malloc_3D_matrix(rows_imt, cols_imt); // DA (1.3)

    // Computes lab'
    for (int i = 0; i < rows_imt; i++) {
        for (int j = 0; j < cols_imt; j++) {
            for (int k = 0; k < 3; k++) {
                res[i][j][k] = (deviations_ims[k] / deviations_imt[k]) * data_imt_lab[i][j][k] + means_ims[k];
            }
        }
    }

    /********** BACK **********/
    float*** lms = switch_space(rows_imt, cols_imt, res,2); // (DA 2)
    free_3D_matrix(rows_imt, cols_imt, res); // Memory free (1.3)

    float*** rgb = switch_space(rows_imt, cols_imt, lms,3); // (DA 3)
    free_3D_matrix(rows_imt, cols_imt, lms); // Memory free (2)



/*
    float max_imt[3] = {};
    float min_imt[3] = {};
    float interval_imt[3] = {};

    for (int k = 0; k < 3; k++) {
        max_imt[k] = rgb[0][0][k];
        min_imt[k] = rgb[0][0][k];
    }


    for (int i = 0; i < rows_imt; i++) {
        for (int j = 0; j < cols_imt; j++) {
            for (int k = 0; k < 3; k++) {
                if(max_imt[k] < rgb[i][j][k]) max_imt[k] = rgb[i][j][k];
                if(min_imt[k] > rgb[i][j][k]) min_imt[k] = rgb[i][j][k];
            }
        }
    }

    for (int k = 0; k < 3; k++) {
        interval_imt[k] = (max_imt[k] - min_imt[k])/2.;
    }

    for (int i = 0; i < rows_imt; i++) {
        for (int j = 0; j < cols_imt; j++) {
            for (int k = 0; k < 3; k++) {
                rgb[i][j][k] = abs(rgb[i][j][k] - interval_imt[k])/(2.*interval_imt[k]);
            }
        }
    }
*/

    normalize(rows_imt, cols_imt, rgb);

    for (int i = 0; i < rows_imt; i++) {
        for (int j = 0; j < cols_imt; j++) {
            for (int k = 0; k < 3; k++) {
                pnm_set_component(imd, i, j , k, rgb[i][j][k]);
            }
        }
    }
    pnm_save(imd, PnmRawPpm, imd_name);

    /********** MEMORY FREE **********/
    pnm_free(imd);
    free_3D_matrix(rows_ims, cols_ims, data_ims_lab); // Memory free (1.1)
    free_3D_matrix(rows_imt, cols_imt, data_imt_lab); // Memory free (1.2)
    free_3D_matrix(rows_imt, cols_imt, rgb); // Memory free (3)
    pnm_free(ims);
    pnm_free(imt);
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
