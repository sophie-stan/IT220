#include <stdlib.h>
#include <stdio.h>
#include <pnm.h>


void
one_process(char current_dir, char* ims_name, char* imd_name)
{

    pnm ims = pnm_load(ims_name);

    int imd_rows, imd_cols;
    
    int ims_rows = pnm_get_height(ims);
    int ims_cols = pnm_get_width(ims);

    if(current_dir == 'h' || current_dir == 'v' ){
        imd_rows = ims_rows;
        imd_cols = ims_cols;
    }
    else if(current_dir == 't'){
        imd_rows = ims_cols;
        imd_cols = ims_rows;
    }
    else{
        return;
    }

    pnm imd = pnm_new(imd_cols, imd_rows, PnmRawPpm);

    for (int p = 0; p < ims_rows; p++) {
        for (int q = 0; q < ims_cols; q++) {
            for (int k = 0; k < 3; k++) {
                float val;
                if(current_dir == 'h') {
                    val = pnm_get_component(ims, p, ims_cols-1-q, k);
                    pnm_set_component(imd, p, q, k, val);
                }
                if(current_dir == 'v') {
                    val = pnm_get_component(ims, ims_rows-1-p, q, k);
                    pnm_set_component(imd, p, q, k, val);
                }
                if(current_dir == 't') {
                    val = pnm_get_component(ims, p, q, k);
                    pnm_set_component(imd, q, p, k, val);
                }

            }
        }
    }

    pnm_save(imd, PnmRawPpm, imd_name);

    pnm_free(ims);
    pnm_free(imd);
}

void 
process(char* dir, char* ims_name, char* imd_name)
{
    int i = 0;
    char * tmp_ims_name = ims_name;
    while(dir[i] != '\0'){
        one_process(dir[i], tmp_ims_name, imd_name);
        tmp_ims_name = imd_name;
        i++;
    }
}

void
usage(char* s)
{
  fprintf(stderr,"%s <dir> <ims> <imd>\n",s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int
main(int argc, char* argv[])
{
    if(argc != PARAM+1)
        usage(argv[0]);
    char* _dir = argv[1];
    char* ims_name = argv[2];
    char* imd_name = argv[3];
    process(_dir, ims_name, imd_name);
  return EXIT_SUCCESS;
}
