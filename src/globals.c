/* ------------------------------------------------------------------------ */
/*      Global variable declarations                                        */
/*      GPL2                                                                */
/* ------------------------------------------------------------------------ */

#include "acestruc.h"
#include "unace.h"
#include <stdio.h>

//-------- header buffer and pointers
thead head;
tmhead *t_mhead = (tmhead *) & head;
tfhead *t_fhead = (tfhead *) & head;

//-------- buffers
ULONG *buf_rd  = 0;
CHAR  *buf     = 0;
CHAR  *buf_wr  = 0;
UCHAR *readbuf = 0;

//-------- decompressor variables
SHORT rpos           = 0;
SHORT dcpr_do        = 0;
SHORT dcpr_do_max    = 0;
SHORT blocksize      = 0;
SHORT dcpr_dic       = 0;
SHORT dcpr_oldnum    = 0;
SHORT bits_rd        = 0;
SHORT dcpr_frst_file = 0;

USHORT dcpr_code_mn[1 << maxwd_mn];
USHORT dcpr_code_lg[1 << maxwd_lg];
UCHAR dcpr_wd_mn[maxcode + 2];
UCHAR dcpr_wd_lg[maxcode + 2];
UCHAR wd_svwd[svwd_cnt];

ULONG dcpr_dpos      = 0;
ULONG cpr_dpos2      = 0;
ULONG dcpr_dicsiz    = 0;
ULONG dcpr_dican     = 0;
ULONG dcpr_size      = 0;
ULONG dcpr_olddist[4]= {0,0,0,0};
ULONG code_rd        = 0;

CHAR *dcpr_text      = 0;

//-------- quicksort
USHORT sort_org[maxcode + 2];
UCHAR sort_freq[(maxcode + 2) * 2];

//-------- file handling
CHAR aname[PATH_MAX];
FILE * archive_fp = NULL;
FILE * outfile_fp = NULL;
LONG skipsize= 0;

//-------- structures for archive handling
struct tadat adat;

//-------- flags
INT f_err      = 0;
INT f_err_crc  = 0;
INT f_ovrall   = 0;
INT f_ovrnvr   = 0;
INT f_curpas   = 0;
INT f_criterr  = 0;

