// #include <stdlib.h>
// #include <stdio.h>



// #include "argo.h"
// #include "global.h"
// #include "debug.h"

// int argo_read_basic(ARGO_VALUE *v, FILE *f);
// int argo_read_array(ARGO_ARRAY *a, FILE *f);
// int argo_read_object(ARGO_OBJECT *o, FILE *f);
// int argo_write_basic(ARGO_VALUE *v, FILE *f);
// int argo_write_object(ARGO_OBJECT *o, FILE *f);
// int argo_write_array(ARGO_ARRAY *a, FILE *f);
// int intToHex(int value, FILE *f);
// int hexToInt(char c);

// int spaceGenerator(long spaceRequired, FILE *f);
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
//  * and a NULL pointer value is returned
//  *
//  * @param f  Input stream from which JSON is to be read.
//  * @return  A valid pointer if the operation is completely successful,
//  * NULL if there is any error.
//  */
// ARGO_VALUE *argo_read_value(FILE *f) {
//     char c;
//     while ( (c = fgetc(f)) != EOF){
//         if (argo_is_whitespace(c)){
//             continue;
//         }
//         else if (argo_is_digit(c) || c == ARGO_MINUS){ 
//             ungetc(c, f);
//             (argo_value_storage + argo_next_value) -> type  = ARGO_NUMBER_TYPE;
//             if (argo_read_number(&((argo_value_storage + argo_next_value) -> content.number), f) == -1){
//                 return NULL;
//             }
//             argo_next_value += 1;
//             c = fgetc(f);
//             while(argo_is_whitespace(c)){
//                 c = fgetc(f);
//             }
//             if (c == EOF){
//                 return &argo_value_storage[0];
//             }
//             else{
//                 fprintf(stderr, "unknown character found.\n");               
//                 return NULL;
//             }
//         }
//         else if (c == ARGO_T || c == ARGO_F || c == ARGO_N){
//             ungetc(c, f);
//             (argo_value_storage + argo_next_value) -> type = ARGO_BASIC_TYPE;
//             if (argo_read_basic(argo_value_storage + argo_next_value, f) == -1){
//                 return NULL;
//             }
//             argo_next_value += 1;
//             c = fgetc(f);
//             while(argo_is_whitespace(c)){
//                 c = fgetc(f);
//             }
//             if (c == EOF){
//                 return &argo_value_storage[0];
//             }
//             else{
//                 fprintf(stderr, "unknown character found.\n");               
//                 return NULL;
//             }

//         }
//         else if (c == ARGO_QUOTE){
//             ungetc(c, f);
//             (argo_value_storage + argo_next_value) -> type = ARGO_STRING_TYPE;
//             if (argo_read_string(&((argo_value_storage + argo_next_value) -> content.string), f) == -1){
//                 return NULL;
//             }
//             argo_next_value += 1;
//             c = fgetc(f);
//             while(argo_is_whitespace(c)){
//                 c = fgetc(f);
//             }
//             if (c == EOF){
//                 return &argo_value_storage[0];
//             }
//             else{
//                 fprintf(stderr, "unknown character found.\n");               
//                 return NULL;
//             }
//         }
//         else if (c == ARGO_LBRACK){
//             (argo_value_storage + argo_next_value) -> type = ARGO_ARRAY_TYPE;
//             if (argo_read_array(&((argo_value_storage + argo_next_value) -> content.array), f) == -1){
//                 return NULL;
//             }
//             c = fgetc(f);
//             while(argo_is_whitespace(c)){
//                 c = fgetc(f);
//             }
//             if (c == EOF){
//                 return &argo_value_storage[0];
//             }
//             else{
//                 fprintf(stderr, "unknown character found.\n");               
//                 return NULL;
//             }
//         }
//         else if (c == ARGO_LBRACE){
//             (argo_value_storage + argo_next_value) -> type = ARGO_OBJECT_TYPE;
//             if (argo_read_object(&((argo_value_storage + argo_next_value) -> content.object), f) == -1){
//                 return NULL;
//             }
//             c = fgetc(f);
//             while(argo_is_whitespace(c)){
//                 c = fgetc(f);
//             }
//             if (c == EOF){
//                 return &argo_value_storage[0];
//             }
//             else{
//                 fprintf(stderr, "unknown character found.\n");               
//                 return NULL;
//             }
//         }
//         else{
//             fprintf(stderr, "unknown character found. Possible that wrong format given.\n");               
//             return NULL;
//         }
//     }
//     if (argo_next_value == 0){
//         fprintf(stderr, "Empty File.\n");               
//         return NULL;
//     }
//     return &argo_value_storage[0];

