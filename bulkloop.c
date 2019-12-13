//-----------------------------------------------------------------------------
//   File:      bulkloop.c
//   Contents:  Hooks required to implement USB peripheral function.
//
// $Archive: /USB/Examples/FX2LP/bulkloop/bulkloop.c $
// $Date: 3/23/05 2:55p $
// $Revision: 4 $
//
//-----------------------------------------------------------------------------
// Copyright 2013, Cypress Semiconductor Corporation
//-----------------------------------------------------------------------------
//
//Project Objective
//	This project illustrates the configuration of the FX2LP to accept bulk data from 
//	the host and loop it back to the host
//
//Overview
//	This project demonstrates a bulkloop operation using the FX2LP. The project 
//	illustrates the configuration of the endpoints and the interface to carry out 
//	the bulkloop operation. Two endpoints are configured to handle bulk transfer, 
//	one OUT endpoint and one IN endpoint. Data sent from the host is stored in an 
//	OUT endpoint. This data is transferred to the IN endpoint and then sent back to 
//	the host on request. 
//
//Operation:
//	The descriptor file for this project defines two endpoints to the host. 
//	BULK Endpoint 2-OUT receives host data.
//	BULK Endpoint 6-IN sends the same data back to the host.
//	These endpoints are configured in TD_Init using the EP2CFG and EP6CFG registers.
//  Both endpoints are set for double-buffering, using two 1024-byte FIFOS. 
//	The IFCONFIG register is not written, so it uses the default 8051 PORTS mode.
//
//	Once configured, the OUT endpoints are "armed" to accept data from the host. 
//	An OUT endpoint is said to be armed if it is ready to accept data from the host. 
//	Each endpoint is configured as double buffered. 
//
//	The function TD_Poll is where the bulkloop operation takes place.
//
//	If data is available with endpoint 2 and if endpoint 6 is ready to accept 
//	new data, the data is transferred to endpoint 6 and it is committed by writing
//  the number of bytes to the byte count registers. Endpoint 2 is then rearmed to 
//	accept a new packet from the host
//
//Code Snippets:
//
//	Descriptor:
//	    The interface descriptor defines the number of endpoints to the host. 
//		The two BULK endpoint descriptors report EP2-OUT and EP6-IN for the host. 
//		The maximum packet size is reported as 512 bytes. 
//		Descriptors are included for both high-speed and full-speed configurations. 
//
//	Initialization:
//		FX2LP is initialized in the TD_Init function, which is called once at startup.
//		The two endpoints defined in the descriptor file are be configured using these statements:
//
//		EP2CFG = 0xA2;	// EP2 is BULK-OUT, double-buffered
//		SYNCDELAY;                    
//		EP6CFG = 0xE2;	// EP6 is BULK-IN, double-buffered	
//		SYNCDELAY;                    
//		
//		Writing to these registers typically takes more than 2 clock cycles needed for a MOVX instruction. 
//		Hence the SYNCDELAY macro is used. The list of registers which need this delay 
//		function when writing to it is given in the TRM
//
//		The OUT endpoints, once configured, need to be armed to accept packets from the host. 
//		Since they are double buffered, they need to be armed by writing their byte count twice. 
//
//		By writing a 1 to bit7 of the byte count register the packet is made available to the 8051
//		instead of automatically routing it to the output FIFO. This is called a "skip".  
// 
//		EP2BCL = 0x80;                // arm EP2OUT by writing byte count w/skip.
//		SYNCDELAY;                    
//		EP2BCL = 0x80;
//		SYNCDELAY;                    
//
//		AUTOPTRSETUP |= 0x01;
//		This enables the AUTO pointer used for data transfer in the TD_Poll function. 
//
//	Enumeration:
//		Every time the FX2LP receives a setup command request, an interrupt is triggered where the 
//		GotSUD flag is asserted. The Setup Command function services various set up requests from the host. 
//		The Set up Command is executed through a switch case where the information desired by 
//		the host is serviced. 	  
//		Summing up, the enumeration process is done by repeated calling of the function SetupCommand().
//
//	Bulk Loop Implementation:
//		The bulk loop implementation is carried out in the TD_Poll function which is called repeatedly 
//		during device operation. 
//
//		Endpoint 2 is armed to accept data from the host. This data is transferred to endpoint 6.
//      To implement this, first, endpoint 2 is checked if it has data. 
//		This is done by reading the endpoint 2 empty bit in the endpoint status register (EP2468STAT). 
//		If endpoint 2 has data (that is sent from the host), the capability of endpoint 6 to receive 
//		the data is checked. This is done by reading the endpoint 6 Full bit in the endpoint status register. 
//		If endpoint 6 is not full, then the data is transferred.
//
//
//		Data Transfer implementation:
//			The data pointers are initialized to the corresponding buffers.
//			The first auto-pointer is initialized to the first byte of the endpoint 2 FIFO buffer. 
//			The second auto-pointer is initialized to the first byte of the endpoint 6 FIFO buffer. 
//			
//			The number of bytes to be transferred is read from the byte count registers of Endpoint 2.
//			The registers EP2BCL, EP2BCH contain the number of bytes written into the FIFO buffer 
//			by the host. These two registers give the byte count of the data transferred to the FIFO 
//			in an OUT transaction as long as the data is not committed to the peripheral side (Skip bit set). 
//
//			This data pointer initialization and loading of the count is done in the following statements. 
//			APTR1H = MSB( &EP2FIFOBUF );    // Initializing the first data pointer
//			APTR1L = LSB( &EP2FIFOBUF );
//
//			AUTOPTRH2 = MSB( &EP6FIFOBUF ); // Initializing the second data pointer        
//			AUTOPTRL2 = LSB( &EP6FIFOBUF );
//
//			count = (EP2BCH << 8) + EP2BCL; // The count value is loaded from the byte 
//                                			// count registers
//
//			The data transfer is carried out by the execution of the loop below. 
//
//			for( i = 0x0000; i < count; i++ )
//		    {
//			   // setup to transfer EP2OUT buffer to EP6IN buffer using AUTOPOINTER(s)
//			   EXTAUTODAT2 = EXTAUTODAT1;
//		    }
//
//			As auto pointers have been enabled, the pointers increment automatically, and the statement 
//
//			EXTAUTODAT2 = EXTAUTODAT1;
//
//			transfers data from endpoint 2 to endpoint 6. Each time the above statement is executed 
//			each auto pointer is incremented. The above statement repeatedly executes to 
//			transfer each byte from endpoint 2 to 6. 
// 			Once the data is transferred, endpoint 2 has to be "re-armed" to accept a new packet from the host. 
//			Endpoint 6 has to be "committed", that is, make the FIFO buffers available to the host for reading 
//			data from the Endpoint 6.
//
//			This is accomplished by the following statements. 
//
//			EP6BCH = EP2BCH;  
//			SYNCDELAY;  
//			EP6BCL = EP2BCL;        // commit EP6IN by specifying the number of bytes the host can read from EP6
//			SYNCDELAY;                    
//			EP2BCL = 0x80;          // arm EP2OUT w. SKIP bit set.
//
//----------------------------------------------------------------------------
// Code below
//----------------------------------------------------------------------------

