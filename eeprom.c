//-----------------------------------------------------------------------------
//   File:      eeprom.c
//-----------------------------------------------------------------------------
#include "fx2.h"
#include "fx2regs.h"
#include "eeprom.h"

///////////////////////////////////////////////////////////////////////////////////////

// Returns 0 on success, 1 on failure
// Normally called within a while() loop so that errors are retried:
// while (EEPROMWrite(....))
//    ;


bit EEPROMWrite(WORD addr, BYTE * ptr, BYTE length)
{
    BYTE		i;
    bit      retval;

    EEPROM_DISABLE_WRITE_PROTECT();

    // Make sure the i2c interface is idle
    EEWaitForStop();
    
    // write the START bit and i2c device address
    EEStartAndAddr();
    
    if(EEWaitForAck())
    {
    	retval = 1;
        goto EXIT_WP;
    }

    // write the eeprom offset
    if (DB_Addr)
    {
        I2DAT = MSB(addr);
        if(EEWaitForAck())
        {
            retval = 1;
            goto EXIT_WP;
        }
    }
    I2DAT = LSB(addr);
    if(EEWaitForAck())
    {
	    retval = 1;
        goto EXIT_WP;
    }

    // Write the data Page
    for (i = 0; i < length; i++)
    {
        I2DAT = *ptr++;
        if(EEWaitForDone())
        {
		    retval = 1;
            goto EXIT_WP;
        }
    }	
    I2CS |= bmSTOP;
    WaitForEEPROMWrite();

    retval = 0;

EXIT_WP:            
    EEPROM_ENABLE_WRITE_PROTECT();
    return(retval);
}

void EEStartAndAddr()
{
      I2CS = bmSTART;
      I2DAT = I2C_Addr << 1;
}

// 0x2e in assembly, less than 0x20 with compiler optimization!!
void WaitForEEPROMWrite()
{
   EEWaitForStop();
waitForBusy:
	EEStartAndAddr();

   EEWaitForDone();
   I2CS |= bmSTOP;	//	; Set the STOP bit
   EEWaitForStop();

   if (!(I2CS & bmACK))  // If no ACK, try again.
      goto waitForBusy;
}

void EEWaitForStop()
{
   // Data should not be written to I2CS or I2DAT until the STOP bit returns low.
   while (I2CS & bmSTOP)
      ;
}

// Returns 0 on success, 1 on failure
bit EEPROMRead(WORD addr, BYTE length, BYTE *buf)  //addr is the offset address, length - number of bytes, buf - where to store
{
   BYTE i;

  // Make sure the i2c interface is idle
   EEWaitForStop();
   
   // write the START bit and i2c device address
   EEStartAndAddr();

   if(EEWaitForAck())
   {
    	  return(1);
		  }


   // write the eeprom offset
   if (DB_Addr)
      {
      I2DAT = MSB(addr);
      if(EEWaitForAck())
	  {
         return(1);
		 }
      }
   I2DAT = LSB(addr);
   if(EEWaitForAck())
      return(1);

   I2CS = bmSTART;

   // send the read command
   I2DAT = (I2C_Addr << 1) | 1;
   if(EEWaitForDone())
      return(1);

   // read dummy byte
   i = I2DAT;
   if(EEWaitForDone())
      return(1);

   for (i=0; i < (length - 1); i++)
   {
      *(buf+i) = I2DAT;
      if(EEWaitForDone())
         return(1);
   }
   
   I2CS = bmLASTRD;
   if(EEWaitForDone())
      return(1);

   *(buf+i) = I2DAT;
   if(EEWaitForDone())
      return(1);

   I2CS = bmSTOP;

   i = I2DAT;
   return(0);
}

// Return 0 for ok, 1 for error
bit EEWaitForDone()
{
   BYTE i;

   while (!((i = I2CS) & 1))  // Poll the done bit
      ;
   if (i & bmBERR)
      return 1;
   else
      return 0;
}

// Return 0 for ok, 1 for error
// Same as wait for done, but checks for ACK as well
bit EEWaitForAck()
{
   BYTE i;

   while (!((i = I2CS) & 1))  // Poll the done bit
      ;
   if (i & bmBERR)
      return 1;
   else if (!(i & bmACK))
      return 1;
   else
      return 0;
}


