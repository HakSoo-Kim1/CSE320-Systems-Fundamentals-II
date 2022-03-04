/*********************/
/* par.c             */
/* for Par 3.20      */
/* Copyright 1993 by */
/* Adam M. Costello  */
/*********************/

/* This is ANSI C code. */


#include "errmsg.h"
#include "buffer.h"    /* Also includes <stddef.h>. */
#include "reformat.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

#undef NULL
#define NULL ((void *) 0)

static int versionFlag = 0;

static void custom_parseopt(  
  int argc,  const char *const *argv, int *pwidth, int *pprefix,
  int *psuffix, int *phang, int *plast, int *pmin
);

const char * const progname = "par";
const char * const version = "3.20";



static int digtoint(char c)

/* Returns the value represented by the digit c,   */
/* or -1 if c is not a digit. Does not use errmsg. */
{
  return c == '0' ? 0 :
         c == '1' ? 1 :
         c == '2' ? 2 :
         c == '3' ? 3 :
         c == '4' ? 4 :
         c == '5' ? 5 :
         c == '6' ? 6 :
         c == '7' ? 7 :
         c == '8' ? 8 :
         c == '9' ? 9 :
         -1;

  /* We can't simply return c - '0' because this is ANSI  */
  /* C code, so it has to work for any character set, not */
  /* just ones which put the digits together in order.    */
}


static int strtoudec(const char *s, int *pn)

/* Puts the decimal value of the string s into *pn, returning */
/* 1 on success. If s is empty, or contains non-digits,       */
/* or represents an integer greater than 9999, then *pn       */
/* is not changed and 0 is returned. Does not use errmsg.     */
{
  int n = 0;

  if (!*s) return 0;

  do {
    if (n >= 1000 || !isdigit(*s)) return 0;
    n = 10 * n + digtoint(*s);
  } while (*++s);

  *pn = n;

  return 1;
}


// static void parseopt(
//   const char *opt, int *pwidth, int *pprefix,
//   int *psuffix, int *phang, int *plast, int *pmin
// )
// /* Parses the single option in opt, setting *pwidth, *pprefix,     */
// /* *psuffix, *phang, *plast, or *pmin as appropriate. Uses errmsg. */
// {
//   const char *saveopt = opt;
//   char oc;
//   int n, r;

//   if (*opt == '-') ++opt;

//   if (!strcmp(opt, "version")) {
//     // sprintf(errmsg, "%s %s\n", progname, version);
//     set_error("par 3.20\n");
//     return;
//   }

//   oc = *opt;

//   if (isdigit(oc)) {
//     if (!strtoudec(opt, &n)) goto badopt;
//     if (n <= 8) *pprefix = n;
//     else *pwidth = n;
//   }
//   else {
//     if (!oc) goto badopt;
//     n = 1;
//     r = strtoudec(opt + 1, &n);
//     if (opt[1] && !r) goto badopt;

//     if (oc == 'w' || oc == 'p' || oc == 's') {
//       if (!r) goto badopt;
//       if      (oc == 'w') *pwidth  = n;
//       else if (oc == 'p') *pprefix = n;
//       else                *psuffix = n;
//     }
//     else if (oc == 'h') *phang = n;
//     else if (n <= 1) {
//       if      (oc == 'l') *plast = n;
//       else if (oc == 'm') *pmin = n;
//     }
//     else goto badopt;
//   }

//   // *errmsg = '\0';
//   return;

// badopt:
//   set_error("Bad Option");
//   // sprintf(errmsg, "Bad option: %.149s\n", saveopt);
// }


static char **readlines(void)