// }


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
// int argo_read_string(ARGO_STRING *s, FILE *f) {
//     char c = fgetc(f);
//     c = fgetc(f);
//     while (c != ARGO_QUOTE){
//         if (c == ARGO_BSLASH){
//             c = fgetc(f);
//             if (c == ARGO_B){
//                 argo_append_char(s, ARGO_BS);
//             }
//             else if (c == ARGO_F){
//                 argo_append_char(s, ARGO_FF);
//             }
//             else if (c == ARGO_N){
//                 argo_append_char(s, ARGO_LF);
//             }
//             else if (c == ARGO_R){
//                 argo_append_char(s, ARGO_CR);
//             }
//             else if (c == ARGO_R){
//                 argo_append_char(s, ARGO_CR);
//             }
//             else if (c == ARGO_T){
//                 argo_append_char(s, ARGO_HT);
//             }
//             else if (c == ARGO_BSLASH){
//                 argo_append_char(s, ARGO_BSLASH);
//             } 
//             else if (c == ARGO_QUOTE){
//                 argo_append_char(s, ARGO_QUOTE);
//             }
//             else if (c == ARGO_FSLASH){
//                 argo_append_char(s, ARGO_FSLASH);
//             }
//             else if (c == ARGO_U){
//                 int counter = 4;
//                 int hex;
//                 while(counter != 0){
//                     c = getchar();
//                     if (!argo_is_hex(c)){
//                         fprintf(stderr, "wrong hex value given.\n");      
//                         return -1;     
//                     }
//                     if (counter == 4){
//                         hex = hexToInt(c);
//                     }
//                     else{
//                         hex *= 16;
//                         hex += hexToInt(c);
//                     }
//                     counter -= 1;
//                 }
//                 if (hex == 8){
//                     argo_append_char(s, ARGO_BS);
//                 }
//                 else if (hex == 12){
//                     argo_append_char(s, ARGO_FF);
//                 }
//                 else if (hex == 10){
//                     argo_append_char(s, ARGO_LF);
//                 }
//                 else if (hex == 13){
//                     argo_append_char(s, ARGO_CR);
//                 }
//                 else if (hex == 9){
//                     argo_append_char(s, ARGO_HT);
//                 }
//                 else if (hex == 47){
//                     argo_append_char(s, ARGO_FSLASH);
//                 }
//                 else{
//                     argo_append_char(s, hex);
//                 }
//             }
//             else{
//                 fprintf(stderr, "wrong escaping letter.\n");      
//                 return -1;
//             }
//         }
//         else if (c == EOF){
//             fprintf(stderr, "string does not end.\n");   
//             return -1;   
//         }
//         else{
//             argo_append_char(s, c);
//         }
//         c = fgetc(f);
//     }


//     return 0;

