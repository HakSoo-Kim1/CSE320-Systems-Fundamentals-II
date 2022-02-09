#include <stdlib.h>

#include "argo.h"
#include "global.h"
#include "debug.h"

int strLen(char *str);
int stringToInt(char *str);

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the various options that were specified will be
 * encoded in the global variable 'global_options', where it will be
 * accessible elsewhere in the program.  For details of the required
 * encoding, see the assignment handout.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * @modifies global variable "global_options" to contain an encoded representation
 * of the selected program options.
 */

int validargs(int argc, char **argv) {
    if (argc == 1){     // no flag is given
        return -1;
    }

    if ( *(*(argv + 1)) == 45 && *((*(argv + 1)) + 1) == 104 && strLen(*(argv + 1)) == 2){      // arg1 is -h
        global_options = HELP_OPTION;
        return 0;
    }

    if (*(*(argv + 1)) == 45 && strLen(*(argv + 1)) == 2){                                          // arg1 is -X
        if(*((*(argv + 1)) + 1) == 99 ){                                                       // arg1 is -c   
            if (argc == 2){
                global_options = global_options | CANONICALIZE_OPTION;
                return 0;
            }
            else if (argc == 3){    
                if (*(*(argv + 2)) == 45 && *((*(argv + 2)) + 1) == 112 && strLen(*(argv + 2)) == 2){                                          // arg2 is -p
                global_options = global_options | CANONICALIZE_OPTION;
                global_options = global_options | PRETTY_PRINT_OPTION;
                global_options = global_options | 4;
                return 0;
                }
            }
            else if (argc == 4){
                if (*(*(argv + 2)) == 45 && *((*(argv + 2)) + 1) == 112 && strLen(*(argv + 2)) == 2){                                          // arg2 is -p
                    int integer = stringToInt(*(argv + 3));
                    if (integer == -1){
                        return -1;
                    }
                    global_options = global_options | CANONICALIZE_OPTION;
                    global_options = global_options | PRETTY_PRINT_OPTION;
                    global_options = global_options | integer;
                    return 0;
                }    
            }
        }
        else if (*((*(argv + 1)) + 1) == 118){                                                 // arg1 is -v
            if (argc == 2){
                global_options = global_options | VALIDATE_OPTION;
                return 0;
            }
        }
    }
    return -1;
}

/// compute string length
int strLen(char *str){
    int counter = 0;
    while(*(str + counter) != '\0'){
      counter++;
    }
   return counter;
}

/// convert string to int
/// if coversion is invalid return -1, otherwise return correspond int
int stringToInt(char *str){
    int integer = 0;
    while ((* str) != '\0'){
        if (!(48 <= (* str) && (* str) <= 57)){
            return -1;
        }
        integer *= 10;
        integer += (* str) - 48;
        str = str + 1;
    }
    if (integer >= 256){
        return -1;
    }
    return integer;
}
