/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Main file of public UNACE.                                          */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */

#include <config.h>

#include "unace.h"
#include <ctype.h>      // tolower()
#include <fcntl.h>      // open()
#include <sys/stat.h>   // 
#include <signal.h>     // signal()
#if defined(UNIX)
  #include <unistd.h>
  #include <errno.h>
#endif
#if defined(W_MTRACE) && defined(__GLIBC__)
  #include <mcheck.h>
#endif

#define version_msg "UNACE " VERSION "    public version\n"

/* ------------------------------------------------------------------------ */
/*      Global variable declarations                                        */
/* ------------------------------------------------------------------------ */

#include <stdio.h>
#include "unace.h"

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



//--------------- BEGIN OF UNACE ROUTINES ----------------------------------//

static INT cancel(void)               // checks whether to interrupt the program
{
   return (f_err);
}

static void my_signalhandler(INT sig_number)     // sets f_err if ctrl+c or ctrl+brk
{
   f_err = ERR_USER;
   printf("\nUser break\n");
}

void init_unace(void)           // initializes unace
{
   buf_rd = (ULONG*) malloc(size_rdb * sizeof(ULONG));  // Allocate buffers: increase
   buf    = (char*) malloc(size_buf);                  // sizes when possible to speed
   buf_wr = (char*) malloc(size_wrb);                  // up the program
   readbuf= (UCHAR*) malloc(size_headrdb);

   if (buf_rd ==NULL ||
       buf    ==NULL ||
       buf_wr ==NULL ||
       readbuf==NULL )
      f_err = ERR_MEM;

   make_crctable();             // initialize CRC table
   dcpr_init();                 // initialize decompression

   signal(SIGINT, my_signalhandler); // ctrl+break etc.
}

void done_unace(void)
{
   if (buf_rd   ) free(buf_rd   );
   if (buf      ) free(buf      );
   if (buf_wr   ) free(buf_wr   );
   if (readbuf  ) free(readbuf  );
   if (dcpr_text) free(dcpr_text);
}

INT  read_header(INT print_err)         // reads any header from archive
{
   USHORT rd,
        head_size,
        crc_ok;
   LONG crc;
   UCHAR *tp=readbuf;

   fseek (archive_fp, skipsize, SEEK_CUR);   // skip ADDSIZE block

   if (fread(&head, 1, 4, archive_fp) < 4)
      return 0;                         // read CRC and header size

#ifdef WORDS_BIGENDIAN
   head.HEAD_CRC  = bswap16(head.HEAD_CRC);
   head.HEAD_SIZE = bswap16(head.HEAD_SIZE);
#endif
                                        // read size_headrdb bytes into 
   head_size = head.HEAD_SIZE;          // header structure 
   rd = (head_size > size_headrdb) ? size_headrdb : head_size;
   if (fread(readbuf, 1, rd, archive_fp) < rd)
      return 0;
   head_size -= rd;
   crc = getcrc(CRC_MASK, readbuf, rd);

   while (head_size)                    // skip rest of header
   {                            
      rd = (head_size > size_buf) ? size_buf : head_size;
      if (fread(buf, 1, rd, archive_fp) < rd)
         return 0;
      head_size -= rd;
      crc = getcrc(crc, (UCHAR*)buf, rd);
   }

   head.HEAD_TYPE =*tp++;               // generic buffer to head conversion
   head.HEAD_FLAGS=BUFP2WORD(tp);

   if (head.HEAD_FLAGS & ACE_ADDSIZE)
      skipsize = head.ADDSIZE = BUF2LONG(tp);   // get ADDSIZE
   else
      skipsize = 0;

                                                // check header CRC 
   if (!(crc_ok = head.HEAD_CRC == (crc & 0xffff)) && print_err)
      printf("\nError: archive is broken\n");
   else
   switch (head.HEAD_TYPE)              // specific buffer to head conversion
   {
      case MAIN_BLK:
         memcpy(mhead.ACESIGN, tp, acesign_len); tp+=acesign_len;
         mhead.VER_MOD=*tp++;
         mhead.VER_CR =*tp++;
         mhead.HOST_CR=*tp++;
         mhead.VOL_NUM=*tp++;
         mhead.TIME_CR=BUFP2LONG(tp);
         mhead.RES1   =BUFP2WORD(tp);
         mhead.RES2   =BUFP2WORD(tp);
         mhead.RES    =BUFP2LONG(tp);
         mhead.AV_SIZE=*tp++;
         memcpy(mhead.AV, tp, rd-(USHORT)(tp-readbuf));
         break;
      case FILE_BLK:
         fhead.PSIZE     =BUFP2LONG(tp);
         fhead.SIZE      =BUFP2LONG(tp);
         fhead.FTIME     =BUFP2LONG(tp);
         fhead.ATTR      =BUFP2LONG(tp);
         fhead.CRC32     =BUFP2LONG(tp);
         fhead.TECH.TYPE =*tp++;
         fhead.TECH.QUAL =*tp++;
         fhead.TECH.PARM =BUFP2WORD(tp);
         fhead.RESERVED  =BUFP2WORD(tp);
         fhead.FNAME_SIZE=BUFP2WORD(tp);
         memcpy(fhead.FNAME, tp, rd-(USHORT)(tp-readbuf));
         break;
//    default: (REC_BLK and future things): 
//              do nothing 'cause isn't needed for extraction
   }

   return crc_ok;
}
                                // maximum SFX module size 
