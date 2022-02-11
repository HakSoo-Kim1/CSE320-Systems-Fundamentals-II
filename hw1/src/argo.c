// #include <stdlib.h>
// #include <stdio.h>
// // #include <math.h>



// #include "argo.h"
// #include "global.h"
// #include "debug.h"

// int argo_write_basic(ARGO_VALUE *v, FILE *f);
// int argo_write_object(ARGO_OBJECT *o, FILE *f);
// int argo_write_array(ARGO_ARRAY *a, FILE *f);
// int intToHex(int value);
// int spaceGenerator(long spaceRequired);
// int indentation = 0; 

// /**
//  * @brief  Read JSON input from a specified input stream, parse it,
//  * and return a data structure representing the corresponding value.
//  * @details  This function reads a sequence of 8-bit bytes from
//  * a specified input stream and attempts to parse it as a JSON value,
//  * according to the JSON syntax standard.  If the input can be
//  * successfully parsed, then a pointer to a data structure representing
//  * the corresponding value is returned.  See the assignment handout for
//  * information on the JSON syntax standard and how parsing can be
//  * accomplished.  As discussed in the assignment handout, the returned
//  * pointer must be to one of the elements of the argo_value_storage
//  * array that is defined in the const.h header file.
//  * In case of an error (these include failure of the input to conform
//  * to the JSON standard, premature EOF on the input stream, as well as
//  * other I/O errors), a one-line error message is output to standard error
//  * and a NULL pointer value is returned.
//  *
//  * @param f  Input stream from which JSON is to be read.
//  * @return  A valid pointer if the operation is completely successful,
//  * NULL if there is any error.
//  */
// // ARGO_VALUE *argo_read_value(FILE *f) {
// //     // TO BE IMPLEMENTED.
// //     abort();
// // }

// /**
//  * @brief  Read JSON input from a specified input stream, attempt to
//  * parse it as a JSON string literal, and return a data structure
//  * representing the corresponding string.
//  * @details  This function reads a sequence of 8-bit bytes from
//  * a specified input stream and attempts to parse it as a JSON string
//  * literal, according to the JSON syntax standard.  If the input can be
//  * successfully parsed, then a pointer to a data structure representing
//  * the corresponding value is returned.
//  * In case of an error (these include failure of the input to conform
//  * to the JSON standard, premature EOF on the input stream, as well as
//  * other I/O errors), a one-line error message is output to standard error
//  * and a NULL pointer value is returned.
//  *
//  * @param f  Input stream from which JSON is to be read.
//  * @return  Zero if the operation is completely successful,
//  * nonzero if there is any error.
//  */
// // int argo_read_string(ARGO_STRING *s, FILE *f) {
// //     // TO BE IMPLEMENTED.
// //     abort();
// // }

// /**
//  * @brief  Read JSON input from a specified input stream, attempt to
//  * parse it as a JSON number, and return a data structure representing
//  * the corresponding number.
//  * @details  This function reads a sequence of 8-bit bytes from
//  * a specified input stream and attempts to parse it as a JSON numeric
//  * literal, according to the JSON syntax standard.  If the input can be
//  * successfully parsed, then a pointer to a data structure representing
//  * the corresponding value is returned.  The returned value must contain
//  * (1) a string consisting of the actual sequence of characters read from
//  * the input stream; (2) a floating point representation of the corresponding
//  * value; and (3) an integer representation of the corresponding value,
//  * in case the input literal did not contain any fraction or exponent parts.
//  * In case of an error (these include failure of the input to conform
//  * to the JSON standard, premature EOF on the input stream, as well as
//  * other I/O errors), a one-line error message is output to standard error
//  * and a NULL pointer value is returned.
//  *
//  * @param f  Input stream from which JSON is to be read.
//  * @return  Zero if the operation is completely successful,
//  * nonzero if there is any error.
//  */
// // int argo_read_number(ARGO_NUMBER *n, FILE *f) {
// //     // TO BE IMPLEMENTED.
// //     abort();
// // }

// /**
//  * @brief  Write canonical JSON representing a specified value to
//  * a specified output stream.
//  * @details  Write canonical JSON representing a specified value
//  * to specified output stream.  See the assignment document for a
//  * detailed discussion of the data structure and what is meant by
//  * canonical JSON.
//  *
//  * @param v  Data structure representing a value.
//  * @param f  Output stream to which JSON is to be written.
//  * @return  Zero if the operation is completely successful,
//  * nonzero if there is any error.
//  */
// int argo_write_value(ARGO_VALUE *v, FILE *f) {
//     indent_level = 0;
//     if ((global_options & PRETTY_PRINT_OPTION) == PRETTY_PRINT_OPTION){   // pretty print option has been specified 
//         indentation = global_options & 0x00FF;
//         debug("!! The given indentation is %d",indentation);
//     }
//     else{
//         indentation = 0;
//     }

