#include <stdio.h>
#include "sfmm.h"
#include "debug.h"

int main(int argc, char const *argv[]) {
    // sf_set_magic(0);
    debug("MAGIC is %ld",MAGIC);
    size_t sz = sizeof(int);
    sf_malloc(sz);
        sf_show_heap();

        // sf_free(y);

    return EXIT_SUCCESS;
}
