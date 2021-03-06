#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "debug.h"

/*
 * This is the "data store" module for Mush.
 * It maintains a mapping from variable names to values.
 * The values of variables are stored as strings.
 * However, the module provides functions for setting and retrieving
 * the value of a variable as an integer.  Setting a variable to
 * an integer value causes the value of the variable to be set to
 * a string representation of that integer.  Retrieving the value of
 * a variable as an integer is possible if the current value of the
 * variable is the string representation of an integer.
 */

typedef struct var_link {
    char *var;       
    char *val;            
    struct var_link *prev;
    struct var_link *next;
} VAR_LINK;

VAR_LINK varHead = {NULL, NULL, &varHead, &varHead};

/**
 * @brief  Get the current value of a variable as a string.
 * @details  This function retrieves the current value of a variable
 * as a string.  If the variable has no value, then NULL is returned.
 * Any string returned remains "owned" by the data store module;
 * the caller should not attempt to free the string or to use it
 * after any subsequent call that would modify the value of the variable
 * whose value was retrieved.  If the caller needs to use the string for
 * an indefinite period, a copy should be made immediately.
 *
 * @param  var  The variable whose value is to be retrieved.
 * @return  A string that is the current value of the variable, if any,
 * otherwise NULL.
 */
char *store_get_string(char *var) {
    debug("store_get_string is called");
    VAR_LINK *head = &varHead;
    VAR_LINK *node = varHead.next;

    while(node != head){
        if (strcmp(node->var, var) == 0){
            return node -> val;
        }
        node = node -> next;
    }
    debug("\t\t\tERROR");
    return NULL;
}

/**
 * @brief  Get the current value of a variable as an integer.
 * @details  This retrieves the current value of a variable and
 * attempts to interpret it as an integer.  If this is possible,
 * then the integer value is stored at the pointer provided by
 * the caller.
 *
 * @param  var  The variable whose value is to be retrieved.
 * @param  valp  Pointer at which the returned value is to be stored.
 * @return  If the specified variable has no value or the value
 * cannot be interpreted as an integer, then -1 is returned,
 * otherwise 0 is returned.
 */
int store_get_int(char *var, long *valp) {
        debug("store_get_int is called");

    char *valString = store_get_string(var);
    if (!valString){
        return -1;
    }
    char *ptr;
    debug("valString is %s",valString);
    *valp = strtol(valString, &ptr, 10);
    debug("converted val in long form : %ld", *valp);
    debug("rest of string after converted in store get int is : %s", ptr);

    if (valString && !*ptr){
        return 0;
    }
    else{
        debug("\t\t\t\t\tERROR!!!");
        return -1;
    }
}

/**
 * @brief  Set the value of a variable as a string.
 * @details  This function sets the current value of a specified
 * variable to be a specified string.  If the variable already
 * has a value, then that value is replaced.  If the specified
 * value is NULL, then any existing value of the variable is removed
 * and the variable becomes un-set.  Ownership of the variable and
 * the value strings is not transferred to the data store module as
 * a result of this call; the data store module makes such copies of
 * these strings as it may require. 
 *
 * @param  var  The variable whose value is to be set.
 * @param  val  The value to set, or NULL if the variable is to become
 * un-set.
 */
int store_set_string(char *var, char *val) {
    debug("\tstoring var : %s = %s in store_set_string",var,val);

    VAR_LINK *head = &varHead;
    VAR_LINK *node = varHead.next;

    while(node != head){
        if (strcmp(node->var, var) == 0){
            if (val == NULL){
                node -> prev -> next = node -> next;
                node -> next -> prev = node -> prev;
                free(node -> var);
                free(node -> val);
                free(node);
            }
            else{
                strcpy(node -> val, val);

                // node -> val = val;
            }
            return 0;
        }
        node = node -> next;
    }

    // there is no matched variable in map

    VAR_LINK *newVar = malloc(sizeof(VAR_LINK));

    newVar -> var = malloc(strlen(var) + 1);
    strcpy(newVar -> var, var);

    newVar -> val = malloc(strlen(val) + 1);
    strcpy(newVar -> val, val);

    // debug("\t storing var : %s = %s in store_set_string",newVar -> var,newVar -> val);


    VAR_LINK *last =  varHead.prev;
    newVar -> prev = last;
    newVar -> next = &varHead;
    varHead.prev = newVar;
    last -> next = newVar;

    return 0;
}

/**
 * @brief  Set the value of a variable as an integer.
 * @details  This function sets the current value of a specified
 * variable to be a specified integer.  If the variable already
 * has a value, then that value is replaced.  Ownership of the variable
 * string is not transferred to the data store module as a result of
 * this call; the data store module makes such copies of this string
 * as it may require.
 *
 * @param  var  The variable whose value is to be set.
 * @param  val  The value to set.
 */
int store_set_int(char *var, long val) {
    debug("storing var : %s = %ld in store_set_int ",var,val);

    char valString[255];
    sprintf(valString, "%ld", val);
    debug("int to string result is : %s",valString);

    return store_set_string(var, valString);
}

/**
 * @brief  Print the current contents of the data store.
 * @details  This function prints the current contents of the data store
 * to the specified output stream.  The format is not specified; this
 * function is intended to be used for debugging purposes.
 *
 * @param f  The stream to which the store contents are to be printed.
 */
void store_show(FILE *f) {
    debug("---- stored value ----");
    VAR_LINK *head = &varHead;
    VAR_LINK *node = varHead.next;
    fprintf(f, "{");
    while(node != head){
        fprintf(f,"%s -> %s \t",node -> var, node -> val);
        // debug("\t\t var : %d -> %d",node -> var, node -> val);

        node = node -> next;
    }
        fprintf(f, "}");


}
