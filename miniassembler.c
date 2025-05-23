/*--------------------------------------------------------------------*/
/* miniassembler.c                                                    */
/* Author: Bob Dondero, Donna Gabai                                   */
/* Completed by Ben Zhou and Owen Clarke                              */
/*--------------------------------------------------------------------*/

#include "miniassembler.h"
#include <assert.h>
#include <stddef.h>

/*--------------------------------------------------------------------*/
/* Modify *puiDest in place,
   setting uiNumBits starting at uiDestStartBit (where 0 indicates
   the least significant bit) with bits taken from uiSrc,
   starting at uiSrcStartBit.
   uiSrcStartBit indicates the rightmost bit in the field.
   setField sets the appropriate bits in *puiDest to 1.
   setField never unsets any bits in *puiDest.                        */
static void setField(unsigned int uiSrc, unsigned int uiSrcStartBit,
                     unsigned int *puiDest, unsigned int uiDestStartBit,
                     unsigned int uiNumBits)
{
   unsigned int i;
   unsigned int j;
   unsigned int mask;

   for (i = 0; i < uiNumBits; ++i)
   {
      /* Uses a mask in order to determine if the bit is 1 */

      mask = 1;
      for (j = 0; j < uiSrcStartBit + i; j++)
      {
         mask *= 2;
      }
      /* If it is not 0, we proceed */

      if ((uiSrc & mask) != 0)
      {
         mask = 1;
      
         for (j = 0; j < uiDestStartBit + i; j++)
         {
            mask *= 2;
         }
         /* Similar idea to earlier */
         if ((*puiDest & mask) == 0)
         {
            *puiDest += mask;
         }
      }
   }
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_mov(unsigned int uiReg, int iImmed)
{
   unsigned int uiInstr;

   uiInstr = 0x52800000U;

   setField(uiReg, 0, &uiInstr, 0, 5);
   setField(iImmed, 0, &uiInstr, 5, 16);

   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_adr(unsigned int uiReg, unsigned long ulAddr,
                               unsigned long ulAddrOfThisInstr)
{
   unsigned int uiInstr;
   unsigned int uiDisp;

   /* Base Instruction Code */
   uiInstr = 0x10000000;

   /* register to be inserted in instruction */
   setField(uiReg, 0, &uiInstr, 0, 5);

   /* displacement to be split into immlo and immhi and inserted */
   uiDisp = (unsigned int)(ulAddr - ulAddrOfThisInstr);

   setField(uiDisp, 0, &uiInstr, 29, 2);
   setField(uiDisp, 2, &uiInstr, 5, 19);

   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_strb(unsigned int uiFromReg,
                                unsigned int uiToReg)
{
   unsigned int uiInstr;

   uiInstr = 0x39000000U;
   setField(uiFromReg, 0, &uiInstr, 0, 5);
   setField(uiToReg, 0, &uiInstr, 5, 5);

   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_b(unsigned long ulAddr,
                             unsigned long ulAddrOfThisInstr)
{
   unsigned int uiInstr;
   unsigned long uiDisp;

   uiInstr = 0x14000000U;

   uiDisp = (unsigned int)((long)ulAddr - (long)ulAddrOfThisInstr);
   uiDisp = uiDisp >> 2;
   uiDisp = (unsigned int)uiDisp;

   setField(uiDisp, 0, &uiInstr, 0, 26);

   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_bl(unsigned long ulAddr,
                              unsigned long ulAddrOfThisInstr)
{
   unsigned int uiInstr;
   unsigned long uiDisp;

   uiInstr = 0x94000000u;

   uiDisp = (unsigned int)(long)ulAddr - (long)ulAddrOfThisInstr;
   uiDisp = uiDisp >> 2;
   uiDisp = (unsigned int)uiDisp;

   setField(uiDisp, 0, &uiInstr, 0, 26);

   return uiInstr;
}