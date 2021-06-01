/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */ 

#ifndef __uac_comm_h
#define __uac_comm_h

#include "declare.h"

/* comment */
extern INT  comm_cpr_size;
extern UCHAR *comm;
void comment_out(const CHAR *top);

/* crc */
#define CRC_MASK 0xFFFFFFFFL
#define CRCPOLY  0xEDB88320L
extern ULONG crctable[256];
extern ULONG rd_crc;
ULONG getcrc(ULONG crc, UCHAR * addr, INT len);
void  make_crctable(void);

/* uac_dcpr.c - decompression */
INT  calc_dectabs(void);
void dcpr_comm(INT comm_size);
INT  read_wd(UINT maxwd, USHORT * code, UCHAR * wd, INT max_el);
void dcpr_init(void);
INT  dcpr_adds_blk(CHAR * buf, UINT len);
void dcpr_init_file(void);

/* uac_crt.c - file creation */
INT  wrask(const CHAR * s);
void ace_fname (CHAR * out_s, thead * head, INT nopath, unsigned int size, INT extract);
FILE * create_dest_file (CHAR * file, INT a);

#endif /* __uac_comm_h */

