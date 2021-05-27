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

#ifdef _WIN32
#include <direct.h>   // mkdir()
#else
#include <unistd.h>
#endif

#include "globals.h"
#include "uac_crt.h"
#include "uac.h"

/* gets file name from header
 */
CHAR *ace_fname(CHAR * s, thead * head, INT nopath, unsigned int size)
{
   unsigned int i;
   char *cp;

   i = (*(tfhead *) head).FNAME_SIZE;
   if (i > (size - 1))
     i = size - 1;
   strncpy(s, (*(tfhead *) head).FNAME, i);
   s[i] = 0;

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

int is_directory_traversal(char *str)
{
  unsigned int mode, countdots;
  /* mode 0 = fresh, 1 = just dots, 2 = not just dots */

  mode = countdots = 0;

  while (*str)
  {
    char ch = *str++;

    if ((ch == '/') && (mode == 1) && (countdots > 1))
      return 1;

    if (ch == '/')
    {
       mode = countdots = 0;
       continue;
    }

    if (ch == '.')
    {
      if (mode == 0)
        mode = 1;

      countdots++;
    }
    else
      mode = 2;
  }

  if ((mode == 1) && (countdots > 1))
    return 1;

  return 0;
}

void check_ext_dir(CHAR * f)        // checks/creates path of file
{
   CHAR *cp,
        d[PATH_MAX];
   unsigned int i;

   d[0] = 0;

   if (is_directory_traversal(f))
   {
      f_err = ERR_WRITE;
      printf("\n    Directory traversal attempt:  %s\n", f);
      return;
   }

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

INT  create_dest_file(CHAR * file, INT a)  // creates file or directory
{
   INT  han,
        i  = 0,
        ex = fileexists(file);
   struct stat st;

   check_ext_dir(file);
   if (f_err)
      return (-1);
   if (a & _A_SUBDIR)
   {                                // create dir or file?
      if (ex) stat(file, &st);
      if (ex ? (st.st_mode & S_IFDIR) : mkdir(file))
      {
         printf("\n    Could not create directory.\n");
         return (-1);
      }
      return (-1);
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
            return (-1);            // delete?
      }
      if ((han = open(file, O_WRONLY | O_TRUNC | O_CREAT | O_BINARY,
                            S_IREAD | S_IWRITE | S_IEXEC | S_IDELETE |
                            S_IRGRP | S_IWGRP  | S_IROTH | S_IWOTH )) < 0)
         printf("\n    Could not create destination file.\n");
      return (han);
   }
}