// }

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
// int argo_read_number(ARGO_NUMBER *n, FILE *f) {
//     char c = fgetc(f);
//     n -> valid_int = 1;
//     n -> valid_float = 1;
//     n -> int_value = 0;
//     n -> float_value = 0;
//     double floatPoint;
//     int isNegative = 0;
//     int isExponentNegative = 0;
//     while (argo_is_digit(c) || c == ARGO_PERIOD || c == ARGO_MINUS){
//         if (c == ARGO_MINUS){
//             if (n -> int_value != 0 || n -> float_value != 0){
//                 fprintf(stderr, "wrong minus place\n");                  
//                 return -1;
//             }
//             isNegative = 1;
//         }
//         else if (c == ARGO_PERIOD){
//             if (n->valid_float == 1 && n -> valid_int == 0){
//                 fprintf(stderr, "more than one point exist in one float number\n");
//                 return -1;
//             }
//             floatPoint = 0.1;
//             n -> valid_int = 0;
//             n -> int_value = 0;
//         }
//         else if (argo_is_digit(c)){
//             if (n->valid_float == 1 && n -> valid_int == 0){
//                 n -> float_value +=  (c - '0') * floatPoint;
//                 floatPoint /= 10;
//             }
//             else{
//                 if (n -> int_value == 0 && n -> float_value == 0){
//                     n -> int_value += c - '0';
//                     n -> float_value += c - '0';
//                 }
//                 else{
//                     n -> int_value *= 10;
//                     n -> int_value += c - '0';
//                     n -> float_value *= 10;
//                     n -> float_value += c - '0';
//                 }
//             }
//         }
//         argo_append_char(&(n -> string_value), (int)c);
//         c = fgetc(f);
//     }
//     if (isNegative){
//         if (n->valid_float == 1 && n -> valid_int == 0){
//             n -> float_value *= -1;
//         }
//         else{
//             n -> float_value *= -1;
//             n -> int_value *= -1;
//         }
//     }
//     if (argo_is_exponent(c)){
//         n -> valid_int = 0;
//         n -> int_value = 0;        
//         argo_append_char(&(n -> string_value), (int)c);
//         c = fgetc(f);
//         if(c == ARGO_MINUS){
//             argo_append_char(&(n -> string_value), (int)c);
//             isExponentNegative = 1;
//             c = fgetc(f);
//         }
//         int exponent = 0;
//         while(argo_is_digit(c)){
//             if (exponent == 0){
//                 exponent = c - '0';
//             }
//             else{
//                 exponent *= 10;
//                 exponent += c - '0';
//             }
//             argo_append_char(&(n -> string_value), (int)c);
//             c = fgetc(f);
//         }
//         double num = 1;
//         while(exponent != 0){
//             if (isExponentNegative){
//                 num /= 10;
//             }
//             else{
//                 num *= 10;
//             }
//             exponent -= 1;
//         }
//         n -> float_value *= num;
//     }
//     ungetc(c, f);
//     return 0;
// }

// int argo_read_basic(ARGO_VALUE *v, FILE *f) {
//     char c = fgetc(f);
//     if (c == ARGO_T){
//         c = fgetc(f);
//         if (c == ARGO_R){
//             c = fgetc(f);
//             if (c == ARGO_U){
//                 c = fgetc(f);
//                 if (c == ARGO_E){
//                     v -> content.basic = ARGO_TRUE;
//                     return 0;
//                 }
//             }
//         }
//     }
//     else if (c == ARGO_F){
//         c = fgetc(f);
//         if (c == 'a'){
//             c = fgetc(f);
//             if (c == 'l'){
//                 c = fgetc(f);
//                 if (c == 's'){
//                     c = fgetc(f);
//                     if (c == 'e'){
//                         v -> content.basic = ARGO_FALSE;
//                         return 0;
//                     } 
//                 }
//             }
//         }
//     }
//     else if (c == ARGO_N){
//         c = fgetc(f);
//         if (c == ARGO_U){
//             c = fgetc(f);
//             if (c == 'l'){
//                 c = fgetc(f);
//                 if (c == 'l'){
//                     v -> content.basic = ARGO_NULL;
//                     return 0;
//                 }
//             }
//         }
//     }
//     fprintf(stderr, "unknown character found. Possibly wrong basic given.\n");                 
//     return -1;


// }

