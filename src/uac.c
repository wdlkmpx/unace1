/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */

#include "unace.h"
#include <sys/stat.h>  // struct stat
#include <ctype.h>     // toupper()
#ifdef _WIN32
#include <direct.h>   // mkdir()
#else
#include <unistd.h>
#define mkdir(_a) mkdir(_a,  S_IRWXU | S_IRWXG | S_IRWXO)
#endif

// ==========================================================
/*      Decompresses and outputs comment if present.       */
// ==========================================================

INT  comm_cpr_size=0;
UCHAR *comm;

void comment_out(const CHAR *top)      // outputs comment if present
{
   INT  i;

   if (head.HEAD_FLAGS & ACE_COMM)
   {                             // comment present?
      if (head.HEAD_TYPE == MAIN_BLK)
      {                          // get begin and size of comment data
         comm = MCOMM;
         comm_cpr_size = MCOMM_SIZE;
      }
      else
      {
         comm = FCOMM;
         comm_cpr_size = FCOMM_SIZE;
      }                          // limit comment size if too big
      i = sizeof(head) - (INT)(comm - (UCHAR*) &head);
      if (comm_cpr_size > i)
         comm_cpr_size = i;
      dcpr_comm(i);              // decompress comment

      printf("%s\n\n%s\n\n", top, comm); // output comment
   }
}

// ==========================================================
/*                  CRC-calculation                        */
// ==========================================================

#define CRCPOLY  0xEDB88320L
static ULONG crctable[256];
ULONG rd_crc=0;

void make_crctable(void)   // initializes CRC table
{
   ULONG r, i, j;
   for (i = 0; i < 256; i++)
   {
      for (r = i, j = 8; j; j--)
         r = (r & 1) ? (r >> 1) ^ CRCPOLY : (r >> 1);
      crctable[i] = r;
   }
}

ULONG getcrc(ULONG crc, UCHAR * addr, INT len)
{ // Updates crc from addr till addr+len-1
   while (len--)
      crc = crctable[(unsigned char) crc ^ (*addr++)] ^ (crc >> 8);
   return (crc);
}

// ==========================================================
/*                    File creation                        */
// ==========================================================

// prompt-routine
INT  wrask(const CHAR * s)
{
   INT  ch;

   fprintf(stderr, "\n %s (Yes,Always,No,Cancel) ", s);
   fflush(stderr);
   do {
      ch = getchar();
      ch = toupper(ch);
   }
   while (ch != 'Y' && ch != 'A' && ch != 'N' && ch != 'C' && ch != 27)
      ;
   fprintf(stderr, "%s", ch == 'Y' ? "Yes" : (ch == 'A' ? "Always" : (ch == 'N' ? "No" : "Cancel")));
   fflush(stderr);
   return (ch == 'Y' ? 0 : (ch == 'A' ? 1 : (ch == 'N' ? 2 : 3)));
}


static int is_directory_traversal(char *str)
{
  if (*str == '.' && (!strncmp(str,"../",3) || !strncmp(str,"..\\",3))) {
      return 1;
  }
  if (strstr(str, "/../")  || strstr(str, "\\..\\")) {
      return 1;
  }
  return 0;
}

/* gets file name from header
 */
void ace_fname (CHAR * out_s, thead * head, INT nopath, unsigned int size, INT extract)
{
   char *cp = NULL;
   unsigned int fnsize = ((tfhead*)head)->FNAME_SIZE;
   unsigned int x = (fnsize >= size) ? (size-1) : fnsize;
   snprintf (out_s, size-1, "%s", ((tfhead*)head)->FNAME);
   // need exact size, as the filename may not end in '\0'
   out_s[x] = 0;

   if (extract)
   {
      // '/': UNIX-specific attack
      //  - ACE32.EXE creates:
      //       man\man8\e2mmpstatus.8
      //  - tests/dirtraversal2.ace:
      //       /tmp/unace-dir-traversal
      //       ('/' must not be present in the string)
     if (is_directory_traversal(out_s) || strchr(out_s,'/')) {
        f_err = ERR_WRITE;
        printf("\n    Directory traversal attempt:  %s\n", out_s);
        *out_s = 0;
        return;
     }
   }

   if (nopath)
   {
      cp = strrchr(out_s, '\\');
      if (cp)
         memmove(out_s, cp+1, strlen(cp));
   }
#if (DIRSEP!='\\')                  // replace msdos directory seperator
   else
   {                                // by current OS seperator
      cp = out_s;
      while ((cp=strchr(cp, '\\'))!=NULL)
         *cp++=DIRSEP;
   }
#endif

   cp = out_s;
   while (*cp == '/') cp++;
   if (cp != out_s)
     memmove(out_s, cp, strlen(cp) + 1);
}

static void mksubdirs (CHAR * f)        // checks/creates path of file
{
   CHAR d[PATH_MAX];
   CHAR *p = d;
   snprintf (d, sizeof(d)-1, "%s", f);

   while (p && *p)
   {
      p = strchr (p, DIRSEP);
      if (!p)
         return;
      *p = 0;
      if (!fileexists(d)) {
         if (mkdir(d) != 0) {
            f_err = ERR_WRITE;
            printf("\n    Error while creating directory.\n");
         }
     }
     *p = DIRSEP;
     p++;
   }
}


FILE * create_dest_file (CHAR * file, INT a)  // creates file or directory
{
   FILE * han;
   INT  i  = 0,
        ex = fileexists(file);

   mksubdirs (file);
   if (f_err)
      return (NULL);
#ifndef _A_SUBDIR //already defined in mingw
#define _A_SUBDIR 0x10 /* MS-DOS directory constant */
#endif
   if (a & _A_SUBDIR)
   {                                // create dir or file?
      if (!ex) {
         if (mkdir(file) != 0) {
             printf("\n    Could not create directory.\n");
         }
      }
      return (NULL);
   }
   else
   {
      if (ex)
      {                             // does the file already exist
         if (f_ovrnvr) return NULL;
         if (!f_ovrall)
         {
            i = wrask("Overwrite existing file?");  // prompt for overwrite
            f_ovrall = (i == 1);
            if (i == 3) {
               f_err = ERR_USER;
               return NULL;
            }
         }
         // i: 0=yes 1=all
         if (i > 1 && !f_ovrall) {
            return NULL;
         } else if (remove(file) != 0) {
            printf("\n    Could not delete file or directory. Access denied.\n");
            return NULL;
         }
      }
      han = fopen (file, "wb");
      if (!han) {
         printf("\n    Could not create destination file.\n");
      }
      return han;
   }
}

