/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */  

#ifndef __uac_crt_h
#define __uac_crt_h

#include "declare.h"
#include "acestruc.h"

INT  wrask(CHAR * s);
CHAR *ace_fname(CHAR * s, thead * head, INT nopath, unsigned int size, INT extract);
FILE * create_dest_file (CHAR * file, INT a);

#ifdef UNIX
  #define mkdir(_a) mkdir(_a,  S_IRWXU | S_IRWXG | S_IRWXO)
#endif

#ifndef _A_SUBDIR
  #define _A_SUBDIR 0x10        /* MS-DOS directory constant */
#endif

#ifndef O_BINARY
  #define O_BINARY 0
#endif

#endif /* __uac_crt_h */