// int argo_read_array(ARGO_ARRAY *a, FILE *f){
//     argo_next_value += 1;
//     if (NUM_ARGO_VALUES == argo_next_value){
//         fprintf(stderr, "argo_value_storage is full\n");                 
//         return -1;
//     }
//     a -> element_list = argo_value_storage + argo_next_value;
//     a -> element_list -> next = a -> element_list;
//     a -> element_list -> prev = a -> element_list;
//     argo_next_value += 1;
//     if (NUM_ARGO_VALUES == argo_next_value){
//         fprintf(stderr, "argo_value_storage is full\n");                 
//         return -1;
//     }
//     int nextFlag = 1;
//     char c = fgetc(f);
//     while(c != ARGO_RBRACK){
//         if (argo_is_whitespace(c) || c == ARGO_COMMA){
//             if (c == ARGO_COMMA){
//                 if (nextFlag){ fprintf(stderr, "comma place is wrong. %d\n",__LINE__);  return -1; }
//                 nextFlag = 1;
//                 char d = fgetc(f);
//                 while(argo_is_whitespace(d)){
//                     d = fgetc(f);
//                 }
//                 if (d == ARGO_RBRACK){
//                     fprintf(stderr, "comma place is wrong. %d\n",__LINE__);  
//                     return -1;
//                 }
//                 else{
//                     ungetc(d,f);
//                 }
//             }
//             c = fgetc(f);
//             continue;
//         }
//         else if (argo_is_digit(c) || c == ARGO_MINUS ||(c == ARGO_T || c == ARGO_F || c == ARGO_N) || c == ARGO_QUOTE || c == ARGO_LBRACK || c == ARGO_LBRACE){ 
//             if (!nextFlag){ fprintf(stderr, "comma place is wrong. %d\n",__LINE__);  return -1; }
//             nextFlag = 0;
//             ARGO_VALUE * last = a -> element_list -> prev;
//             (argo_value_storage + argo_next_value) -> prev = last;
//             (argo_value_storage + argo_next_value) -> next = a -> element_list;
//             a -> element_list -> prev = argo_value_storage + argo_next_value;
//             last -> next = argo_value_storage + argo_next_value;
//             if (argo_is_digit(c) || c == ARGO_MINUS){
//                 ungetc(c,f);
//                 (argo_value_storage + argo_next_value) -> type  = ARGO_NUMBER_TYPE;
//                 if (argo_read_number(&((argo_value_storage + argo_next_value) -> content.number), f) == -1){ return -1;}
//                 argo_next_value += 1;
//                 if (NUM_ARGO_VALUES == argo_next_value){
//                 fprintf(stderr, "argo_value_storage is full\n");                 
//                 return -1;
//                 }
//             }
//             else if (c == ARGO_T || c == ARGO_F || c == ARGO_N){
//                 ungetc(c,f);
//                 (argo_value_storage + argo_next_value) -> type = ARGO_BASIC_TYPE;
//                 if (argo_read_basic(argo_value_storage + argo_next_value, f) == -1){ return -1;}
//                 argo_next_value += 1;
//                 if (NUM_ARGO_VALUES == argo_next_value){
//                 fprintf(stderr, "argo_value_storage is full\n");                 
//                 return -1;
//                 }
//             }
//             else if (c == ARGO_QUOTE){
//                 ungetc(c, f);
//                 (argo_value_storage + argo_next_value) -> type = ARGO_STRING_TYPE;
//                 if (argo_read_string(&((argo_value_storage + argo_next_value) -> content.string), f) == -1){ return -1;}
//                 argo_next_value += 1;
//                 if (NUM_ARGO_VALUES == argo_next_value){
//                 fprintf(stderr, "argo_value_storage is full\n");                 
//                 return -1;
//                 }
//             }
//             else if (c == ARGO_LBRACK){
//                 (argo_value_storage + argo_next_value) -> type = ARGO_ARRAY_TYPE;
//                 if (argo_read_array(&((argo_value_storage + argo_next_value) -> content.array), f) == -1){ return -1;}
//             }
//             else if (c == ARGO_LBRACE){
//                 (argo_value_storage + argo_next_value) -> type = ARGO_OBJECT_TYPE;
//                 if (argo_read_object(&((argo_value_storage + argo_next_value) -> content.object), f) == -1){ return -1;}
//             }
//         }
//         else{ 
//             fprintf(stderr, "unknown character found. Possible that wrong format given.%d\n",__LINE__); 
//             return -1;
//         }
//         c = fgetc(f);
//     }
//     return 0;
// }

