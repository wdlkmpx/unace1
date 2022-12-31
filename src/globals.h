/* ------------------------------------------------------------------------ */
/*      Global variable definitions                                         */
/*      GPL2                                                                */
/* ------------------------------------------------------------------------ */

#ifndef __globals_h
#define __globals_h

#include "acestruc.h"
#include "unace.h"

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


#endif /* __globals_h */

