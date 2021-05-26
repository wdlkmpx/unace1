/******************************************************/
/*                                                    */
/* declare.h: declaration-symbols (UCHAR, ULONG, ...) */
/*                                                    */
/******************************************************/
/*  ML - 01/2004: changed licence to GPL              */
/*----------------------------------------------------*/

#ifndef __declare_h
#define __declare_h

#ifdef AMIGA

#include <exec/types.h>

#else /* AMIGA */

#include <stdint.h>

typedef uint16_t       USHORT;
typedef int16_t        SHORT ;
typedef uint16_t       UWORD ;
typedef int16_t        WORD  ;
typedef uint32_t       ULONG ;
typedef int32_t        LONG  ;

#endif  /* !AMIGA */

typedef unsigned char  UCHAR ;
typedef char           CHAR  ;
typedef unsigned       UINT  ;
typedef int            INT   ;


#endif /* __declare_h */