// int argo_read_object(ARGO_OBJECT *o, FILE *f) {
//     argo_next_value += 1;
//     if (NUM_ARGO_VALUES == argo_next_value){
//         fprintf(stderr, "argo_value_storage is full\n");                 
//         return -1;
//     }
//     o -> member_list = argo_value_storage + argo_next_value;
//     o -> member_list -> next = o -> member_list;
//     o -> member_list -> prev = o -> member_list;
//     argo_next_value += 1;
//     if (NUM_ARGO_VALUES == argo_next_value){
//         fprintf(stderr, "argo_value_storage is full\n");                 
//         return -1;
//     }
//     int flag = 1;   // 1 == name turn, 2 == quote turn, 3 == content turn
//     char c = fgetc(f);
//     while(c != ARGO_RBRACE){
//         if (flag == 1){         // name turn 
//             if (argo_is_whitespace(c)){
//                 c = fgetc(f);
//                 continue;
//             }
//             else if (c == ARGO_QUOTE){
//                 ungetc(c, f);
//                 ARGO_VALUE * last = o -> member_list -> prev;
//                 (argo_value_storage + argo_next_value) -> prev = last;
//                 (argo_value_storage + argo_next_value) -> next = o -> member_list;
//                 o -> member_list -> prev = argo_value_storage + argo_next_value;
//                 last -> next = argo_value_storage + argo_next_value;
//                 (argo_value_storage + argo_next_value) -> type  = ARGO_STRING_TYPE;
//                 if (argo_read_string(&((argo_value_storage + argo_next_value) -> name), f) == -1){ return -1;}
//                 flag = 2;
//             }
//             else{
//                 fprintf(stderr, "Wrong object format is given.\n"); 
//                 return -1;
//             }
//         }
//         else if (flag == 2){    // quote turn
//                 if (argo_is_whitespace(c)){
//                     c = fgetc(f);
//                     continue;
//                 }
//                 else if (c == ARGO_COLON){
//                     flag = 3;
//                 }  
//                 else{
//                     fprintf(stderr, "wrong colon format.\n"); 
//                     return -1;
//                 }        
//         }
//         else if (flag == 3){    // content turn
//             if (argo_is_whitespace(c)){
//                 c = fgetc(f);
//                 continue;
//             }
//             else if (argo_is_digit(c) || c == ARGO_MINUS || (c == ARGO_T || c == ARGO_F || c == ARGO_N) || c == ARGO_QUOTE || c == ARGO_LBRACE || c == ARGO_LBRACK ){ 
//                 flag = 1;
//                 if (argo_is_digit(c) || c == ARGO_MINUS){
//                     ungetc(c,f);
//                     (argo_value_storage + argo_next_value) -> type  = ARGO_NUMBER_TYPE;
//                     if (argo_read_number(&((argo_value_storage + argo_next_value) -> content.number), f) == -1){ return -1;}
//                     argo_next_value += 1;
//                     if (NUM_ARGO_VALUES == argo_next_value){
//                     fprintf(stderr, "argo_value_storage is full\n");                 
//                     return -1;
//                     }
//                 }
//                 else if (c == ARGO_T || c == ARGO_F || c == ARGO_N){
//                     ungetc(c,f);
//                     (argo_value_storage + argo_next_value) -> type = ARGO_BASIC_TYPE;
//                     if (argo_read_basic(argo_value_storage + argo_next_value, f) == -1){ return -1;}
//                     argo_next_value += 1;
//                     if (NUM_ARGO_VALUES == argo_next_value){
//                     fprintf(stderr, "argo_value_storage is full\n");                 
//                     return -1;
//                     }
//                 }
//                 else if (c == ARGO_QUOTE){
//                     ungetc(c, f);
//                     (argo_value_storage + argo_next_value) -> type = ARGO_STRING_TYPE;
//                     if (argo_read_string(&((argo_value_storage + argo_next_value) -> content.string), f) == -1){ return -1;}
//                     argo_next_value += 1;
//                     if (NUM_ARGO_VALUES == argo_next_value){
//                     fprintf(stderr, "argo_value_storage is full\n");                 
//                     return -1;
//                     }
//                 }
//                 else if (c == ARGO_LBRACE){
//                     (argo_value_storage + argo_next_value) -> type = ARGO_OBJECT_TYPE;
//                     if (argo_read_object(&((argo_value_storage + argo_next_value) -> content.object), f) == -1){ return -1;}
//                 }
//                 else if (c == ARGO_LBRACK){
//                     (argo_value_storage + argo_next_value) -> type = ARGO_ARRAY_TYPE;
//                     if (argo_read_array(&((argo_value_storage + argo_next_value) -> content.array), f) == -1){ return -1;}
//                 }
//                 c = fgetc(f);
//                 while (argo_is_whitespace(c)){
//                     c = fgetc(f);
//                 }
//                 if (c == ARGO_RBRACE){
//                     ungetc(c,f);
//                 }
//                 else if (c == ARGO_COMMA){
//                     flag = 1;
//                     c = fgetc(f);
//                     while (argo_is_whitespace(c)){
//                         c = fgetc(f);
//                     }
//                     if (c == ARGO_RBRACE){
//                         fprintf(stderr, "right brace cannot be after comma\n"); 
//                         return -1;
//                     }
//                     else{
//                         ungetc(c,f);
//                     }
//                 }
//                 else{
//                     fprintf(stderr, "unknown character found. Possible that wrong format given.\n"); 
//                     return -1;
//                 }
//             }
//             else{ 
//                 fprintf(stderr, "unknown character found. Possible that wrong format given.\n"); 
//                 return -1;
//             }
                
