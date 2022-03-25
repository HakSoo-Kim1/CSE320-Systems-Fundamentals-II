#include <stdio.h>
#include "sfmm.h"
#include "debug.h"

int main(int argc, char const *argv[]) {

    // void * a = sf_malloc(2);
    //     // void * b = sf_malloc(2000);
    //     // if(b);
    // sf_show_heap();
    // sf_peak_utilization();
    // sf_internal_fragmentation();
    // a = sf_realloc(a,3000);
    //     sf_show_heap();

    //     sf_peak_utilization();
    //         sf_internal_fragmentation();


    // sf_free(a);
    //     sf_show_heap();

    // sf_peak_utilization();
    //     sf_internal_fragmentation();

    void * x = sf_malloc(32);
    sf_show_heap();
    sf_free(x);
sf_show_heap();
    debug("hello");
    sf_show_heap();

    debug("bye");
    x = sf_malloc(32);
    sf_malloc(32);
    sf_free(x);
    sf_peak_utilization();

    return EXIT_SUCCESS;
}
