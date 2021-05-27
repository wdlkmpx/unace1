/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      Some basic things.                                                  */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */

#include "declare.h"

#include <signal.h>  // signal()
#include <stdio.h>   // fprintf() fflush() getch() putc()
#include <ctype.h>   // tolower()

#include "globals.h"
#include "uac.h"

void memset16(USHORT * dest, SHORT val, INT len)  // fills short-array with
{                                                 // value
   while (len--)
      *(dest++) = val;
}

INT  cancel(void)               // checks whether to interrupt the program
{
   return (f_err);
}

INT  wrask(CHAR * s)            // prompt-routine
{
   INT  ch;

   fprintf(stderr, "\n %s (Yes,Always,No,Cancel) ", s);
   fflush(stderr);
   do
   {
      ch = getch();
      ch = toupper(ch);
   }
   while (ch != 'Y' && ch != 'A' && ch != 'N' && ch != 'C' && ch != 27)
      ;
   fprintf(stderr, "%s", ch == 'Y' ? "Yes" : (ch == 'A' ? "Always" : (ch == 'N' ? "No" : "Cancel")));
   fflush(stderr);
   return (ch == 'Y' ? 0 : (ch == 'A' ? 1 : (ch == 'N' ? 2 : 3)));
}

void beep(void)                           // makes some noise
{
   putc(0x07, stdout);
}

void my_signalhandler(INT sig_number)     // sets f_err if ctrl+c or ctrl+brk
{
   f_err = ERR_USER;
   printf("\nUser break\n");
}

void set_handler(void)                    // initializes handlers
{
   signal(SIGINT, my_signalhandler);
}

