#pragma NOIV               // Do not generate interrupt vectors
//-----------------------------------------------------------------------------
//   File:       master.c
//   Contents:   Hooks required to implement FX2 B2B interface
//
//   Copyright (c) 2003 Cypress Semiconductor, Inc. All rights reserved
//-----------------------------------------------------------------------------
#include "fx2.h"
#include "fx2regs.h"
#include "fx2sdly.h"            // SYNCDELAY macro, see Section 15.14 of FX2 Tech.
                                // Ref. Manual for usage details.

#define SLAVENOTFULL   GPIFREADYSTAT & bmBIT1
#define SLAVENOTEMPTY  GPIFREADYSTAT & bmBIT0


#define GPIFTRIGWR 0
#define GPIFTRIGRD 4

#define GPIF_EP2 0
#define GPIF_EP4 1
#define GPIF_EP6 2
#define GPIF_EP8 3
#define EP2EMPTY 0x02
#define EP6FULL  0x01

#define LED_ALL         (bmBIT0 | bmBIT1 | bmBIT2 | bmBIT3)

// 7-segment readout
#define LED_ADDR		0x21	
BYTE xdata Digit[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x98, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e };

// EZUSB FX2 PORTA = slave fifo enable(s), when IFCFG[1:0]=11
sbit PERIPH_PA0 = IOA ^ 0;      // tied to peripheral PA0 pin
sbit PERIPH_PA1 = IOA ^ 1;      // tied to peripheral PA1 pin
sbit NA_PA2 = IOA ^ 2;          // not used (make it an output)
sbit PERIPH_PA3 = IOA ^ 3;      // tied to peripheral PA3 pin
sbit PERIPH_FLAGA = IOA ^ 4;    // tied to peripheral PF pin (programmable flag)  
sbit PERIPH_PA7 = IOA ^ 5;      // tied to peripheral FLAGD pin (not used)
sbit PERIPH_FIFOADR0 = IOA ^ 6; // tied to peripheral FIFOADR0 pin
sbit PERIPH_FIFOADR1 = IOA ^ 7; // tied to peripheral FIFOADR1 pin


// EZUSB FX2 PORTB = FD[7:0], when IFCFG[1:0]=10
// sbit PB0 = IOB ^ 0;
// sbit PB1 = IOB ^ 1;
// sbit PB2 = IOB ^ 2;
// sbit PB3 = IOB ^ 3;
// sbit PB4 = IOB ^ 4;
// sbit PB5 = IOB ^ 5;
// sbit PB6 = IOB ^ 6;
// sbit PB7 = IOB ^ 7;

// EZUSB FX2 PORTD = FD[15:8], when IFCFG[1:0]=10 and WORDWIDE=1
// sbit PD0 = IOD ^ 0;
// sbit PD1 = IOD ^ 1;
// sbit PD2 = IOD ^ 2;
// sbit PD3 = IOD ^ 3;
// sbit PD4 = IOD ^ 4;
// sbit PD5 = IOD ^ 5;
// sbit PD6 = IOD ^ 6;
// sbit PD7 = IOD ^ 7;

BOOL zerolenpkt = 0;            // issued by peripheral for zero length packet

// ...debug LEDs: accessed via movx reads only ( through CPLD )
xdata volatile const BYTE LED0_ON  _at_ 0x8800;
xdata volatile const BYTE LED0_OFF _at_ 0x8000;
xdata volatile const BYTE LED1_ON  _at_ 0x9800;
xdata volatile const BYTE LED1_OFF _at_ 0x9000;
xdata volatile const BYTE LED2_ON  _at_ 0xA800;
xdata volatile const BYTE LED2_OFF _at_ 0xA000;
xdata volatile const BYTE LED3_ON  _at_ 0xB800;
xdata volatile const BYTE LED3_OFF _at_ 0xB000;
static WORD xdata LED_Count = 0;
static BYTE xdata LED_Status = 1;
static WORD xdata Blink_Rate = 30000;

void LED_Off (BYTE LED_Mask);
void LED_On (BYTE LED_Mask);
void LED_Control ();

// use this global variable when (de)asserting debug LEDs...
BYTE ledX_rdvar = 0x00;
BYTE xdata LED_State = 0;

// it may be worth noting here that the default monitor loads at 0xC000
extern BOOL GotSUD;             // Received setup data flag
extern BOOL Sleep;
extern BOOL Rwuen;
extern BOOL Selfpwr;

