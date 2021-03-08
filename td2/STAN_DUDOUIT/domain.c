#include <math.h>
#include <domain.h>

unsigned short bilinear_interpolation(float x, float y, pnm ims, int c) {
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);

    float j = floor(x);
    float i = floor(y);
    if (j < 0 || i < 0 || j + 1 >= cols || i + 1 >= rows) {
        return 0;
    }
    float dx = x - j;
    float dy = y - i;

    return (1 - dx) * (1 - dy) * pnm_get_component(ims, i, j, c)
    + dx * (1 - dy) * pnm_get_component(ims, i, j + 1, c)
    + (1 - dx) * dy * pnm_get_component(ims, i + 1, j , c)
    + dx * dy * pnm_get_component(ims, i + 1, j + 1, c);
}

