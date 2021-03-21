/**
 * Link for the article which inspired the following exercise:
 * https://vta.vvv.enseirb-matmeca.fr/IT220/20.21/WAM-02.pdf
 */

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <bcl.h>

#define D 3
#define NB_SAMPLES 200 // Must be an even number
#define NEIGHBORHOOD_SIZE 5 // Must be an odd number

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

// Computes the matrix product: output = matrix * input
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
                            input[i][j][k] = log10f(input[i][j][k]);
                        }
                    }
                    matrix_prod_channels(LMS2LAB, input[i][j], output[i][j]);
                    break;
                case 2: // LAB to LMS
                    matrix_prod_channels(LAB2LMS, input[i][j], output[i][j]);
                    for (int k = 0; k < 3; k++) {
                        output[i][j][k] = powf(10, output[i][j][k]);
                    }
                    break;
                case 3: // LMS to RGB
                    matrix_prod_channels(LMS2RGB, input[i][j], output[i][j]);
            }
        }
    }
    return output;
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

// Computes the mean of the luminance/first channel of data in the mean float
float compute_luminance_mean(int rows, int cols, float*** data) {
    float mean = 0;
    int total_points = rows * cols;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mean += data[i][j][0];
        }
    }
    mean /= total_points;
    return mean;
}

// Computes the deviations of the luminance/first channel of data, and stores it in the deviations tab
float
compute_luminance_deviation(int rows, int cols, float*** data, float mean) {
    float deviation = 0;
    int total_points = rows * cols;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            deviation += powf(data[i][j][0] - mean, 2);
        }
    }
    deviation = sqrtf(deviation / total_points);
    return deviation;
}

// Luminance remapping of data
void remap_luminance(int rows, int cols, float*** data,
                     float luminance_deviation_a, float luminance_deviation_b,
                     float luminance_mean_a, float luminance_mean_b) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            data[i][j][0] = (luminance_deviation_b / luminance_deviation_a) *
                            (data[i][j][0] - luminance_mean_a) +
                            luminance_mean_b;
        }
    }
}

// Computes the deviations of the area around the pixel (p,q) from data
float compute_area_deviation(int rows, int cols, float*** data, int p, int q,
                             float mean) {
    int radius = NEIGHBORHOOD_SIZE / 2;
    int total_points = 0;
    float value = 0;
    for (int i = -radius; i < radius + 1; i++) {
        for (int j = -radius; j < radius + 1; j++) {
            if ((0 <= (p + i) && (p + i) < rows) &&
                (0 <= (q + j) && (q + j) < cols)) {
                value += powf(data[p + i][q + j][0] - mean, 2);
                total_points += 1;
            }
        }
    }
    value = sqrtf(value / total_points);
    return value;
}

/*
// Computes the mean of the area around data[p][q]
float compute_area_mean(int rows, int cols, float*** data, int p, int q) {
    int total_points = 0;
    float value = 0;
    int radius = NEIGHBORHOOD_SIZE / 2;
    for (int i = -radius; i < radius + 1; i++) {
        for (int j = -radius; j < radius + 1; j++) {
            if ((0 <= (p + i) && (p + i) < rows) &&
                (0 <= (q + j) && (q + j) < cols)){
                //value += data[p + i][q + j][0]
                //total_points += 1;
                value += pow(data[p + i][q + j][0] - mean, 2);
            }
        }
    }
    value = sqrtf(value / total_points);
    return value;
}*/


// Fills candidates, a vector of exactly NB_SAMPLES pixels from data
void compute_candidates(int rows, int cols, int candidates[NB_SAMPLES][2]) {
    /** Appropriate grid **/
    /* Finds the two integers R and C, so that R * C = NB_SAMPLES
     * and so that the ratio R/C is the closest possible to rows/cols.
     */
    int ratio = rows > cols ? rows / cols : cols / rows; // ensures ratio > 1
    int min, max, min_prev, max_prev, R, C;
    min = 2;
    max = NB_SAMPLES / 2;
    /* (1) While our current ratio (max / min) is higher than the image's ratio
     * (2) We keep digging for another couple of integers (min, max) such as
     * min * max = NB_SAMPLES
     * (3) When max / min < ratio, we verify that the previous couple (min, max)
     * is not CLOSER to the image's ratio than the last one we found.
     * (4) Which side is longer than the other one ?
     */
    // (1)
    while (max / min > ratio) {
        min_prev = min;
        max_prev = max;
        // (2)
        do {
            max--;
        } while (NB_SAMPLES % max != 0);
        min = NB_SAMPLES / max;
    }
    // (3)
    if (abs(ratio - max / min) > abs(ratio - max_prev / min_prev)) {
        min = min_prev;
        max = max_prev;
    }
    // (4)
    R = rows > cols ? max : min;
    C = rows > cols ? min : max;

    /** Candidates filling **/
    // A grid square has a size of row/R * cols/C
    int h = rows / R;
    int L = cols / C;
    int gap = NEIGHBORHOOD_SIZE / 2;
    int cpt = 0; // current index in candidates
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            int i1, j1;
            do {
                i1 = rand() % h;
                j1 = rand() % L;
            } while (i * h + i1 > rows - gap || i * h + i1 < gap ||
                     j * L + j1 > cols - gap || j * L + j1 < gap);
            candidates[cpt][0] = i * h + i1;
            candidates[cpt][1] = j * L + j1;
            cpt++;
        }
    }
}

