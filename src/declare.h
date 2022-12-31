/******************************************************/
/*                                                    */
/* declare.h: declaration-symbols (UCHAR, ULONG, ...) */
/*                                                    */
/******************************************************/
/*  ML - 01/2004: changed licence to GPL              */
/*----------------------------------------------------*/

#ifndef __declare_h
#define __declare_h

#include "w_endian.h"
#include <stdint.h>

typedef uint16_t       USHORT;
typedef int16_t        SHORT ;
typedef uint16_t       UWORD ;
typedef int16_t        WORD  ;
typedef uint32_t       ULONG ;
typedef int32_t        LONG  ;

typedef unsigned char  UCHAR ;
typedef char           CHAR  ;
typedef unsigned       UINT  ;
typedef int            INT   ;


#if defined(_WIN32)
  #define DIRSEP '\\'
#else
  #define UNIX 1
  #define DIRSEP '/'
#endif


/* GENERIC: Convert to LONG or WORD from BYTE-Pointer-to-LOHI-byte-order data,
 *          without worrying if the bytes are word alined in memory.
 *  p is a pointer to char.
 */

#ifdef WORDS_BIGENDIAN

#define WORDswap(n)  (*(n) = (*(n) << 8) | (*(n) >> 8))
#define LONGswap(n)  ( WORDswap(&((WORD *)(n))[0]),\
                       WORDswap(&((WORD *)(n))[1]),\
                       *(n) = (*(n) >> 16) | (*(n) << 16) )
#define BUFP2WORD(p) ((UWORD)*(p)++ | ((*(p)++)<<8))
#define BUFP2LONG(p) ((ULONG)*(p)++ | ((*(p)++)<<8) | ((*(p)++)<<16) | ((*(p)++)<<24))
#define BUF2WORD(p) ((UWORD)*(p) | (*((p)+1)<<8))
#define BUF2LONG(p) ((ULONG)*(p) | (*((p)+1)<<8) | (*((p)+2)<<16) | (*((p)+3)<<24))

#else /* little endian */

#define BUFP2WORD(p) *(UWORD*)((p+=2)-2)
#define BUFP2LONG(p) *(ULONG*)((p+=4)-4)
#define BUF2WORD(p) (*(UWORD*)p)
#define BUF2LONG(p) (*(ULONG*)p)

#endif /* WORDS_BIGENDIAN */

#endif /* __declare_h */