//     if (v -> type == ARGO_NO_TYPE){                 // no type
//         return -1;
//     }
//     else if (v -> type == ARGO_BASIC_TYPE){         // basic (True, False, Null)
//         argo_write_basic(v, stdout);
//     }
//     else if (v -> type == ARGO_NUMBER_TYPE){        // number
//         argo_write_number(& v -> content.number, f);
//     }
//     else if (v -> type == ARGO_STRING_TYPE){        // String
//         argo_write_string(& v -> content.string, f);
//     }
//     else if (v -> type == ARGO_OBJECT_TYPE){        // object
//         argo_write_object(& v -> content.object, f);
//     }
//     else if (v -> type == ARGO_ARRAY_TYPE){         // array
//         argo_write_array(& v -> content.array, f);
//     }
//     if(indentation){
//         putchar(ARGO_LF);
//     }
//     return 1;
// }

// /**
//  * @brief  Write canonical JSON representing a specified string
//  * to a specified output stream.
//  * @details  Write canonical JSON representing a specified string
//  * to specified output stream.  See the assignment document for a
//  * detailed discussion of the data structure and what is meant by
//  * canonical JSON.  The argument string may contain any sequence of
//  * Unicode code points and the output is a JSON string literal,
//  * represented using only 8-bit bytes.  Therefore, any Unicode code
//  * with a value greater than or equal to U+00FF cannot appear directly
//  * in the output and must be represented by an escape sequence.
//  * There are other requirements on the use of escape sequences;
//  * see the assignment handout for details.
//  *
//  * @param v  Data structure representing a string (a sequence of
//  * Unicode code points).
//  * @param f  Output stream to which JSON is to be written.
//  * @return  Zero if the operation is completely successful,
//  * nonzero if there is any error.
//  */
// int argo_write_string(ARGO_STRING *s, FILE *f) {
//     debug("Processing String");
//     size_t count = 0;
//     putchar(ARGO_QUOTE); 
//     while (count != s -> length){
//         if ( *((s -> content) + count) == ARGO_BS){
//             putchar(ARGO_BSLASH);
//             putchar(ARGO_B);
//         }
//         else if ( *((s -> content) + count) == ARGO_FF){
//             putchar(ARGO_BSLASH);
//             putchar(ARGO_F);
//         }
//         else if ( *((s -> content) + count) == ARGO_LF){
//             putchar(ARGO_BSLASH);
//             putchar(ARGO_N);
//         }
//         else if ( *((s -> content) + count) == ARGO_CR){
//             putchar(ARGO_BSLASH);
//             putchar(ARGO_R);
//         }
//         else if ( *((s -> content) + count) == ARGO_HT){
//             putchar(ARGO_BSLASH);
//             putchar(ARGO_T);
//         }
//         else if (*((s -> content) + count) == ARGO_BSLASH){
//             putchar(ARGO_BSLASH);
//             putchar(ARGO_BSLASH);
//         }
//         else if (*((s -> content) + count) == ARGO_QUOTE){
//             putchar(ARGO_BSLASH);
//             putchar(ARGO_QUOTE);
//         }
//         else if ( 0x001F < *((s -> content) + count) && *((s -> content) + count) <  0x00FF){
//             putchar(*((s -> content) + count));
//         }
//         else{
//             putchar(ARGO_BSLASH);
//             putchar(ARGO_U);
//             int number = *((s -> content) + count);
//             int divisor = 4096;
//             while(divisor != 0){
//                 if (number >= divisor){
//                     int dividend = number/divisor;
//                     debug("%d",dividend);
//                     intToHex(dividend);
//                     number -= divisor * dividend;
//                 }
//                 else{
//                     putchar('0');
//                 }
//                 divisor /= 16;
//             }
//         }
//         count ++;
//     }
//     putchar(ARGO_QUOTE);
//     return 0;
// }


