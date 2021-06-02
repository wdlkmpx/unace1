/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Creates/Replaces files or directories.                              */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */

#include "declare.h"

#include <stdio.h>     // printf() remove()
#include <string.h>    // strncpy()
#include <sys/stat.h>  // struct stat
#include <ctype.h>   // tolower()

#ifdef _WIN32
#include <direct.h>   // mkdir()
#else
#include <unistd.h>
#define mkdir(_a) mkdir(_a,  S_IRWXU | S_IRWXG | S_IRWXO)
#endif

#include "globals.h"
#include "uac.h"

#ifndef _A_SUBDIR
  #define _A_SUBDIR 0x10        /* MS-DOS directory constant */
#endif


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
   unsigned int fnsize = (*(tfhead *) head).FNAME_SIZE;
   unsigned int x = (fnsize >= size) ? (size-1) : fnsize;
   strncpy (out_s, (*(tfhead *) head).FNAME, size-2);
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
   size_t len = (strlen (f) >= PATH_MAX) ? (PATH_MAX - 1) : (strlen (f));
   strncpy (d, f, len);

   while (p && *p)
   {
      p = strchr (p, DIRSEP);
      if (!p)
         return;
      *p = 0;
      if (!fileexists(d)) {
         if (mkdir(d)) {
            f_err = ERR_WRITE;
            printf("\n    Error while creating directory.\n");
         }
     }
     *p = DIRSEP;
     p++;
   }
}

INT  ovr_delete(CHAR * n)           // deletes directory or file
{
   if (remove(n) && rmdir(n))
   {
      printf("\n    Could not delete file or directory. Access denied.\n");
      return (1);
   }
   return (0);
}

FILE * create_dest_file (CHAR * file, INT a)  // creates file or directory
{
   FILE * han;
   INT  i  = 0,
        ex = fileexists(file);
   struct stat st;

   mksubdirs (file);
   if (f_err)
      return (NULL);
   if (a & _A_SUBDIR)
   {                                // create dir or file?
      if (ex) {
         stat (file, &st);
         if (!(st.st_mode & S_IFDIR) && mkdir(file)) {
             printf("\n    Could not create directory.\n");
         }
      }
      return (NULL);
   }
   else
   {
      if (ex)
      {                             // does the file already exist
         if (!f_ovrall)
         {
            i = wrask("Overwrite existing file?");  // prompt for overwrite
            f_ovrall = (i == 1);
            if (i == 3)
               f_err = ERR_USER;
         }
         if ((i && !f_ovrall) || ovr_delete(file))
            return (NULL);            // delete?
      }
      han = fopen (file, "wb");
      if (!han) {
         printf("\n    Could not create destination file.\n");
      }
      return (han);
   }
}

