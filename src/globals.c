/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Global variable declarations                                        */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */

#include "acestruc.h"
#include "unace.h"
#include <stdio.h>

//-------- Ace sign
const char *acesign = "**ACE**";

//-------- header buffer and pointers
thead head;

tmhead *t_mhead = (tmhead *) & head;
tfhead *t_fhead = (tfhead *) & head;

//-------- buffers
ULONG *buf_rd        =0;
CHAR  *buf           =0;
CHAR  *buf_wr        =0;
UCHAR *readbuf       =0;

//-------- decompressor variables
SHORT rpos           =0,
      dcpr_do        =0,
      dcpr_do_max    =0,
      blocksize      =0,
      dcpr_dic       =0,
      dcpr_oldnum    =0,
      bits_rd        =0,
      dcpr_frst_file =0;
USHORT dcpr_code_mn[1 << maxwd_mn],
       dcpr_code_lg[1 << maxwd_lg];
UCHAR dcpr_wd_mn[maxcode + 2],
      dcpr_wd_lg[maxcode + 2],
      wd_svwd[svwd_cnt];
ULONG dcpr_dpos      =0,
      cpr_dpos2      =0,
      dcpr_dicsiz    =0,
      dcpr_dican     =0,
      dcpr_size      =0,
      dcpr_olddist[4]={0,0,0,0},
      code_rd        =0;

CHAR *dcpr_text      =0;

//-------- quicksort
USHORT sort_org[maxcode + 2];
UCHAR sort_freq[(maxcode + 2) * 2];

//-------- file handling
CHAR aname[PATH_MAX];
FILE * archive_fp = NULL;
FILE * outfile_fp = NULL;
LONG skipsize=0;

//-------- structures for archive handling
struct tadat adat;

//-------- flags
INT  f_err      =0,
     f_err_crc  =0,
     f_ovrall   =0,
     f_ovrnvr   =0,
     f_curpas   =0,
     f_criterr  =0;