//         }
//         c = fgetc(f);
//     } 
//     if (flag == 2 || flag == 3){
//         fprintf(stderr, "given object is wrong format. check object elements' name and content.\n"); 
//         return -1;
//     }
//     return 0;
// }

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
//     }
//     else{
//         indentation = 0;
//     }

//     if (v -> type == ARGO_NO_TYPE){                 // no type
//         return -1;
//     }
//     else if (v -> type == ARGO_BASIC_TYPE){         // basic (True, False, Null)
//         if (argo_write_basic(v, f) == -1){return -1;}
//     }
//     else if (v -> type == ARGO_NUMBER_TYPE){        // number
//         if (argo_write_number(& v -> content.number, f) == -1){return -1;}
//     }
//     else if (v -> type == ARGO_STRING_TYPE){        // String
//         if (argo_write_string(& v -> content.string, f) == -1){return -1;}
//     }
//     else if (v -> type == ARGO_OBJECT_TYPE){        // object
//         if (argo_write_object(& v -> content.object, f) == -1){return -1;}
//     }
//     else if (v -> type == ARGO_ARRAY_TYPE){         // array
//         if(argo_write_array(& v -> content.array, f) == -1){return -1;}
//     }
//     else{
//         fprintf(stderr, "unknown type argo value is read.\n"); 
//         return -1;
//     }
//     if(indentation){
//         fputc(ARGO_LF,f);
//     }
//     return 0;
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
//     size_t count = 0;
//     fputc(ARGO_QUOTE,f); 
//     while (count != s -> length){
//         if ( *((s -> content) + count) == ARGO_BS){
//             fputc(ARGO_BSLASH,f);
//             fputc(ARGO_B,f);
//         }
//         else if ( *((s -> content) + count) == ARGO_FF){
//             fputc(ARGO_BSLASH,f);
//             fputc(ARGO_F,f);
//         }
//         else if ( *((s -> content) + count) == ARGO_LF){
//             fputc(ARGO_BSLASH,f);
//             fputc(ARGO_N,f);
//         }
//         else if ( *((s -> content) + count) == ARGO_CR){
//             fputc(ARGO_BSLASH,f);
//             fputc(ARGO_R,f);
//         }
//         else if ( *((s -> content) + count) == ARGO_HT){
//             fputc(ARGO_BSLASH,f);
//             fputc(ARGO_T,f);
//         }
//         else if (*((s -> content) + count) == ARGO_BSLASH){
//             fputc(ARGO_BSLASH,f);
//             fputc(ARGO_BSLASH,f);
//         }
//         else if (*((s -> content) + count) == ARGO_QUOTE){
//             fputc(ARGO_BSLASH, f);
//             fputc(ARGO_QUOTE, f);
//         }
//         else if ( 0x001F < *((s -> content) + count) && *((s -> content) + count) <=  0x00FF){
//             fputc(*((s -> content) + count), f);
//         }
//         else{
//             fputc(ARGO_BSLASH, f);
//             fputc(ARGO_U, f);
//             int number = *((s -> content) + count);
//             int divisor = 4096;
//             while(divisor != 0){
//                 if (number >= divisor){
//                     int dividend = number/divisor;
//                     intToHex(dividend, f);
//                     number -= divisor * dividend;
//                 }
//                 else{
//                     fputc('0',f);
//                 }
//                 divisor /= 16;
//             }
//         }
//         count ++;
//     }
//     fputc(ARGO_QUOTE, f);
//     return 0;
// }