BYTE Configuration;                 // Current configuration
BYTE AlternateSetting;              // Alternate settings
BOOL in_enable = TRUE;             // flag to enable IN transfers
BOOL enum_high_speed = FALSE;       // flag to let firmware know FX2 enumerated at high speed
extern const char xdata FlowStates[36];
#define SLAVEREADY	PC2   
BYTE j = 0;
//-----------------------------------------------------------------------------
// Task Dispatcher hooks
//   The following hooks are called by the task dispatcher.
//-----------------------------------------------------------------------------

void GpifInit ();

void TD_Init(void)             // Called once at startup
{
  // set the CPU clock to 48MHz
  //CPUCS = ((CPUCS & ~bmCLKSPD) | bmCLKSPD1);
  CPUCS = 0x10;                 // CLKSPD[1:0]=10, for 48MHz operation
  SYNCDELAY;                    // CLKOE=0, don't drive CLKOUT
  
 
  SYNCDELAY;
  GpifInit();                  // init GPIF engine via GPIFTool output file

  #ifdef Seven_segment  
  EZUSB_INITI2C();				// initialize I2C for 7-seg readout
  Blink_Rate = 1000;   
  #endif

  // Register which require a synchronization delay, see section 15.14
  // FIFORESET        FIFOPINPOLAR
  // INPKTEND         EPxBCH:L
  // EPxFIFOPFH:L     EPxAUTOINLENH:L
  // EPxFIFOCFG       EPxGPIFFLGSEL
  // PINFLAGSxx       EPxFIFOIRQ
  // EPxFIFOIE        GPIFIRQ
  // GPIFIE           GPIFADRH:L
  // UDMACRCH:L       EPxGPIFTRIG
  // GPIFTRIG
  

  EP2CFG = 0xA0;     // EP2OUT, bulk, size 512, 4x buffered
  SYNCDELAY;           
  EP6CFG = 0xE0;     // EP6IN, bulk, size 512, 4x buffered
  SYNCDELAY;
  EP4CFG = 0x00;     // EP4 not valid
  SYNCDELAY;               
  EP8CFG = 0x00;     // EP8 not valid
  SYNCDELAY;
  
  
  FIFORESET = 0x80;  // set NAKALL bit to NAK all transfers from host
  SYNCDELAY;
  FIFORESET = 0x02;  // reset EP2 FIFO
  SYNCDELAY;
  FIFORESET = 0x06;  // reset EP6 FIFO
  SYNCDELAY;
  FIFORESET = 0x00;  // clear NAKALL bit to resume normal operation
  SYNCDELAY;

  EP2FIFOCFG = 0x00; // allow core to see zero to one transition of auto out bit
  SYNCDELAY;
  EP2FIFOCFG = 0x10; // auto out mode, 8 bit data bus
  SYNCDELAY;
  EP6FIFOCFG = 0x08; // auto in mode, 8 bit data bus
  SYNCDELAY; 
  

  EP2GPIFFLGSEL = 0x02; // For EP2OUT, GPIF uses FF flag
  SYNCDELAY;
  EP6GPIFFLGSEL = 0x01; // For EP6IN, GPIF uses EF flag
  SYNCDELAY;
  
  // IN endp's come up in the cpu/peripheral domain
  
  PORTCCFG = 0x00;              //configure port C as an I/O port
  OEC= 0xFB;					//configure PC2 as input pin  

  OEA = 0xC0;                   // PA[7:6]=11 -> outputs (tied to peripheral FIFOADR[1:0] pins
  
  PORTACFG |= 0x01;             // setting BIT 0 to configure PORTA_0 pin as alt. func INTO#
  
  // enable INTO# external pin interrupt, used by peripheral to issue ZEROLENPKT

  EX0 = 1;                      // enable INT0# pin ISR
  IT0 = 1;                      // configure INT0# pin, edge trigger
  
  // turn debug LED[3:0] off...
 // ledX_rdvar = LED0_OFF;
 // ledX_rdvar = LED1_OFF;
 // ledX_rdvar = LED2_ON;
 // ledX_rdvar = LED3_OFF;
}