/* Reads lines from stdin until EOF, or until a blank line is encountered, */
/* in which case the newline is pushed back onto the input stream. Returns */
/* a NULL-terminated array of pointers to individual lines, stripped of    */
/* their newline characters. Uses errmsg, and returns NULL on failure.     */
{
  struct buffer *cbuf = NULL, *pbuf = NULL;
  int c, blank;
  char ch, *ln = NULL, *nullline = NULL, nullchar = '\0', **lines = NULL;
  debug("HERE");

  cbuf = newbuffer(sizeof (char));
    debug("here");

  if (is_error()) goto rlcleanup;
  pbuf = newbuffer(sizeof (char *));
  if (is_error()) goto rlcleanup;
  debug("inside of readlines");
  for (blank = 1;  ; ) {
    c = getchar();
    // debug(" character read is %d",c);
    if (c == EOF) break;
    if (c == '\n') {
      if (blank) {
        ungetc(c,stdin);
        break;
      }
      additem(cbuf, &nullchar);
      if (is_error()) goto rlcleanup;
      ln = copyitems(cbuf);
      if (is_error()) goto rlcleanup;
      additem(pbuf, &ln);
      if (is_error()) goto rlcleanup;
      clearbuffer(cbuf);
      blank = 1;
    }
    else {
      if (!isspace(c)) blank = 0;
      ch = c;
      additem(cbuf, &ch);
      if (is_error()) goto rlcleanup;
    }
  }
  if (!blank) {
    additem(cbuf, &nullchar);
    if (is_error()) goto rlcleanup;
    ln = copyitems(cbuf);
    if (is_error()) goto rlcleanup;
    additem(pbuf, &ln);
    if (is_error()) goto rlcleanup;
  }

  additem(pbuf, &nullline);
  if (is_error()) goto rlcleanup;
  lines = copyitems(pbuf);
  debug("lines in readlines is : %s",*lines);
rlcleanup:

  if (cbuf) freebuffer(cbuf);
  if (pbuf) {
    if (!lines)
      for (;;) {
        lines = nextitem(pbuf);
        if (!lines) break;
        free(*lines);
      }
    freebuffer(pbuf);
  }

  return lines;
}


static void setdefaults(
  const char * const *inlines, int *pwidth, int *pprefix,
  int *psuffix, int *phang, int *plast, int *pmin
)
/* If any of *pwidth, *pprefix, *psuffix, *phang, *plast, *pmin are     */
/* less than 0, sets them to default values based on inlines, according */
/* to "par.doc". Does not use errmsg because it always succeeds.        */
{
  int numlines;
  const char *start, *end, * const *line, *p1, *p2;

  if (*pwidth < 0) *pwidth = 72;
  if (*phang < 0) *phang = 0;
  if (*plast < 0) *plast = 0;
  if (*pmin < 0) *pmin = *plast;

  for (line = inlines;  *line;  ++line);
  numlines = line - inlines;

  if (*pprefix < 0){
    if (numlines <= *phang + 1)
      *pprefix = 0;
    else {
      start = inlines[*phang];
      for (end = start;  *end;  ++end);
      for (line = inlines + *phang + 1;  *line;  ++line) {
        for (p1 = start, p2 = *line;  p1 < end && *p1 == *p2;  ++p1, ++p2);
        end = p1;
      }
      *pprefix = end - start;
    }
  }
  debug("prefix is : %d",*pprefix);
  if (*psuffix < 0){
    if (numlines <= 1)
      *psuffix = 0;
    else {
      start = *inlines;
      for (end = start;  *end;  ++end);
      for (line = inlines + 1;  *line;  ++line) {
        for (p2 = *line;  *p2;  ++p2);
        for (p1 = end;
             p1 > start && p2 > *line && p1[-1] == p2[-1];
             --p1, --p2);
        start = p1;
      }
      while (end - start >= 2 && isspace(*start) && isspace(start[1])) ++start;
      *psuffix = end - start;
      debug("sufix is : %d",*psuffix);
    }
  }
}



static void freelines(char **lines)
/* Frees the strings pointed to in the NULL-terminated array lines, then */
/* frees the array. Does not use errmsg because it always succeeds.      */
{

  for (char **line = lines; *line;  ++line){
    free(*line);
  }
  free(lines);
}