// *
//  * @brief  Write canonical JSON representing a specified number
//  * to a specified output stream.
//  * @details  Write canonical JSON representing a specified number
//  * to specified output stream.  See the assignment document for a
//  * detailed discussion of the data structure and what is meant by
//  * canonical JSON.  The argument number may contain representations
//  * of the number as any or all of: string conforming to the
//  * specification for a JSON number (but not necessarily canonical),
//  * integer value, or floating point value.  This function should
//  * be able to work properly regardless of which subset of these
//  * representations is present.
//  *
//  * @param v  Data structure representing a number.
//  * @param f  Output stream to which JSON is to be written.
//  * @return  Zero if the operation is completely successful,
//  * nonzero if there is any error.
 
// int argo_write_number(ARGO_NUMBER *n, FILE *f) {
//     debug("Processing Number");

//     if (n -> valid_int){
//         long number = n -> int_value; 
//         if (number == 0){
//             putchar(ARGO_DIGIT0);

//         }
//         else{
//             int counter = 0; 
//             while (counter != ARGO_MAX_DIGITS){
//                 *(argo_digits + counter) = (number % 10) + '0';
//                 number /= 10;
//                 counter += 1;
//             }
//             if (number != 0){
//                 fprintf(stderr, "only maximum %d digits numbers allowed \n", ARGO_MAX_DIGITS);                   /// good? 
//                 return -1;
//             }
//             counter = ARGO_MAX_DIGITS - 1;
//             int flag = 0;
//             while(counter >= 0){
//                 if(*(argo_digits + counter) != '0'){
//                     flag = 1;
//                 }
//                 if (flag){
//                     putchar(*(argo_digits + counter));
//                 }
//                 counter -= 1;
//             }
//         }
//     }
//     else if (n -> valid_float){         /// consider 0 and 0.0 
//         debug("FLOATING : %f", n->float_value);
//         double number = n -> float_value;
//         int exponent = 0;
//         int isNegative = 0;
//         if (number < 0){
//             isNegative = 1;
//             number *= -1;
//         }
//         if (number == 0){
//             putchar(ARGO_DIGIT0);
//             putchar(ARGO_PERIOD);
//             putchar(ARGO_DIGIT0);
//         }
//         else{
//             while(!(0.1 <= number && number < 1.0)){
//                 if (number >= 1){
//                     number /= 10;
//                     exponent += 1;
//                 }
//                 else{
//                     number *= 10;
//                     exponent -= 1;
//                 }
//             }
//             debug("number is : %f",number);
//             debug("exponent : %d", exponent);
//             int counter = 1;
//             if (isNegative){
//                 putchar('-');
//             }
//             putchar('0');
//             putchar(ARGO_PERIOD);
//             while(counter <= ARGO_PRECISION + 1 && number != 0){
//                 number *= 10;
//                 int place = (int)number ;
//                 debug("place is %d",place);
//                 putchar(place + '0');
//                 number -= place;
//                 counter += 1;
//             }
//             if (exponent != 0){
//                 putchar(ARGO_E);
//                 if (exponent < 0){
//                     putchar(ARGO_MINUS);
//                     exponent *= -1;
//                 }

//                 counter = 0; 
//                 while (counter != ARGO_MAX_DIGITS){
//                     *(argo_digits + counter) = (exponent % 10) + '0';
//                     exponent /= 10;
//                     counter += 1;
//                 }
//                 if (exponent != 0){
//                     fprintf(stderr, "only maximum %d digits numbers allowed \n", ARGO_MAX_DIGITS);                   /// good? 
//                     return -1;
//                 }
//                 counter = ARGO_MAX_DIGITS - 1;
//                 int flag = 0;
//                 while(counter >= 0){
//                     if(*(argo_digits + counter) != '0'){
//                         flag = 1;
//                     }
//                     if (flag){
//                         putchar(*(argo_digits + counter));
//                     }
//                     counter -= 1;
//                 }
//             }
//         }
//     }
//     return 0;
// }

// int argo_write_basic(ARGO_VALUE *v, FILE *f) {
//     debug("Processing Basic");
//     if(v -> content.basic == ARGO_NULL){
//         fputs(ARGO_NULL_TOKEN, f);
//     }
//     else if(v -> content.basic == ARGO_TRUE){
//         fputs(ARGO_TRUE_TOKEN, f);
//     }
//     else if(v -> content.basic == ARGO_FALSE){
//         fputs(ARGO_FALSE_TOKEN, f);
//     }
//     return 0;
// }

