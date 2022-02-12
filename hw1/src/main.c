#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "argo.h"
#include "global.h"
#include "debug.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

int main(int argc, char **argv)
{   
    int isValid = validargs(argc, argv);
    debug("The result is %d",isValid);
    debug("global options is %X",global_options);
    if (isValid == -1){
        fprintf(stderr, "Wrong argument form\n");                   /// how can I see this? 
        USAGE(*argv, EXIT_FAILURE);                                 
        return EXIT_FAILURE;
    }
    
    if(global_options == HELP_OPTION){
        USAGE(*argv, EXIT_SUCCESS);
        return EXIT_SUCCESS;
    }
    ARGO_VALUE * argoVal =  argo_read_value(stdin);
    if (argoVal -> content.number.valid_float == 1 && argoVal -> content.number.valid_int == 0){
        debug("%f", argoVal -> content.number.float_value);
    }
    else{
        debug("%ld", argoVal -> content.number.int_value);
        debug("%f", argoVal -> content.number.float_value);

    }
    argo_write_value(argoVal, stdout);

    return EXIT_SUCCESS;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