int original_main(int argc, const char * const *argv)
{
  int width, widthbak = -1, prefix, prefixbak = -1, suffix, suffixbak = -1,
      hang, hangbak = -1, last, lastbak = -1, min, minbak = -1, c;
  char *parinit = NULL, *picopy = NULL, *opt = NULL, **inlines = NULL, **outlines = NULL,
       **line = NULL;
  char ** custom_argv = NULL;

  const char * const whitechars = " \f\n\r\t\v";

  parinit = getenv("PARINIT");
  debug("parinit string is : %s",parinit);
  if (parinit) {
    picopy = malloc((strlen(parinit) + 1) * sizeof (char));
    if (!picopy) {
      set_error("Out of memory.\n");
      goto parcleanup;
    }
    strcpy(picopy,parinit);
    opt = strtok(picopy,whitechars);
    custom_argv = malloc( sizeof( char * ) );
    if (!custom_argv){
      set_error("Out of memory.\n");
      // strcpy(errmsg,outofmem);
      goto parcleanup;
    }
    int counter = 1;
    while (opt) {
      custom_argv = realloc(custom_argv,(counter + 1) * sizeof(char *));
      ++counter;
      custom_argv[counter - 2] = malloc(strlen(opt) + 1);
      strcpy(custom_argv[counter - 2],opt);
      opt = strtok(NULL, whitechars);
    }
    custom_argv[counter - 1] = NULL;
    for ( char **p = custom_argv; *p; ++p ){
      debug("%s",*p);
    }
    custom_parseopt(argc, (const char *const *)custom_argv, &widthbak, &prefixbak,
             &suffixbak, &hangbak, &lastbak, &minbak);
    if (is_error()) goto parcleanup;
    for ( char **p = custom_argv; *p; ++p ){
      free(*p);
    }
    free(custom_argv);
    free(picopy);
    custom_argv = NULL;
    picopy = NULL;
  }
  debug("\t PARINIT result : ");
  debug("given width : %d",widthbak);
  debug("given hang : %d",hangbak);
  debug("given last : %d",lastbak);
  debug("given min : %d",minbak);
  debug("given prefix : %d",prefixbak);
  debug("given suffix : %d",suffixbak);

  custom_parseopt(argc, argv, &widthbak, &prefixbak,
             &suffixbak, &hangbak, &lastbak, &minbak);
  if (is_error()) goto parcleanup;

  debug("\t terminal argument result : ");
  debug("given width : %d",widthbak);
  debug("given hang : %d",hangbak);
  debug("given last : %d",lastbak);
  debug("given min : %d",minbak);
  debug("given prefix : %d",prefixbak);
  debug("given suffix : %d",suffixbak);

  for (;;) {
    for (;;) {
      c = getchar();
      if (c == EOF) break;
      if (c != '\n') break;
      putchar(c);
    }
    if(c == EOF) break;
    ungetc(c,stdin);
    debug("READING");
    inlines = readlines();
    debug("DONE READING");
    if (is_error()) goto parcleanup;
    if (!*inlines) {
      free(inlines);
      inlines = NULL;
      continue;
    }

    width = widthbak;  prefix = prefixbak;  suffix = suffixbak;
    hang = hangbak;  last = lastbak;  min = minbak;
    setdefaults((const char * const *) inlines,
                &width, &prefix, &suffix, &hang, &last, &min);
    debug("BEFORE sREFORMAT");
    if (width <= prefix + suffix){
      set_error("width size is smaller than prefix + suffix\n");
      goto parcleanup;
    }
    outlines = reformat((const char * const *) inlines,
                        width, prefix, suffix, hang, last, min);
    debug("GOT here");
    if (is_error()) goto parcleanup;
    debug("AFTER REFORMAT");

    freelines(inlines);
    inlines = NULL;

    for (line = outlines;  *line;  ++line)
      puts(*line);

    freelines(outlines);
    outlines = NULL;
  }

parcleanup:
  if (custom_argv){
    for ( char **p = custom_argv; *p; ++p ){
      debug("FREEING");
      free(*p);
    }
    free(custom_argv);
    debug("good");
  }
  if (picopy) free(picopy);
  if (inlines) freelines(inlines);
  if (outlines) freelines(outlines);
  if (is_error()) {
    report_error(stderr);
    if (versionFlag){
      debug("verstion");
      clear_error();
      exit(EXIT_SUCCESS);
    }
    debug("NOT version");
    clear_error();
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

static void custom_parseopt(  
  int argc, const char * const *argv, int *pwidth, int *pprefix,
  int *psuffix, int *phang, int *plast, int *pmin
)
{
  int index = 0;
  int n, r = -1;
  int option = -1;
  struct option options[] = {
    {"version", no_argument ,       NULL, 'v' },
    {"width",   required_argument,  NULL, 'w' },
    {"prefix",  required_argument,  NULL, 'p' },
    {"suffix",  required_argument,  NULL, 's' },
    {"hang",    required_argument,  NULL, 'h' },
    {"last",    no_argument,        NULL, 0 },
    {"no-last", no_argument,        NULL, 0 },
    {"min",    no_argument,         NULL, 0 },
    {"no-min",  no_argument,        NULL, 0 },
    {NULL,0,NULL, 0}
  };

  while ((option = getopt_long(argc, (char * const*)argv, "-:w:p:s:h:l:m:", options, &index)) != -1){
    switch(option){
      case 'v':
      debug("\t version given ");
      set_error("par 3.20\n");
      versionFlag = 1;
      return;
      break;

      case 'w':
      debug("\t width given ");
      r = strtoudec(optarg, &n);
      if (!r){ set_error("Bad width\n"); return;}
      *pwidth  = n;
      break;

      case 'p':
      debug("\t prefix given ");
      r = strtoudec(optarg, &n);
      if (!r){ set_error("Bad prefix\n"); return;}
      *pprefix  = n;
      break;

      case 's':
      debug("\t suffix given ");
      r = strtoudec(optarg, &n);
      if (!r){ set_error("Bad suffix\n"); return;}
      *psuffix  = n;
      break;

      case 'h':
      debug("\t hang given ");
      r = strtoudec(optarg, &n);
      if (!r){ set_error("Bad hang\n"); return;}
      *phang  = n;
      break;

      case 'l':
      debug("\t last given ");
      r = strtoudec(optarg, &n);
      if (!r || n > 1){ set_error("Bad last\n"); return;}
      *plast  = n;
      break;

      case 'm':
      debug("\t min given ");
      debug("%s",optarg);
      r = strtoudec(optarg, &n);
      if (!r || n > 1){ set_error("Bad min\n"); return;}
      *pmin  = n;
      break;

      case 0:
        if (strcmp(options[index].name, "last") == 0){
          debug("\t last given ");
          *plast  = 1;
        }
        else if (strcmp(options[index].name, "no-last") == 0){
          debug("\t no-last given ");
          *plast  = 0;
        }
        else if (strcmp(options[index].name, "min") == 0){
          debug("\t min given ");
          *pmin  = 1;
        }
        else if (strcmp(options[index].name, "no-min") == 0){
          debug("\t no-min given ");
          *pmin  = 0;
        }
      break;

      case ':':
        if(optopt == 'h'){
          *phang = 1;
        }
        else if (optopt == 'l'){
          *plast = 1;
        }
        else if (optopt == 'm'){
          *pmin  = 1;
        }
        else{
          set_error("Wrong argument given\n");
          return;
        }
      break;

      case 1:
      case '?':
        debug("wrong %d",optopt);
        debug("opt is : %d",option);
        set_error("Wrong argument given\n");
        return;

    }
  }
  // set_error(NULL);
  // errmsg = '\0';
  return;
}