// int argo_write_object(ARGO_OBJECT *o, FILE *f) {
//     debug("Processing Object");
//     putchar(ARGO_LBRACE);
//     if(indentation){
//         putchar(ARGO_LF);
//     }
//     indent_level += 1;
//     ARGO_VALUE *node = o -> member_list -> next;
//     while (node -> type != ARGO_NO_TYPE){
//         if (indentation){
//             spaceGenerator(indentation * indent_level);
//         }
//         argo_write_string(& node -> name, f);
//         putchar(ARGO_COLON);
//         if(indentation){
//             putchar(ARGO_SPACE);
//         }        
//         if (node -> type == ARGO_BASIC_TYPE){         // basic (True, False, Null)
//             argo_write_basic(node, f);
//         }
//         else if (node -> type == ARGO_NUMBER_TYPE){        // number
//             argo_write_number(& node -> content.number, f);
//         }
//         else if (node -> type == ARGO_STRING_TYPE){        // String
//             argo_write_string(& node -> content.string, f);
//         }
//         else if (node -> type == ARGO_OBJECT_TYPE){        // object
//             argo_write_object(& node -> content.object, f);
//         }
//         else if (node -> type == ARGO_ARRAY_TYPE){         // array
//             argo_write_array(& node -> content.array, f);
//         }
//         node = node -> next;
//         if (node -> type != ARGO_NO_TYPE){
//             putchar(ARGO_COMMA); 
//         }
//         if(indentation){
//             putchar(ARGO_LF);
//         }
//     }
//     indent_level -= 1;
//     if (indentation){
//         spaceGenerator(indentation * indent_level);
//     }
//     putchar(ARGO_RBRACE);
//     return 0;
// }

// int argo_write_array(ARGO_ARRAY *a, FILE *f) {
//     debug("Processing Array");
//     putchar(ARGO_LBRACK);
//     if(indentation){
//         putchar(ARGO_LF);
//     }
//     indent_level += 1;
//     ARGO_VALUE *node = a -> element_list -> next;
//     while (node -> type != ARGO_NO_TYPE){
//         if (indentation){
//             spaceGenerator(indentation * indent_level);
//         }
//         if (node -> type == ARGO_BASIC_TYPE){         // basic (True, False, Null)
//             argo_write_basic(node, stdout);
//         }
//         else if (node -> type == ARGO_NUMBER_TYPE){        // number
//             argo_write_number(& node -> content.number, stdout);
//         }
//         else if (node -> type == ARGO_STRING_TYPE){        // String
//             argo_write_string(& node -> content.string, stdout);
//         }
//         else if (node -> type == ARGO_OBJECT_TYPE){        // object
//             argo_write_object(& node -> content.object, f);
//         }
//         else if (node -> type == ARGO_ARRAY_TYPE){         // array
//             argo_write_array(& node -> content.array,f);
//         }
//         node = node -> next;
//         if (node -> type != ARGO_NO_TYPE){
//             putchar(ARGO_COMMA); 
//         }
//         if(indentation){
//             putchar(ARGO_LF);
//         }
//     }
//     indent_level -= 1;
//     if (indentation){
//         spaceGenerator(indentation * indent_level);
//     }
//     putchar(ARGO_RBRACK);
//     return 0;
// }

// int intToHex(int value){
//     if (value < 10){
//         putchar(value + 48);
//         return 0;
//     }
//     else if (value == 10){putchar('a');}
//     else if (value == 11){putchar('b');}
//     else if (value == 12){putchar('c');}
//     else if (value == 13){putchar('d');}
//     else if (value == 14){putchar('e');}
//     else if (value == 15){putchar('f');}
//     return 0;
// }

// int spaceGenerator(long spaceRequired){
//     long counter = 0;
//     while (counter < spaceRequired){
//         putchar(ARGO_SPACE);
//         counter += 1;
//     }
//     return 0;
// }


//         // double x = n -> valid_float;
//         // int exp;
//         // double mantissa = frexp (x, &exp);
//         // debug("%d",exp);
        
//         // double L = *((long *) & x);
//         // byte s = (byte)(L >> 63);
//         // int e = ((int)(L >> 52) & 0x7FF) - 0x3FF;
//         // long f = (L & 0x000FFFFFFFFFFFFF);


//     // if (n -> valid_string != 0){       // can be represented as int
//     //     int count = 0;
//     //     while (count != n -> string_value.length){
//     //     putchar(*((n -> string_value.content) + count));
//     //     // debug("%c",*((n -> string_value.content) + count));
//     //     count ++;
//     //     }
//     // }
//     // else{
//     //     debug("corresponding number of valid string not exist");
//     // }