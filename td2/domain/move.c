#include <stdlib.h>
#include <stdio.h>
#include <pnm.h>

void 
process(int dx, int dy, char* ims_name, char* imd_name)
{
  (void)dx;
  (void)dy;
  (void)ims_name;
  (void)imd_name;    
}

void
usage(char* s)
{
  fprintf(stderr,"%s <dx> <dy> <ims> <imd>\n",s);
  exit(EXIT_FAILURE);
}

#define PARAM 4
int
main(int argc, char* argv[])
{
  if(argc != PARAM+1)
    usage(argv[0]);

  process(0, 0, NULL, NULL);

  return EXIT_SUCCESS;
}