// Returns the index of the first minimum occurrence in tab
int min_tab(float* tab, int len) {
    int i = 0;
    for (int k = 0; k < len; k++) {
        i = (tab[k] < tab[i]) ? k : i;
    }
    return i;
}

// Colorizes imt with ims. rows and cols belongs to imt.
void colorization(int rows, int cols, float*** imt, float*** ims,
                  int candidates_ims[NB_SAMPLES][2],
                  float deviations_ims[NB_SAMPLES],
                  float luminance_mean_ims) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float distances[NB_SAMPLES];
            //float mean = compute_area_mean(rows, cols, imt, i, j);
            float deviation_imt =
                    compute_area_deviation(rows, cols, imt, i, j,
                                           luminance_mean_ims);


            for (int k = 0; k < NB_SAMPLES; k++) {
                distances[k] = 0.5 * (fabs(imt[i][j][0]
                                           -
                                           ims[candidates_ims[k][0]][candidates_ims[k][1]][0])
                                      + fabs(deviation_imt
                                             - deviations_ims[k]));
            }
            int ind = min_tab(distances, NB_SAMPLES);
            int* coord = candidates_ims[ind];
            float* point = ims[coord[0]][coord[1]];
            imt[i][j][1] = point[1];
            imt[i][j][2] = point[2];
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
    pnm_save(imd, PnmRawPpm, imd_name);

    /********** LUMINANCE REMAPPING **********/
    // Computes luminance means
    float luminance_mean_ims =
            compute_luminance_mean(rows_ims, cols_ims, data_ims_lab);
    float luminance_mean_imt =
            compute_luminance_mean(rows_imt, cols_imt, data_imt_lab);

    // Computes luminance deviations
    float luminance_deviation_ims =
            compute_luminance_deviation(rows_ims, cols_ims, data_ims_lab,
                                        luminance_mean_ims);
    float luminance_deviation_imt =
            compute_luminance_deviation(rows_imt, cols_imt, data_imt_lab,
                                        luminance_mean_imt);

    if (luminance_deviation_imt == 0) luminance_deviation_imt = 0.0001;

    // Remaps luminance
    remap_luminance(rows_imt, cols_imt, data_imt_lab,
                    luminance_deviation_imt, luminance_deviation_ims,
                    luminance_mean_imt, luminance_mean_ims);

    /********** JITTERED GRID **********/
    /* This step allows to reduce the number of comparisons made for each pixel
     * in the grey-scale image.
     */
    int candidates[NB_SAMPLES][2];
    compute_candidates(rows_ims, cols_ims, candidates);

    /********** BEST CANDIDATE SELECTION **********/
    /* For each pixel candidate in the greyscale image, the  best  matching
    * color sample is selected.
    */
    // Computes the deviation area for each candidate pixel of ims. (grey img)
    float candidates_deviations[NB_SAMPLES] = {};
    for (int k = 0; k < NB_SAMPLES; k++) {
        candidates_deviations[k] =
                compute_area_deviation(rows_ims, cols_ims, data_ims_lab,
                                       candidates[k][0], candidates[k][1],
                                       luminance_mean_ims);
    }
    colorization(rows_imt, cols_imt, data_imt_lab, data_ims_lab,
                 candidates, candidates_deviations, luminance_mean_ims);

    /********** BACK **********/
    float*** lms = switch_space(rows_imt, cols_imt, data_imt_lab, 2); // (DA 2)
    //free_3D_matrix(rows_imt, cols_imt, res); // Memory free (1.3)

    float*** rgb = switch_space(rows_imt, cols_imt, lms, 3); // (DA 3)
    free_3D_matrix(rows_imt, cols_imt, lms); // Memory free (2)

    normalize(rows_imt, cols_imt, rgb);
    for (int i = 0; i < rows_imt; i++) {
        for (int j = 0; j < cols_imt; j++) {
            for (int k = 0; k < 3; k++) {
                pnm_set_component(imd, i, j, k, rgb[i][j][k]);
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
    int nb_samples = NB_SAMPLES;
    if (nb_samples % 2 != 0) {
        printf("The constant NB_SAMPLES defined at the beginning of the"
               " prog is not valid\n");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