#pragma NOIV               // Do not generate interrupt vectors

#include "fx2.h"
#include "fx2regs.h"
#include "syncdly.h"            // SYNCDELAY macro

extern BOOL GotSUD;             // Received setup data flag
extern BOOL Sleep;
extern BOOL Rwuen;
extern BOOL Selfpwr;

BYTE Configuration;             // Current configuration
BYTE AlternateSetting;          // Alternate settings

int start_7_seg_display = 0;  //This variable is used to start 7-seg display after the FX2LP is enumerated.
void EZUSB_INITI2C();

#define VR_NAKALL_ON    0xD0
#define VR_NAKALL_OFF   0xD1
#define bmEP2IRQ		0x10
#define bmEP6IRQ		0x40	
// 7-segment readout
#define LED_ADDR		0x21

// Provide access to FX2LP DVK LED's D2-D5. An LED is turned 
// on or off by doing a read to one of these memory addresses.  
xdata volatile BYTE D5OFF _at_ 0xB000;
xdata volatile BYTE D4OFF _at_ 0xA000;
xdata volatile BYTE D3OFF _at_ 0x9000;
xdata volatile BYTE D2OFF _at_ 0x8000;
xdata volatile BYTE D5ON  _at_ 0xB800;
xdata volatile BYTE D4ON  _at_ 0xA800;
xdata volatile BYTE D3ON  _at_ 0x9800;
xdata volatile BYTE D2ON  _at_ 0x8800;

int LEDCounter = 0;			// Activity LED blink period
BYTE inblink = 0x00;		// IN transfer LED time constant
BYTE outblink = 0x00;		// OUT transfer LED time constant

// 7-segment readout digits 0-F 
BYTE xdata Digit[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x98, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e };

//-----------------------------------------------------------------------------
// Task Dispatcher hooks
//   The following hooks are called by the task dispatcher.
//-----------------------------------------------------------------------------

