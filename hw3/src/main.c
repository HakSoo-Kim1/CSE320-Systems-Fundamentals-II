#include <stdio.h>
#include "sfmm.h"
#include "debug.h"

int main(int argc, char const *argv[]) {

        sf_set_magic(0);
size_t sz_x = sizeof(double) * 8, sz_y = sizeof(int);
    debug("%ld %ld",sizeof(double),sizeof(int));
    void *x = sf_malloc(sz_x);
    sf_show_heap();
    sf_realloc(x, sz_y);

    sf_show_heap();

        // sf_free(y);

    return EXIT_SUCCESS;
}