#define max_sfx_size 65536      // (needed by read_arc_head)

INT read_arc_head(void)         // searches for the archive header and reads it
{
   INT  i,
        flags,
        buf_pos = 0;
   LONG arc_head_pos,
        old_fpos,
        fpos = 0;
   struct stat st;

   fstat (fileno(archive_fp), &st);

   memset(buf, 0, size_buf);

   while (fseek (archive_fp, 0, SEEK_CUR)<st.st_size && fpos < max_sfx_size)
   {
      old_fpos = fpos;
      fpos += fread (&buf[buf_pos], 1, size_buf - buf_pos, archive_fp);

      for (i = 0; i < size_buf; i++)    // look for the acesign
      {                         
         if (!memcmp(acesign, &buf[i], acesign_len))
         {             
                                        // seek to the probable begin 
                                        // of the archive
            arc_head_pos = old_fpos + i - buf_pos -  bytes_before_acesign;
            fseek (archive_fp, arc_head_pos, SEEK_SET);
            if (read_header(0))         // try to read archive header
            {                           
               flags = mhead.HEAD_FLAGS;
               adat.sol     = (flags & ACE_SOLID) > 0;
               adat.vol     = (flags & ACE_MULT_VOL) > 0;
               adat.vol_num = mhead.VOL_NUM;
               adat.time_cr = mhead.TIME_CR;
               return 1;
            }
         }
      }
                                        // was no archive header,
                                        // continue search
      fseek (archive_fp, fpos, SEEK_SET);
      memcpy(buf, &buf[size_buf - 512], 512);
      buf_pos = 512;                    // keep 512 old bytes
   }
   return 0;
}

INT  open_archive(INT print_err)        // opens archive (or volume)
{
   CHAR av_str[80];
   unsigned int copylen;

   archive_fp = fopen (aname, "rb");   // open file

   if (archive_fp == NULL)
   {
      printf("\nError opening file %s", aname);
      return 0;
   }
   if (!read_arc_head())                        // read archive header
   {                            
      if (print_err)
         printf("\nInvalid archive file: %s\n", aname);
      fclose (archive_fp);
      return 0;
   }

   printf("\nProcessing archive: %s\n\n", aname);
   if (head.HEAD_FLAGS & ACE_AV)
   {
      printf("Authenticity Verification:");   // print the AV
      sprintf(av_str, "\ncreated on %d.%d.%d by ",
              ts_day(adat.time_cr), ts_month(adat.time_cr), ts_year(adat.time_cr));
      printf("%s", av_str);
      copylen = mhead.AV_SIZE;
      if (copylen > 79)
        copylen = 79;
      strncpy(av_str, mhead.AV, copylen);
      av_str[copylen] = 0;
      printf("%s\n\n", av_str);
   }
   comment_out("Main comment:");        // print main comment
   return 1;
}


