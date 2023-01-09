/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */  

#ifndef __UNACE_H__
#define __UNACE_H__

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wintypes.h"
#include "w_endian.h"

#if defined(_WIN32)
  #define DIRSEP '\\'
#else
  #define UNIX 1
  #define DIRSEP '/'
#endif

// The ACE format is Little Endian
#ifdef WORDS_BIGENDIAN /* big endian */
#define BUFP2WORD(p) (bswap16(*(USHORT*) ((p+=2)-2)))
#define BUFP2LONG(p) (bswap32(*(ULONG*) ((p+=4)-4)))
#define BUF2WORD(p) (bswap16(*(USHORT*)(p)))
#define BUF2LONG(p) (bswap32(*(ULONG*)(p)))
#else /* little endian */
#define BUFP2WORD(p) (*(USHORT*) ((p+=2)-2))
#define BUFP2LONG(p) (*(ULONG*) ((p+=4)-4))
#define BUF2WORD(p) (*(USHORT*)p)
#define BUF2LONG(p) (*(ULONG*)p)
#endif /* WORDS_BIGENDIAN */

#include "acestruc.h"

//--------- functions
INT read_adds_blk(CHAR * buffer, INT len);

//--------- buffers: increase sizes when possible to speed up the program
#define size_rdb  2048
#define size_wrb  4096
#define size_buf  2048
#define size_headrdb (sizeof(head)-20) // (some bytes less esp. for Amiga)

//--------- (de-)compressor constants
#define maxdic      22
#define maxwd_mn    11
#define maxwd_lg    11
#define maxwd_svwd   7
#define maxlength  259
#define maxdis2    255
#define maxdis3   8191
#define maxcode   (255+4+maxdic)
#define svwd_cnt    15
#define max_cd_mn (256+4+(maxdic+1)-1)
#define max_cd_lg (256-1)

//--------- flags
#define ERR_MEM      1
#define ERR_FILES    2
#define ERR_FOUND    3
#define ERR_FULL     4
#define ERR_OPEN     5
#define ERR_READ     6
#define ERR_WRITE    7
#define ERR_CLINE    8
#define ERR_CRC      9
#define ERR_OTHER   10
#define ERR_USER   255

//--------- system things
#define flush fflush(stdout);
#define in(v1,v2,v3) ((((long)v1)>=(v2)) && (((long)v1)<=(v3)))
#define delay_len 500

//-------- file creation
#define fileexists(name) (!access(name, 0))

/* Timestamp macros */
#define get_tx(m,d,h,n) (((ULONG)m<<21)+((ULONG)d<<16)+((ULONG)h<<11)+(n<<5))
#define get_tstamp(y,m,d,h,n,s) ((((ULONG)(y-1980))<<25)+get_tx(m,d,h,n)+(s/2))
#define ts_year(ts)  ((UINT)((ts >> 25) & 0x7f) + 1980)
#define ts_month(ts) ((UINT)(ts >> 21) & 0x0f)      // 1..12 means Jan..Dec
#define ts_day(ts)   ((UINT)(ts >> 16) & 0x1f)      // 1..31 means 1st..31st
#define ts_hour(ts)  ((UINT)(ts >> 11) & 0x1f)
#define ts_min(ts)   ((UINT)(ts >> 5) & 0x3f)
#define ts_sec(ts)   ((UINT)((ts & 0x1f) * 2))


/* ------------------------------------------------------------------------ */
/*      Global variable definitions                                         */
/* ------------------------------------------------------------------------ */

//-------- header buffer and pointers
extern thead head;
extern tmhead *t_mhead;
extern tfhead *t_fhead;

//-------- buffers
extern ULONG *buf_rd ;
extern CHAR  *buf    ;
extern CHAR  *buf_wr ;
extern UCHAR *readbuf;

//-------- decompressor variables
extern SHORT rpos          ;
extern SHORT dcpr_do       ;
extern SHORT dcpr_do_max   ;
extern SHORT blocksize     ;
extern SHORT dcpr_dic      ;
extern SHORT dcpr_oldnum   ;
extern SHORT bits_rd       ;
extern SHORT dcpr_frst_file;

extern USHORT dcpr_code_mn[1 << maxwd_mn];
extern USHORT dcpr_code_lg[1 << maxwd_lg];
extern UCHAR dcpr_wd_mn[maxcode + 2];
extern UCHAR dcpr_wd_lg[maxcode + 2];
extern UCHAR wd_svwd[svwd_cnt];

extern ULONG dcpr_dpos      ;
extern ULONG cpr_dpos2      ;
extern ULONG dcpr_dicsiz    ;
extern ULONG dcpr_dican     ;
extern ULONG dcpr_size      ;
extern ULONG dcpr_olddist[4];
extern ULONG code_rd        ;

extern CHAR *dcpr_text      ;

//-------- quicksort
extern USHORT sort_org[maxcode + 2];
extern UCHAR sort_freq[(maxcode + 2) * 2];

//-------- file handling
extern CHAR aname[PATH_MAX];
extern FILE * archive_fp;
extern FILE * outfile_fp;
extern LONG skipsize;

//-------- structures for archive handling
extern struct tadat adat;

//-------- flags
extern INT f_err     ;
extern INT f_err_crc ;
extern INT f_ovrall  ;
extern INT f_ovrnvr  ;
extern INT f_curpas  ;
extern INT f_criterr ;


/* ------------------------------------------------------------------------ */
/*                uac.c / uac_dcpr.c                                        */
/* ------------------------------------------------------------------------ */

/* comment */
extern INT  comm_cpr_size;
extern UCHAR *comm;
void comment_out(const CHAR *top);

/* crc */
#define CRC_MASK 0xFFFFFFFFL
extern ULONG rd_crc;
ULONG getcrc(ULONG crc, UCHAR * addr, INT len);
void  make_crctable(void);

/* uac_dcpr.c - decompression */
void dcpr_comm(INT comm_size);
void dcpr_init(void);
INT  dcpr_adds_blk(CHAR * buf, UINT len);
void dcpr_init_file(void);

/* file creation */
INT  wrask(const CHAR * s);
void ace_fname (CHAR * out_s, thead * head, INT nopath, unsigned int size, INT extract);
FILE * create_dest_file (CHAR * file, INT a);


#endif /* __UNACE_H__ */

