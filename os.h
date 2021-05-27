/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */

#ifndef __os_h
#define __os_h

#if defined(_WIN32)
  #define DIRSEP '\\'
  #define LO_HI_BYTE_ORDER
#endif

#if defined(UNIX)
  #define DIRSEP '/'
  #define LO_HI_BYTE_ORDER
#endif

#endif /* __os_h */
