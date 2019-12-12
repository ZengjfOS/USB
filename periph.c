//-----------------------------------------------------------------------------
//   File:      periph.c
//   Contents:  Hooks required to implement USB peripheral function.
//
// $Archive: /USB/Examples/FX2LP/hid_kb/periph.c $
// $Date: 3/23/05 2:58p $
// $Revision: 2 $
//
//
//-----------------------------------------------------------------------------
// Copyright 2003, Cypress Semiconductor Corporation
//
// This software is owned by Cypress Semiconductor Corporation (Cypress) and is
// protected by United States copyright laws and international treaty provisions. Cypress
// hereby grants to Licensee a personal, non-exclusive, non-transferable license to copy,
// use, modify, create derivative works of, and compile the Cypress Source Code and
// derivative works for the sole purpose of creating custom software in support of Licensee
// product ("Licensee Product") to be used only in conjunction with a Cypress integrated
// circuit. Any reproduction, modification, translation, compilation, or representation of this
// software except as specified above is prohibited without the express written permission of
// Cypress.
//
// Disclaimer: Cypress makes no warranty of any kind, express or implied, with regard to
// this material, including, but not limited to, the implied warranties of merchantability and
// fitness for a particular purpose. Cypress reserves the right to make changes without
// further notice to the materials described herein. Cypress does not assume any liability
// arising out of the application or use of any product or circuit described herein. Cypress’
// products described herein are not authorized for use as components in life-support
// devices.
//
// This software is protected by and subject to worldwide patent coverage, including U.S.
// and foreign patents. Use may be limited by and subject to the Cypress Software License
// Agreement.
//-----------------------------------------------------------------------------
#pragma NOIV               // Do not generate interrupt vectors

#include "fx2.h"
#include "fx2regs.h"

extern BOOL   GotSUD;         // Received setup data flag
extern BOOL   Sleep;
extern BOOL   Rwuen;
extern BOOL   Selfpwr;

WORD	pHIDDscr;
WORD	pReportDscr;
WORD	pReportDscrEnd;
extern code HIDDscr;
extern code HIDReportDscr; 
extern code HIDReportDscrEnd;

BYTE xdata buttons;
BYTE xdata oldbuttons;
BYTE xdata leds;

BYTE   Configuration;      // Current configuration
BYTE   AlternateSetting;   // Alternate settings

#define BTN_ADDR		0x20
#define LED_ADDR		0x21

//-----------------------------------------------------------------------------
// Task Dispatcher hooks
//   The following hooks are called by the task dispatcher.
//-----------------------------------------------------------------------------

void TD_Init(void)             // Called once at startup
{
   leds = 0xFF;
   oldbuttons = 0x01;

   EP1OUTCFG = 0xB0;       // valid, interrupt
   EP1INCFG = 0XB0;        // valid, interrupt

   OEA = 0x01 << 7;
   PA7 = 0;

   EP2CFG = EP4CFG = EP6CFG = EP8CFG = 0;   // disable unused endpoints

	// EZUSB_InitI2C();			// Initialize EZ-USB I2C controller


}

void TD_Poll(void)             // Called repeatedly while the device is idle
{
   if( !(EP1INCS & bmEPBUSY) )	// Is the IN1BUF available,
   {
      buttons = PA6;	// Read button states
	  buttons &= 0x01;
      if ((oldbuttons - buttons) != 0)	//Change in button state
	  {
         EP1INBUF[0] = 0;

         if (buttons & 1)	//a
			EP1INBUF[2] = 4;
         else
            EP1INBUF[2] = 0;

		 EP1INBUF[3] = 0;
         EP1INBUF[4] = 0;

		 EP1INBUF[1] = 0;
		 EP1INBC = 5;

			
	  	 PA7 = PA6;
      }
      oldbuttons = buttons;
   }

	if( !(EP1OUTCS & bmEPBUSY) )	// Is there something available
	{
		leds = 0xFF;			//Turn all off

		if (EP1OUTBUF[0] & 0x02)	//Caps
			leds &= 0xBF;
		if (EP1OUTBUF[0] & 0x01)	//Scroll
			leds &= 0xF7;
		if (EP1OUTBUF[0] & 0x04)	//Num
			leds &= 0xFE;

		EP1OUTBC = 0;				//Rearm endpoint buffer
	}
}


BOOL TD_Suspend(void)          // Called before the device goes into suspend mode
{
   return(TRUE);
}

BOOL TD_Resume(void)          // Called after the device resumes
{
   return(TRUE);
}

//-----------------------------------------------------------------------------
// Device Request hooks
//   The following hooks are called by the end point 0 device request parser.
//-----------------------------------------------------------------------------

BOOL DR_GetDescriptor(void)
{
	BYTE length,i;

	pHIDDscr = (WORD)&HIDDscr;
	pReportDscr = (WORD)&HIDReportDscr;
	pReportDscrEnd = (WORD)&HIDReportDscrEnd;

	switch (SETUPDAT[3])
	{
		case GD_HID:					//HID Descriptor
			SUDPTRH = MSB(pHIDDscr);
			SUDPTRL = LSB(pHIDDscr);
			return (FALSE);
		case GD_REPORT:					//Report Descriptor
			length = pReportDscrEnd - pReportDscr;

         AUTOPTR1H = MSB(pReportDscr);
         AUTOPTR1L = LSB(pReportDscr);

         for(i=0;i<length;i++)
            EP0BUF[i]=XAUTODAT1;

         EP0BCL = length;
         return (FALSE);
		default:
			return(TRUE);
	}
}