void TD_Poll(void)
{
BYTE waiting_inpkts;

// update 7-seg readout with number of IN packets in EP6 waiting for transfer to the host
#ifdef Seven_segment 
  waiting_inpkts = (EP6CS & 0xF0)>>4;			//right shift by 4 bits
  EZUSB_WriteI2C(LED_ADDR, 0x01, &(Digit[waiting_inpkts]));
  EZUSB_WaitForEEPROMWrite(LED_ADDR);
#endif

// blink LED D2 to indicate firmware is running and LED D4 and D5 to show the states of FIFO of EP2 and EP6
#ifdef LED_Enable
LED_Control();  
#endif

// Handle OUT data...

  if( GPIFTRIG & 0x80 )               // if GPIF interface IDLE
  {   
    if ( ! ( EP24FIFOFLGS & EP2EMPTY ) )  // if there's a packet in the peripheral domain for EP2
	{
	  // check if peripheral "not full"...
	  PERIPH_FIFOADR0 = 0;               // FIFOADR[1:0]=10 - point to peripheral EP6 
      PERIPH_FIFOADR1 = 1;               
      SYNCDELAY;                         // used here as "delay"

      if ( SLAVENOTFULL )           // if the slave is not full
	  {  
	    if(enum_high_speed)			//usb high-speed mode
		{
	      SYNCDELAY;    
          GPIFTCB1 = 0x02;            // setup transaction count 
          SYNCDELAY;
          GPIFTCB0 = 0x00;
          SYNCDELAY;
		}
		else
		{
		  SYNCDELAY;
		  GPIFTCB1 = 0x00;            // setup transaction count 
          SYNCDELAY;
		  GPIFTCB0 = 0x40;
		  SYNCDELAY;
		}	    

        SYNCDELAY;
        GPIFTRIG = GPIFTRIGWR | GPIF_EP2;  // launch GPIF FIFO WRITE Transaction from EP2 FIFO
        SYNCDELAY;
   
	    while( !( GPIFTRIG & 0x80 ) )      // poll GPIFTRIG.7 GPIF Done bit
        {
          ;
        }
        SYNCDELAY;
	  }
    }
  }

  
if(in_enable)
{
  // Handle IN data...
if (SLAVEREADY)
{
  if ( GPIFTRIG & 0x80 )                  // if GPIF interface IDLE
  { 
    PERIPH_FIFOADR0 = 0;
    PERIPH_FIFOADR1 = 0;              // FIFOADR[1:0]=00 - point to peripheral EP2
    SYNCDELAY;   

    if ( SLAVENOTEMPTY )                // if slave is not empty
    {
      if ( !( EP68FIFOFLGS & EP6FULL ) )     // if EP6 FIFO is not full
      {      
	  	if(enum_high_speed)				//usb high-speed mode
	    {
	      SYNCDELAY;    
          GPIFTCB1 = 0x02;                // setup transaction count 
          SYNCDELAY;
          GPIFTCB0 = 0x00;
          SYNCDELAY;
		  j++;
	    }
	    else
	    {
	      SYNCDELAY;
	      GPIFTCB1 = 0x00;                // setup transaction count 
          SYNCDELAY;
	      GPIFTCB0 = 0x40;
	      SYNCDELAY;
	    }

        GPIFTRIG = GPIFTRIGRD | GPIF_EP6; // launch GPIF FIFO READ Transaction to EP6 FIFO
        SYNCDELAY;
        while( !( GPIFTRIG & 0x80 ) )     // poll GPIFTRIG.7 GPIF Done bit
        {
          ;
        }
        SYNCDELAY;
	  }
    }
  }
  }
  else{
  FIFORESET = 0x80;  // set NAKALL bit to NAK all transfers from host
  SYNCDELAY;
  FIFORESET = 0x06;  // reset EP6 FIFO
  SYNCDELAY;
  FIFORESET = 0x00;  
  SYNCDELAY;
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
  if( EZUSB_HIGHSPEED( ) )		//usb high-speed mode
  { // FX2 enumerated at high speed
    SYNCDELAY;                  // 
    EP6AUTOINLENH = 0x02;       // set AUTOIN commit length to 512 bytes
    SYNCDELAY;                  // 
    EP6AUTOINLENL = 0x00;
    SYNCDELAY;                  
    enum_high_speed = TRUE;
  }
  else
  { // FX2 enumerated at full speed
    SYNCDELAY;                   
    EP6AUTOINLENH = 0x00;       // set AUTOIN commit length to 64 bytes
    SYNCDELAY;                   
    EP6AUTOINLENL = 0x40;
    SYNCDELAY;                  
    enum_high_speed = FALSE;
  }

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

#define VX_B2 0xB2 // your vendor command here
#define VX_B3 0xB3 // enable IN transfers
#define VX_B4 0xB4 // disable IN transfers
#define VX_B5 0xB5 // read GPIFREADYSTAT register
#define VX_B6 0xB6 // read GPIFTRIG register

BOOL DR_VendorCmnd(void)
{
  switch (SETUPDAT[1])
  { case 0xBB:
    { 
      // your vendor command here

      *EP0BUF = j;
	  EP0BCH = 0;
	  EP0BCL = 1;                   // Arm endpoint with # bytes to transfer
	  EP0CS |= bmHSNAK;             // Acknowledge handshake phase of device request
      break;
    }
    case VX_B2:
    { 
      // your vendor command here

      *EP0BUF = VX_B2;
	  EP0BCH = 0;
	  EP0BCL = 1;                   // Arm endpoint with # bytes to transfer
	  EP0CS |= bmHSNAK;             // Acknowledge handshake phase of device request
      break;
    }
	case VX_B3: // enable IN transfers
	{
	  in_enable = TRUE;

      *EP0BUF = VX_B3;
  	  EP0BCH = 0;
	  EP0BCL = 1;
	  EP0CS |= bmHSNAK;
	  break;
    }
	case VX_B4: // disable IN transfers
	{
	  in_enable = FALSE;

      *EP0BUF = VX_B4;
  	  EP0BCH = 0;
	  EP0BCL = 1;
	  EP0CS |= bmHSNAK;
	  break;
    }
	case VX_B5: // read GPIFREADYSTAT register
	{	  
      EP0BUF[0] = VX_B5;
	  SYNCDELAY;
	  EP0BUF[1] = GPIFREADYSTAT;
 	  SYNCDELAY;
  	  EP0BCH = 0;
	  EP0BCL = 2;
	  EP0CS |= bmHSNAK;
	  break;
    }
    case VX_B6: // read GPIFTRIG register
	{	  
      EP0BUF[0] = VX_B6;
	  SYNCDELAY;
	  EP0BUF[1] = GPIFTRIG;
 	  SYNCDELAY;
  	  EP0BCH = 0;
	  EP0BCL = 2;
	  EP0CS |= bmHSNAK;
	  break;
    }
     default:
        return(TRUE);
  }

  return(FALSE);
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
   if (EZUSB_HIGHSPEED())		//usb high-speed mode
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
void LED_Off (BYTE LED_Mask)
{
	if (LED_Mask & bmBIT0)
	{
		ledX_rdvar = LED0_OFF;
		LED_State &= ~bmBIT0;
	}
	if (LED_Mask & bmBIT1)
	{
		ledX_rdvar = LED1_OFF;
		LED_State &= ~bmBIT1;
	}
	if (LED_Mask & bmBIT2)
	{
		ledX_rdvar = LED2_OFF;
		LED_State &= ~bmBIT2;
	}
	if (LED_Mask & bmBIT3)
	{
		ledX_rdvar = LED3_OFF;
		LED_State &= ~bmBIT3;
	}
}

void LED_On (BYTE LED_Mask)
{
	if (LED_Mask & bmBIT0)
	{
		ledX_rdvar = LED0_ON;
		LED_State |= bmBIT0;
	}
	if (LED_Mask & bmBIT1)
	{
		ledX_rdvar = LED1_ON;
		LED_State |= bmBIT1;
	}
	if (LED_Mask & bmBIT2)
	{
		ledX_rdvar = LED2_ON;
		LED_State |= bmBIT2;
	}
	if (LED_Mask & bmBIT3)
	{
		ledX_rdvar = LED3_ON;
		LED_State |= bmBIT3;
	}
}

//This function controls the state of D4 and D5 LEDs on the Master FX2LP DVK based upon the state of EP2 and EP6 FIFOs. 
//Also it blinks LED D2 while the firmware on the device is running
void LED_Control()
{
if (!( EP24FIFOFLGS & EP2EMPTY ))		//LED D4 turns on whenever EP2 has got data to transfer to Slave i.e. EP2 is not Empty
LED_On(bmBIT2);
else
LED_Off(bmBIT2);

if (!( EP68FIFOFLGS & EP6FULL ))		//LED D5 turns on whenever EP6 can accept data from Slave i.e. EP6 is not Full
LED_On(bmBIT3);
else
LED_Off(bmBIT3);

//For blinking LED D2
//LED D2 blinks to indicate that firmware is running.
if (++LED_Count == Blink_Rate)	   //Blink_rate=10000 for Seven_segment enabled and 30000 otherwise
  {   if (LED_Status)
    {
      LED_Off (bmBIT0);
      LED_Status = 0;
    }
    else
    {
      LED_On (bmBIT0);
      LED_Status = 1;
    }
	  LED_Count = 0;
   } 
}