INT  proc_next_vol(void)        // opens next volume to process
{
   fclose (archive_fp);               // close handle

   // get file name of next volume
   CHAR *cp = strrchr (aname, '.');
   CHAR *c  = NULL;
   INT  num;
   if (cp == NULL || !*(cp + 1)) {
      num = -1;
   } else {
      cp++;
      num = (*(cp + 1) - '0') * 10 + *(cp + 2) - '0';
      if (!in(num, 0, 99))
         num = -1;
      if (in(*cp, '0', '9'))
         num += (*cp - '0') * 100;
   }
   num++;
   if (num < 100) {
      *cp = 'C';
      c = cp;
   } else {
      *cp = num / 100 + '0';
   }
   *(cp + 1) = (num / 10) % 10 + '0';
   *(cp + 2) = num % 10 + '0';
   /* UNIX: C00 & c00 are different strings */
   if (c && !fileexists (aname)) {
      *c = 'c';
   }

   // try to open volume, read archive header
   if (!open_archive (1)) {
      printf("\nError while opening archive. File not found or archive broken.\n");
      f_err = ERR_OPEN;
      return 0;
   }

   if (!read_header(1))         // read 2nd header
   {
      f_err=ERR_READ;
      return 0;
   }
   return 1;
}


INT  read_adds_blk(CHAR * buffer, INT len)      // reads part of ADD_SIZE block
{
   INT  rd = 0,
        l = len;
   LONG i;

   while (!f_err && len && skipsize)
   {
      i = (skipsize > len) ? len : skipsize;
      skipsize -= i;

      errno = 0;
      rd += fread(buffer, 1, i, archive_fp);
      if (errno)
      {
         printf("\nRead error\n");
         f_err = ERR_READ;
      }

      buffer += i;
      len -= i;

      if (!skipsize)            // if block is continued on next volume
         if (head.HEAD_FLAGS & ACE_SP_AFTER && !proc_next_vol())
            break;
   }

   return (rd > l ? l : rd);
}

void crc_print(void)            // checks CRC, prints message
{
   INT  crc_not_ok = rd_crc != fhead.CRC32;  /* check CRC of file */

   if(crc_not_ok)
     f_err_crc=1;

   if (!f_err)                  // print message
   {                            
      printf(crc_not_ok ? "          CRC-check error" : "          CRC OK");
      flush;
   }
}

void analyze_file(void)         // analyzes one file (for solid archives)
{
   printf("\n Analyzing");
   flush;
   while (!cancel() && (dcpr_adds_blk(buf_wr, size_wrb))) // decompress only
      ;
   crc_print();
}

void extract_file(void)         // extracts one file
{
   size_t rd;
   printf("\n Extracting");
   flush;                       // decompress block
   while (!cancel() && (rd = dcpr_adds_blk(buf_wr, size_wrb)))
   {
      if (fwrite (buf_wr, 1, rd, outfile_fp) != rd)       // write block
      {                         
         printf("\nWrite error\n");
         f_err = ERR_WRITE;
      }
   }
   crc_print();
}

/* extracts or tests all files of the archive
 */
void extract_files(int nopath, int test)
{
   CHAR file[PATH_MAX];

   while (!cancel() && read_header(1))
   {
      if (head.HEAD_TYPE == FILE_BLK)
      {
         comment_out("File comment:");   // show file comment
         ace_fname(file, &head, nopath, sizeof(file), 1); // get file name
         printf("\n%s", file);
         flush;
         dcpr_init_file();               // initialize decompression of file
         if (!f_err)
         {
            if (test || 
                (outfile_fp = create_dest_file(file, (INT) fhead.ATTR)) == NULL)
            {
               if ((test || adat.sol) && (f_err != ERR_USER))
                  analyze_file();        // analyze file
            }
            else
            {
               extract_file();           // extract it
               fclose (outfile_fp);
               if (f_err)
                  remove(file);
            }
         }
      }
   }
}

unsigned percentage(ULONG p, ULONG d)
{
   return (unsigned)( d ? (d/2+p*100)/d : 100 );
}

