/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */

#include <stdio.h>    // printf()

#include "globals.h"
#include "uac.h"

INT  comm_cpr_size=0;
UCHAR *comm;

// ==========================================================
/*      Decompresses and outputs comment if present.       */
// ==========================================================

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

ULONG crctable[256];
ULONG rd_crc=0;

void make_crctable(void)   // initializes CRC table
{
   ULONG r, i, j;
   for (i = 0; i <= 255; i++)
   {
      for (r = i, j = 8; j; j--)
         r = (r & 1) ? (r >> 1) ^ CRCPOLY : (r >> 1);
      crctable[i] = r;
   }
}

// Updates crc from addr till addr+len-1
ULONG getcrc(ULONG crc, UCHAR * addr, INT len)
{
   while (len--)
      crc = crctable[(unsigned char) crc ^ (*addr++)] ^ (crc >> 8);
   return (crc);
}
