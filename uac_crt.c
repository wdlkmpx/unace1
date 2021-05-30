/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Creates/Replaces files or directories.                              */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */

#include "declare.h"

#include <fcntl.h>     // AMIGA: open()
#include <stdio.h>     // printf() remove()
#include <string.h>    // strncpy()
#include <sys/stat.h>  // struct stat
#include <ctype.h>   // tolower()

#ifdef _WIN32
#include <direct.h>   // mkdir()
#else
#include <unistd.h>
#endif

#include "globals.h"
#include "uac_crt.h"
#include "uac.h"

// prompt-routine
INT  wrask(CHAR * s)
{
   INT  ch;

   fprintf(stderr, "\n %s (Yes,Always,No,Cancel) ", s);
   fflush(stderr);
   do {
      ch = getch();
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
CHAR *ace_fname(CHAR * s, thead * head, INT nopath, unsigned int size, INT extract)
{
   unsigned int i;
   char *cp = NULL;

   i = (*(tfhead *) head).FNAME_SIZE;
   if (i > (size - 1))
     i = size - 1;
   strncpy(s, (*(tfhead *) head).FNAME, i);
   s[i] = 0;

   if (extract)
   {
      // '/': UNIX-specific atack
      //  - ACE32.EXE creates:
      //       man\man8\e2mmpstatus.8
      //  - tests/dirtraversal2.ace:
      //       /tmp/unace-dir-traversal
      //       ('/' must not be present in the string)
     if (is_directory_traversal(s) || strchr(s,'/')) {
        f_err = ERR_WRITE;
        *s = 0;
        printf("\n    Directory traversal attempt:  %s\n", s);
        return NULL;
     }
   }

   if (nopath)
   {
      cp=strrchr(s, '\\');
      if (cp)
         memmove(s, cp+1, strlen(cp));
   }
#if (DIRSEP!='\\')                  // replace msdos directory seperator
   else
   {                                // by current OS seperator
      cp=s;
      while ((cp=strchr(cp, '\\'))!=NULL)
         *cp++=DIRSEP;
   }
#endif

   cp = s;
   while (*cp == '/') cp++;
   if (cp != s)
     memmove(s, cp, strlen(cp) + 1);

   return s;
}

void check_ext_dir(CHAR * f)        // checks/creates path of file
{
   CHAR *cp,
        d[PATH_MAX];
   unsigned int i;

   d[0] = 0;

   for (;;)
   {
      if ((cp = (CHAR *) strchr(&f[strlen(d) + 1], DIRSEP))!=NULL)
      {
         i = cp - f;
         if (i > (PATH_MAX - 1))
           i = PATH_MAX - 1;
         strncpy(d, f, i);
         d[i] = 0;
      }
      else
         return;

      if (!fileexists(d))
         if (mkdir(d))
         {
            f_err = ERR_WRITE;
            printf("\n    Error while creating directory.\n");
         }
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

   check_ext_dir(file);
   if (f_err)
      return (NULL);
   if (a & _A_SUBDIR)
   {                                // create dir or file?
      if (ex) stat(file, &st);
      if (ex ? (st.st_mode & S_IFDIR) : mkdir(file))
      {
         printf("\n    Could not create directory.\n");
         return (NULL);
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