void list_files(int verbose)
{
   unsigned files=0;
   ULONG    size =0,
            psize=0,
            tpsize;
   CHAR     file[PATH_MAX];

   printf("Date    |Time |Packed     |Size     |Ratio|File\n");

   while (!cancel() && read_header(1))
   {
      if (head.HEAD_TYPE == FILE_BLK)
      {
         ULONG ti=fhead.FTIME;
         ace_fname(file, &head, verbose ? 0 : 1, sizeof(file), 0); // get file name

         size  += fhead.SIZE;
         psize +=
         tpsize = fhead.PSIZE;
         files++;

         while (head.HEAD_FLAGS & ACE_SP_AFTER)
         {
            skipsize=0;
            if (!proc_next_vol())
               break;
            psize += fhead.PSIZE;
            tpsize+= fhead.PSIZE;
         }
         if (!f_err)
            printf("%02u.%02u.%02u|%02u:%02u|%c%c%9u|%9u|%4u%%|%c%s\n",
                   ts_day (ti), ts_month(ti), ts_year(ti)%100,
                   ts_hour(ti), ts_min  (ti),
                   fhead.HEAD_FLAGS & ACE_SP_BEF   ? '<' : ' ',
                   fhead.HEAD_FLAGS & ACE_SP_AFTER ? '>' : ' ',
                   tpsize, fhead.SIZE, percentage(tpsize, fhead.SIZE),
                   fhead.HEAD_FLAGS & ACE_PASSW    ? '*'    : ' ',
                   file
                  );
      }
   }
   if (!f_err)
   {
      printf("\n                 %9u|%9u|%4u%%| %u file%s",
             psize,
             size,
             percentage(psize, size),
             files,
             (char*)(files == 1 ? "" : "s")
            );
   }
}

void showhelp(void)
{
   printf("\n"
          "Usage: UNACE <command> [<switches>] <archive[.ace]>\n"
          "\n"
          "Where <command> is one of:\n"
          "\n"
          "  e   Extract files\n"
          "  l   List archive\n"
          "  t   Test archive integrity\n"
          "  v   List archive (verbose)\n"
          "  x   Extract files with full path\n"
          "\n"
          "And <switches> is zero or more of:\n"
          "\n"
          " -y   Assume 'yes' on all questions, never ask for input"
          "\n"
          " -o   Overwrite existing file(s)"
          "\n"
          " -n   Never overwrite existing file(s)"
        );
}

int main(INT argc, CHAR * argv[])              // processes the archive
{

#if defined(W_MTRACE) && defined(__GLIBC__)
   mtrace();
#endif

   INT show_help,
       arg_cnt = 1;

   printf("%s", version_msg);
   show_help=0;

   if (argc < 3 || strlen(argv[1]) > 1 || argv[argc-1][0] == '-') {
      show_help=1;
      f_err = ERR_CLINE;
   }

   // empty args or -h /? trigger help without error code
   if (argc == 1 || !strcmp(argv[1],"-h") || !strcmp(argv[1],"/?")) {
       show_help=1;
       f_err = 0;
   }

   while (!show_help && argv[++arg_cnt][0] == '-')
   {
      switch (tolower(argv[arg_cnt][1]))
      {
         case 'y':
            f_ovrall    = 1;    // Overwrite all
            break;
         case 'o':
            f_ovrall    = 1;    // Overwrite all
            break;
         case 'n':
            f_ovrnvr    = 1;    // Never Overwrite
            break;
         default:
            show_help = 1;
            f_err = ERR_CLINE;
            break;
      }
   }

   if (show_help)
     showhelp();
   else
   {
      CHAR *s;

      init_unace();                              // initialize unace

      // get archive name
      snprintf (aname, sizeof(aname)-4, "%s", argv[arg_cnt]);

      if (!(s = strrchr(aname, DIRSEP)))
         s = aname;
      if (!strrchr(s, '.'))
         strcat(aname, ".ace");

      if (open_archive(1))                       // open archive to process
      {
         if (adat.vol_num)
            printf("\nFirst volume of archive required!\n");
         else
            switch (tolower(*argv[1]))
            {
               case 'e': extract_files(1, 0); break;  // extract files without path
               case 'x': extract_files(0, 0); break;  // extract files with path
               case 'l': list_files   (0   ); break;  // list files
               case 'v': list_files   (1   ); break;  // list files verbose
               case 't': extract_files(0, 1); break;  // test archive integrity.
               default : showhelp();                  // Wrong command!
            }

         fclose (archive_fp);
         if (f_err && (f_err != ERR_USER))
         {
            printf("\nError occurred\n");
            if (f_criterr)
               printf("Critical error on drive %c\n", f_criterr);
         }
      }
      else
         f_err = ERR_CLINE;

      done_unace();
   }

   putchar('\n');
   putc   ('\n', stderr);

   if (!f_err && f_err_crc)
   {
      printf("One or more CRC-errors were found.\n");
      f_err = ERR_CRC;
   }
   return f_err;
}

