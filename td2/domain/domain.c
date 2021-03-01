#include <math.h>
#include <domain.h>
#include <assert.h>

unsigned short bilinear_interpolation(float x, float y, pnm ims, int c) {
    assert(c >= 0 && c <= 2);

    float j = floor(x);
    float i = floor(y);
    float dx = x - j;
    float dy = y - i;

    return (1 - dx) * (1 - dy) * pnm_get_component(ims, i, j, c)
    + dx * (1 - dy) * pnm_get_component(ims, i, j + 1, c)
    + dx * dy * pnm_get_component(ims, i + 1, j + 1, c);
}