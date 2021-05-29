/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */ 

#ifndef __uac_comm_h
#define __uac_comm_h


#include "declare.h"

/* comment */
extern INT  comm_cpr_size;
extern UCHAR *comm;
void comment_out(CHAR *top);

/* crc */
#define CRC_MASK 0xFFFFFFFFL
#define CRCPOLY  0xEDB88320L
extern ULONG crctable[256];
extern ULONG rd_crc;
ULONG getcrc(ULONG crc, UCHAR * addr, INT len);
void  make_crctable(void);

/* decompression */
INT  calc_dectabs(void);
void dcpr_comm(INT comm_size);
INT  read_wd(UINT maxwd, USHORT * code, UCHAR * wd, INT max_el);
void dcpr_init(void);
INT  dcpr_adds_blk(CHAR * buf, UINT len);
void dcpr_init_file(void);

/* basic things */
void memset16(USHORT * dest, SHORT val, INT len);
INT  cancel(void);
INT  wrask(CHAR * s);
void set_handler(void);
#if defined(UNIX)
  #define getch() getchar()
#endif /* UNIX */


#endif /* __uac_comm_h */