// /**
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
//  */
// int argo_write_number(ARGO_NUMBER *n, FILE *f) {
//     if (n -> valid_int){
//         long number = n -> int_value; 
//         int isNegative = 0;
//         if (number < 0){
//             isNegative = 1;
//             number *= -1;
//         }
//         if (number == 0){
//             fputc(ARGO_DIGIT0, f);
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
//             if (isNegative){
//                 fputc('-',f);
//             }
//             counter = ARGO_MAX_DIGITS - 1;
//             int flag = 0;
//             while(counter >= 0){
//                 if(*(argo_digits + counter) != '0'){
//                     flag = 1;
//                 }
//                 if (flag){
//                     fputc(*(argo_digits + counter),f);
//                 }
//                 counter -= 1;
//             }
//         }
//     }
//     else if (n -> valid_float){         /// consider 0 and 0.0 
//         double number = n -> float_value;
//         int exponent = 0;
//         int isNegative = 0;
//         if (number < 0){
//             isNegative = 1;
//             number *= -1;
//         }
//         if (number == 0){
//             fputc(ARGO_DIGIT0, f);
//             fputc(ARGO_PERIOD, f);
//             fputc(ARGO_DIGIT0, f);
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
//             int counter = 1;
//             if (isNegative){
//                 fputc('-',f);
//             }
//             fputc('0', f);
//             fputc(ARGO_PERIOD, f);
//             while(counter <= ARGO_PRECISION + 1 && number != 0){
//                 number *= 10;
//                 int place = (int)number ;
//                 fputc(place + '0', f);
//                 number -= place;
//                 counter += 1;
//             }
//             if (exponent != 0){
//                 fputc(ARGO_E, f);
//                 if (exponent < 0){
//                     fputc(ARGO_MINUS, f);
//                     exponent *= -1;
//                 }
//                 counter = 0; 
//                 while (counter != ARGO_MAX_DIGITS){
//                     *(argo_digits + counter) = (exponent % 10) + '0';
//                     exponent /= 10;
//                     counter += 1;
//                 }
//                 if (exponent != 0){
//                     fprintf(stderr, "only maximum %d digits exponents allowed \n", ARGO_MAX_DIGITS);                   /// good? 
//                     return -1;
//                 }
//                 counter = ARGO_MAX_DIGITS - 1;
//                 int flag = 0;
//                 while(counter >= 0){
//                     if(*(argo_digits + counter) != '0'){
//                         flag = 1;
//                     }
//                     if (flag){
//                         fputc(*(argo_digits + counter), f);
//                     }
//                     counter -= 1;
//                 }
//             }
//         }
//     }
//     else{
//         fprintf(stderr, "number is neither valid float nor valid int.\n"); 
//         return -1;
//     }
//     return 0;
// }

// int argo_write_basic(ARGO_VALUE *v, FILE *f) {
//     if(v -> content.basic == ARGO_NULL){
//         fputs(ARGO_NULL_TOKEN, f);
//     }
//     else if(v -> content.basic == ARGO_TRUE){
//         fputs(ARGO_TRUE_TOKEN, f);
//     }
//     else if(v -> content.basic == ARGO_FALSE){
//         fputs(ARGO_FALSE_TOKEN, f);
//     }
//     else{
//         return -1;
//     }
//     return 0;
// }