BOOL DR_SetConfiguration(void)   // Called when a Set Configuration command is received
{
   Configuration = SETUPDAT[2];
   return(TRUE);            // Handled by user code
}

BOOL DR_GetConfiguration(void)   // Called when a Get Configuration command is received
{
   EP0BUF[0] = Configuration;
   EP0BCH = 0;
   EP0BCL = 1;
   return(TRUE);            // Handled by user code
}

BOOL DR_SetInterface(void)       // Called when a Set Interface command is received
{
   AlternateSetting = SETUPDAT[2];
   return(TRUE);            // Handled by user code
}

BOOL DR_GetInterface(void)       // Called when a Set Interface command is received
{
   EP0BUF[0] = AlternateSetting;
   EP0BCH = 0;
   EP0BCL = 1;
   return(TRUE);            // Handled by user code
}

BOOL DR_GetStatus(void)
{
   return(TRUE);
}

BOOL DR_ClearFeature(void)
{
   return(TRUE);
}

BOOL DR_SetFeature(void)
{
   return(TRUE);
}

BOOL DR_VendorCmnd(void)
{
   return(TRUE);
}

//-----------------------------------------------------------------------------
// USB Interrupt Handlers
//   The following functions are called by the USB interrupt jump table.
//-----------------------------------------------------------------------------

// Setup Data Available Interrupt Handler
void ISR_Sudav(void) interrupt 0
{
   GotSUD = TRUE;            // Set flag
   EZUSB_IRQ_CLEAR();
   USBIRQ = bmSUDAV;         // Clear SUDAV IRQ
}

// Setup Token Interrupt Handler
void ISR_Sutok(void) interrupt 0
{
   EZUSB_IRQ_CLEAR();
   USBIRQ = bmSUTOK;         // Clear SUTOK IRQ
}

void ISR_Sof(void) interrupt 0
{
   EZUSB_IRQ_CLEAR();
   USBIRQ = bmSOF;            // Clear SOF IRQ
}

void ISR_Ures(void) interrupt 0
{
   // whenever we get a USB reset, we should revert to full speed mode
   pConfigDscr = pFullSpeedConfigDscr;
   ((CONFIGDSCR xdata *) pConfigDscr)->type = CONFIG_DSCR;
   pOtherConfigDscr = pHighSpeedConfigDscr;
   ((CONFIGDSCR xdata *) pOtherConfigDscr)->type = OTHERSPEED_DSCR;
   
   EZUSB_IRQ_CLEAR();
   USBIRQ = bmURES;         // Clear URES IRQ
}

void ISR_Susp(void) interrupt 0
{
   Sleep = TRUE;
   EZUSB_IRQ_CLEAR();
   USBIRQ = bmSUSP;
}

void ISR_Highspeed(void) interrupt 0
{
   if (EZUSB_HIGHSPEED())
   {
      pConfigDscr = pHighSpeedConfigDscr;
      ((CONFIGDSCR xdata *) pConfigDscr)->type = CONFIG_DSCR;
      pOtherConfigDscr = pFullSpeedConfigDscr;
      ((CONFIGDSCR xdata *) pOtherConfigDscr)->type = OTHERSPEED_DSCR;
   }

   EZUSB_IRQ_CLEAR();
   USBIRQ = bmHSGRANT;
}
void ISR_Ep0ack(void) interrupt 0
{
}
void ISR_Stub(void) interrupt 0
{
}
void ISR_Ep0in(void) interrupt 0
{
}
void ISR_Ep0out(void) interrupt 0
{
}
void ISR_Ep1in(void) interrupt 0
{
}
void ISR_Ep1out(void) interrupt 0
{
}
void ISR_Ep2inout(void) interrupt 0
{
}
void ISR_Ep4inout(void) interrupt 0
{
}
void ISR_Ep6inout(void) interrupt 0
{
}
void ISR_Ep8inout(void) interrupt 0
{
}
void ISR_Ibn(void) interrupt 0
{
}
void ISR_Ep0pingnak(void) interrupt 0
{
}
void ISR_Ep1pingnak(void) interrupt 0
{
}
void ISR_Ep2pingnak(void) interrupt 0
{
}
void ISR_Ep4pingnak(void) interrupt 0
{
}
void ISR_Ep6pingnak(void) interrupt 0
{
}
void ISR_Ep8pingnak(void) interrupt 0
{
}
void ISR_Errorlimit(void) interrupt 0
{
}
void ISR_Ep2piderror(void) interrupt 0
{
}
void ISR_Ep4piderror(void) interrupt 0
{
}
void ISR_Ep6piderror(void) interrupt 0
{
}
void ISR_Ep8piderror(void) interrupt 0
{
}
void ISR_Ep2pflag(void) interrupt 0
{
}
void ISR_Ep4pflag(void) interrupt 0
{
}
void ISR_Ep6pflag(void) interrupt 0
{
}
void ISR_Ep8pflag(void) interrupt 0
{
}
void ISR_Ep2eflag(void) interrupt 0
{
}
void ISR_Ep4eflag(void) interrupt 0
{
}
void ISR_Ep6eflag(void) interrupt 0
{
}
void ISR_Ep8eflag(void) interrupt 0
{
}
void ISR_Ep2fflag(void) interrupt 0
{
}
void ISR_Ep4fflag(void) interrupt 0
{
}
void ISR_Ep6fflag(void) interrupt 0
{
}
void ISR_Ep8fflag(void) interrupt 0
{
}
void ISR_GpifComplete(void) interrupt 0
{
}
void ISR_GpifWaveform(void) interrupt 0
{
}