void TD_Init(void)             // Called once at startup
{
   BYTE dum;					// For the LEDS
   CPUCS = ((CPUCS & ~bmCLKSPD) | bmCLKSPD1) ;	// 48 MHz CPU clock
   
   
   // Turn off all 4 LEDS
   dum = D2OFF;
   dum = D3OFF;
   dum = D4OFF;
   dum = D5OFF;

   OEA = 0x01 << 7;
   PA7 = 0;

// EP2CFG & EP6CFG configure our two endpoints, EP2-OUT and EP6-IN
// b7:		Valid
// b6:		DIR (0=OUT, 1=IN)
// b[5:4]	Type (01=ISO, 10=BULK, 11=INT)
// b3:		Size (0=512, 1=1024 bytes)
// b2:		0
// b[1:0]	Buffering (00=quad, 10=double, 11=triple)		
//

  EP2CFG = 0xA2;	// Valid, BULK-OUT, 512 byte buffer, double-buffered
  SYNCDELAY;		// Some regs take longer to update, see TRM Section 15.14.                    
  EP6CFG = 0xE2;	// Valid, BULK-IN, 512 byte buffer, double-buffered
  SYNCDELAY;                    

  // OUT endpoints do not come up armed
  // Since the endpoint is double buffered we must write dummy byte counts twice
  EP2BCL = 0x80;  	// arm EP2OUT by writing byte count w/skip.
  SYNCDELAY;                    
  EP2BCL = 0x80;	// again
  SYNCDELAY;                    
  // enable dual autopointer feature
  AUTOPTRSETUP |= 0x01;

  USBIE |= bmSOF;				// Enable the SOF IRQ to serve as LED timers
  EPIE = bmEP6IRQ | bmEP2IRQ;	// Enable EP6 and EP2 Interrupts to turn on transfer LEDS
}

void TD_Poll(void)              // Called repeatedly while the device is idle
{
  WORD i;
  WORD count;
//  BYTE dummy_LED2;		// ***For the LED
  BYTE waiting_inpkts;

#ifdef ENABLE_7_SEG_DISPLAY
if(start_7_seg_display)
{
// update 7-seg readout with number of IN packets waiting for transfer to the host
  waiting_inpkts = (EP6CS & 0xF0)>>4;
  EZUSB_WriteI2C(LED_ADDR, 0x01, &(Digit[waiting_inpkts]));
  EZUSB_WaitForEEPROMWrite(LED_ADDR);
}
#endif

// Transfer EP6-OUT buffer to EP2-IN buffer when there is a packet in one of the EP6-OUT buffers, AND
// there is an available EP2-IN buffer. The FIFO status flags update after full packets are transferred.
// Therefore EP2-OUT "Not Empty" means a packet is available, and "EP6-IN "Not Full" means there is an
// available buffer. Using the flags this way handles any packet size and takes multiple buffering
// into account.   

	if(!(EP2468STAT & bmEP2EMPTY))		// Is EP2-OUT buffer not empty (has at least one packet)?
	{
		if(!(EP2468STAT & bmEP6FULL))	// YES: Is EP6-IN buffer not full (room for at least 1 pkt)?
     	{ 
        APTR1H = MSB( &EP2FIFOBUF );
        APTR1L = LSB( &EP2FIFOBUF );
        AUTOPTRH2 = MSB( &EP6FIFOBUF );
        AUTOPTRL2 = LSB( &EP6FIFOBUF );

        count = (EP2BCH << 8) + EP2BCL;

        // loop EP2OUT buffer data to EP6IN
        for( i = 0; i < count; i++ )
        {
            EXTAUTODAT2 = EXTAUTODAT1;	// Autopointers make block transfers easy...
        }
        EP6BCH = EP2BCH;		// Send the same number of bytes as received  
        SYNCDELAY;  
        EP6BCL = EP2BCL;        // arm EP6IN
        SYNCDELAY;                    
        EP2BCL = 0x80;          // arm EP2OUT

        PA7 = ~PA7;
     }
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
   return(TRUE);
}

BOOL DR_SetConfiguration(void)   // Called when a Set Configuration command is received
{
   Configuration = SETUPDAT[2];
#ifdef ENABLE_7_SEG_DISPLAY

  EZUSB_INITI2C();				// for the 7-seg readout
  start_7_seg_display = 1;

#endif
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
   BYTE dum;
   BYTE Led_Flag = 0;
   EZUSB_IRQ_CLEAR();
   USBIRQ = bmSOF;          	// Clear SOF IRQ

   	if(--inblink == 0)			// IN and OUT LED extinguishers
		dum = D2OFF;
	if(--outblink == 0)	
		dum = D3OFF;

   // *** Blink the LED with one second period. At High-Speed the
   // microSOF's occur every 125 usec. Flash the LED 8 times per sec for High-Speed,
   // 1 per sec for Full-Speed. 

   LEDCounter++;
   if(LEDCounter > 1000)
    	LEDCounter = 0;		// 1 sec period
   		if(LEDCounter < 500)
			dum = D5OFF;
		else
			dum = D5ON;
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
	BYTE dum;   	
	EZUSB_IRQ_CLEAR();
   	EPIRQ = bmEP2IRQ;         // Clear IRQ
	dum = D3ON;
	outblink = 200;
}
void ISR_Ep4inout(void) interrupt 0
{
}
void ISR_Ep6inout(void) interrupt 0
{
	BYTE dum;
	EZUSB_IRQ_CLEAR();
   	EPIRQ = bmEP6IRQ;         // Clear IRQ 	dum = D2OFF;
	dum = D2ON;
	inblink = 200;
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