// int argo_write_object(ARGO_OBJECT *o, FILE *f) {
//     fputc(ARGO_LBRACE, f);
//     if(indentation){
//         fputc(ARGO_LF, f);
//     }
//     indent_level += 1;
//     ARGO_VALUE *node = o -> member_list -> next;
//     while (node -> type != ARGO_NO_TYPE){
//         if (indentation){
//             spaceGenerator(indentation * indent_level, f);
//         }
//         if (argo_write_string(& node -> name, f) == -1){ return -1;}
//         fputc(ARGO_COLON, f);
//         if(indentation){
//             fputc(ARGO_SPACE, f);
//         }        
//         if (node -> type == ARGO_BASIC_TYPE){         // basic (True, False, Null)
//             if (argo_write_basic(node, f) == -1){return -1;}
//         }
//         else if (node -> type == ARGO_NUMBER_TYPE){        // number
//             if (argo_write_number(& node -> content.number, f) == -1){return -1;}
//         }
//         else if (node -> type == ARGO_STRING_TYPE){        // String
//             if (argo_write_string(& node -> content.string, f) == -1){return -1;}
//         }
//         else if (node -> type == ARGO_OBJECT_TYPE){        // object
//             if (argo_write_object(& node -> content.object, f) == -1){return -1;}
//         }
//         else if (node -> type == ARGO_ARRAY_TYPE){         // array
//             if (argo_write_array(& node -> content.array, f) == -1){return -1;}
//         }
//         node = node -> next;
//         if (node -> type != ARGO_NO_TYPE){
//             fputc(ARGO_COMMA, f); 
//         }
//         if(indentation){
//             fputc(ARGO_LF, f);
//         }
//     }
//     indent_level -= 1;
//     if (indentation){
//         spaceGenerator(indentation * indent_level, f);
//     }
//     fputc(ARGO_RBRACE, f);
//     return 0;
// }

// int argo_write_array(ARGO_ARRAY *a, FILE *f) {
//     fputc(ARGO_LBRACK, f);
//     if(indentation){
//         fputc(ARGO_LF, f);
//     }
//     indent_level += 1;
//     ARGO_VALUE *node = a -> element_list -> next;
//     while (node -> type != ARGO_NO_TYPE){
//         if (indentation){
//             spaceGenerator(indentation * indent_level, f);
//         }
//         if (node -> type == ARGO_BASIC_TYPE){         // basic (True, False, Null)
//             if (argo_write_basic(node, f) == -1){return -1;}
//         }
//         else if (node -> type == ARGO_NUMBER_TYPE){        // number
//             if (argo_write_number(& node -> content.number, f) == -1){return -1;}
//         }
//         else if (node -> type == ARGO_STRING_TYPE){        // String
//             if (argo_write_string(& node -> content.string, f) == -1){return -1;}
//         }
//         else if (node -> type == ARGO_OBJECT_TYPE){        // object
//             if (argo_write_object(& node -> content.object, f) == -1){return -1;}
//         }
//         else if (node -> type == ARGO_ARRAY_TYPE){         // array
//             if (argo_write_array(& node -> content.array,f) == -1){return -1;}
//         }
//         node = node -> next;
//         if (node -> type != ARGO_NO_TYPE){
//             fputc(ARGO_COMMA, f); 
//         }
//         if(indentation){
//             fputc(ARGO_LF, f);
//         }
//     }
//     indent_level -= 1;
//     if (indentation){
//         spaceGenerator(indentation * indent_level, f);
//     }
//     fputc(ARGO_RBRACK, f);
//     return 0;
// }

// int intToHex(int value, FILE *f){
//     if (value < 10){
//         fputc(value + '0', f);
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

// int spaceGenerator(long spaceRequired, FILE *f){
//     long counter = 0;
//     while (counter < spaceRequired){
//         fputc(ARGO_SPACE, f);
//         counter += 1;
//     }
//     return 0;
// }

// int hexToInt(char c){
//     if (argo_is_digit(c)){
//         return c - '0';
//     }
//     else if (c == 'A' || c == 'a'){return 10;}
//     else if (c == 'B' || c == 'b'){return 11;}
//     else if (c == 'C' || c == 'c'){return 12;}
//     else if (c == 'D' || c == 'd'){return 13;}
//     else if (c == 'E' || c == 'e'){return 14;}
//     else if (c == 'F' || c == 'f'){return 15;}
//     else {return -1;}
// }
