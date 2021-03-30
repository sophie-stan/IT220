/**
 * @file copy.c
 * @brief resize a given image with an increasing factor by copying existing
 *        pixels. The resize factor is considered as an interger.
 * @author Vinh-Thong Ta <vinh-thong.ta@labri.fr>
 */

#include <stdio.h>
#include <stdlib.h>

#include <pnm.h>

void process(char* factor, char* ims_name, char* imd_name) {
    // Loading inputs    
    pnm ims = pnm_load(ims_name);
    int rows_ims = pnm_get_height(ims);
    int cols_ims = pnm_get_width(ims);

    int factor_int = atoi(factor);
    int rows_imd = factor_int * rows_ims;
    int cols_imd = factor_int * cols_ims;

    pnm imd = pnm_new(cols_imd, rows_imd, PnmRawPpm);


    // Computing the zoom result
    for (int i = 0; i < rows_ims; i++) {
        for (int j = 0; j < cols_ims; j++) {
            for (int k = 0; k < 3; k++) {
                unsigned short value = pnm_get_component(ims, i, j, k);
                for (int i_ims = 0; i_ims < factor_int; i_ims++) {
                    for (int j_ims = 0; j_ims < factor_int; j_ims++) {
                        pnm_set_component(imd, i * factor_int + i_ims, j * factor_int + j_ims, k, value);
                    }
                }
            }
        }
    }
    // Saving results
    pnm_save(imd, PnmRawPpm, imd_name);
    
    // Memory free
    pnm_free(ims);
    pnm_free(imd);
}


void usage (char *s){
    fprintf(stderr, "Usage: %s <factor> <ims> <imd> \n", s);
    exit(EXIT_FAILURE);
}

#define param 3
int main(int argc, char *argv[]){
    if (argc != param+1) usage(argv[0]);
    if (atoi(argv[1]) <= 1) {
        printf("Error : <factor> should be > 1\n");
        exit(EXIT_FAILURE);
    }
    process(argv[1], argv[2], argv[3]);

    return EXIT_SUCCESS;
}
