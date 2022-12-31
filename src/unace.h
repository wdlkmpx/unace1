/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */  

#ifndef __unace_h
#define __unace_h

#include "declare.h"

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


#endif /* __unace_h */

