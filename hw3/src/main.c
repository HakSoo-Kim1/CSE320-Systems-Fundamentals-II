#include <stdio.h>
#include "sfmm.h"
#include "debug.h"

int main(int argc, char const *argv[]) {
        // sf_set_magic(0);
    size_t sz = sizeof(double) * 30;
    debug("%ld",sz);
    // sf_free(NULL);
    void *x = sf_malloc(sz);
    void *y = sf_malloc(sz);
    void *z = sf_malloc(sz);
    void *q = sf_malloc(sz);
    sf_free(sf_mem_start() + 40);
    // sf_show_heap();
    // sf_internal_fragmentation();
    // sf_free(x);         // case 1
    // // sf_show_heap();
    // sf_free(y);         // case 3
    // // sf_show_heap();

    // sf_free(q);         // case 2
    // // sf_show_heap();

    // sf_free(z);         // case 4
    // // sf_show_heap();
    if(x && y && z&& q){

    }
        debug("%p",sf_mem_start());

    sf_show_heap();
    debug("final %f",sf_internal_fragmentation());

    return EXIT_SUCCESS;
}
