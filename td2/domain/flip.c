#include <stdlib.h>
#include <stdio.h>
#include <pnm.h>


void
one_process(char current_dir, pnm* ims, pnm* imd, int rows, int cols)
{
    for (int p = 0; p < rows; p++) {
        for (int q = 0; q < cols; q++) {
            for (int k = 0; k < 3; k++) {
                float val;
                if(current_dir == 'h') {
                    val = pnm_get_component(*ims, p, cols-1-q, k);
                    pnm_set_component(*imd, p, q, k, val);

                }
                if(current_dir == 'v') {
                    val = pnm_get_component(*ims, rows-1-p, q, k);
                    pnm_set_component(*imd, p, q, k, val);
                }
                if(current_dir == 't') {
                    val = pnm_get_component(*ims, p, q, k);
                    pnm_set_component(*imd, q, p, k, val);
                }

            }
        }
    }
}

void 
process(char* dir, char* ims_name, char* imd_name)
{
    pnm ims = pnm_load(ims_name);
    pnm imd = pn;

    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);

    int i = 0;
    while(dir[i] != '\0'){
        if(dir[i] == 'h' || dir[i] == 'v' ){
            pnm imd_tmp = pnm_new(cols, rows, PnmRawPpm);
            one_process(dir[i], &ims, &imd_tmp, rows, cols);
            imd = pnm_dup(imd_tmp);
            free(imd_tmp);
        }
        else if(dir[i] == 't'){
            pnm imd_tmp = pnm_new(rows, cols, PnmRawPpm);
            one_process(dir[i], &ims, &imd_tmp, rows, cols);
            imd = pnm_dup(imd_tmp);        
            free(imd_tmp);
        }
        else{
            break;
        }
        i++;
    }

    pnm_save(imd, PnmRawPpm, imd_name);

    pnm_free(ims);
    pnm_free(imd);


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
