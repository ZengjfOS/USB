#pragma NOIV                    // Do not generate interrupt vectors
//-----------------------------------------------------------------------------
//   File:      slave.c
//   Contents:  Hooks required to implement USB peripheral function.
//              Code written for FX2 REVE 56-pin and above.
//              This firmware is used to demonstrate FX2 Slave FIF
//              operation.
//   Copyright (c) 2003 Cypress Semiconductor All rights reserved
//-----------------------------------------------------------------------------
#include "fx2.h"
#include "fx2regs.h"
#include "fx2sdly.h"            // SYNCDELAY macro

#define LED_ALL         (bmBIT0 | bmBIT1 | bmBIT2 | bmBIT3)

// 7-segment readout
#define LED_ADDR		0x21
BYTE xdata Digit[] = { 0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x98, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e };

extern BOOL GotSUD;             // Received setup data flag
extern BOOL Sleep;
extern BOOL Rwuen;
extern BOOL Selfpwr;

BYTE Configuration;             // Current configuration
BYTE AlternateSetting;          // Alternate settings
static WORD xdata LED_Count = 0;
static BYTE xdata LED_Status = 0;
static WORD xdata Blink_Rate = 30000;

#define EP2EMPTY 0x02
#define EP6FULL  0x01

// EZUSB FX2 PORTA = slave fifo enable(s), when IFCFG[1:0]=11
//sbit PA0 = IOA ^ 0;             // alt. func., INT0#
//sbit PA1 = IOA ^ 1;             // alt. func., INT1#
// sbit PA2 = IOA ^ 2;          // is SLOE
//sbit PA3 = IOA ^ 3;             // alt. func., WU2
// sbit PA4 = IOA ^ 4;          // is FIFOADR0
// sbit PA5 = IOA ^ 5;          // is FIFOADR1
// sbit PA6 = IOA ^ 6;          // is PKTEND
// sbit PA7 = IOA ^ 7;          // is FLAGD

// EZUSB FX2 PORTC i/o...       port NA for 56-pin FX2
// sbit PC0 = IOC ^ 0;
// sbit PC1 = IOC ^ 1;
// sbit PC2 = IOC ^ 2;
// sbit PC3 = IOC ^ 3;
// sbit PC4 = IOC ^ 4;
// sbit PC5 = IOC ^ 5;
// sbit PC6 = IOC ^ 6;
// sbit PC7 = IOC ^ 7;

// EZUSB FX2 PORTB = FD[7:0], when IFCFG[1:0]=11
// sbit PB0 = IOB ^ 0;
// sbit PB1 = IOB ^ 1;
// sbit PB2 = IOB ^ 2;
// sbit PB3 = IOB ^ 3;
// sbit PB4 = IOB ^ 4;
// sbit PB5 = IOB ^ 5;
// sbit PB6 = IOB ^ 6;
// sbit PB7 = IOB ^ 7;

// EZUSB FX2 PORTD = FD[15:8], when IFCFG[1:0]=11 and WORDWIDE=1
//sbit PD0 = IOD ^ 0;
//sbit PD1 = IOD ^ 1;
//sbit PD2 = IOD ^ 2;
//sbit PD3 = IOD ^ 3;
//sbit PD4 = IOD ^ 4;
//sbit PD5 = IOD ^ 5;
//sbit PD6 = IOD ^ 6;
//sbit PD7 = IOD ^ 7;

// EZUSB FX2 PORTE is not bit-addressable...

//-----------------------------------------------------------------------------
// Task Dispatcher hooks
// The following hooks are called by the task dispatcher.
//-----------------------------------------------------------------------------
void LED_Off (BYTE LED_Mask);
void LED_On (BYTE LED_Mask);
void LED_Control();

BOOL Header_insert = FALSE; // insert header

//-----------------------------------------------------------------------------
// Task Dispatcher hooks
//   The following hooks are called by the task dispatcher.
//-----------------------------------------------------------------------------
void TD_Init( void )
{ // Called once at startup

  CPUCS = 0x12; // CLKSPD[1:0]=10, for 48MHz operation, output CLKOUT

  IFCONFIG = 0xCF; // for async? for sync?

  #ifdef Seven_segment
  EZUSB_INITI2C();				// initialize I2C for 7-seg readout
  Blink_Rate = 1000; 
  #endif  


  // IFCLKSRC=1   , FIFOs executes on external clk source 
  // xMHz=1       , don't care since IFCLKSRC=0
  // IFCLKOE=0    , Don't drive IFCLK pin signal at 48MHz
  // IFCLKPOL=0   , (Don't) invert IFCLK pin signal from internal clk
  // ASYNC=1      , master samples synchronous
  // GSTATE=1     , Don't drive GPIF states out on PORTE[2:0], debug WF
  // IFCFG[1:0]=11, FX2 in slave FIFO mode


  // Registers which require a synchronization delay, see section 15.14
  // FIFORESET        FIFOPINPOLAR
  // INPKTEND         OUTPKTEND
  // EPxBCH:L         REVCTL
  // GPIFTCB3         GPIFTCB2
  // GPIFTCB1         GPIFTCB0
  // EPxFIFOPFH:L     EPxAUTOINLENH:L
  // EPxFIFOCFG       EPxGPIFFLGSEL
  // PINFLAGSxx       EPxFIFOIRQ
  // EPxFIFOIE        GPIFIRQ
  // GPIFIE           GPIFADRH:L
  // UDMACRCH:L       EPxGPIFTRIG
  // GPIFTRIG
  
  // Note: The pre-REVE EPxGPIFTCH/L register are affected, as well...
  //      ...these have been replaced by GPIFTC[B3:B0] registers

  // EP2 512 BULK OUT 4x
  SYNCDELAY;                    // see TRM section 15.14
  EP2CFG = 0xA0;                // BUF[1:0]=00 for 4x buffering
  
  // EP6 512 BULK IN 4x
  SYNCDELAY;                    // 
  EP6CFG = 0xE0;                // BUF[1:0]=00 for 4x buffering
  
  // EP4 and EP8 are not used in this implementation...
  SYNCDELAY;                    // 
  EP4CFG = 0x20;                // clear valid bit
  SYNCDELAY;                    // 
  EP8CFG = 0x60;                // clear valid bit


  SYNCDELAY;
  FIFORESET = 0x80;             // activate NAK-ALL to avoid race conditions
  SYNCDELAY;                    // see TRM section 15.14
  FIFORESET = 0x02;             // reset, FIFO 2
  SYNCDELAY;                    // 
  FIFORESET = 0x04;             // reset, FIFO 4
  SYNCDELAY;                    // 
  FIFORESET = 0x06;             // reset, FIFO 6
  SYNCDELAY;                    // 
  FIFORESET = 0x08;             // reset, FIFO 8
  SYNCDELAY;                    // 
  FIFORESET = 0x00;             // deactivate NAK-ALL


  // handle the case where we were already in AUTO mode...
  // ...for example: back to back firmware downloads...
  SYNCDELAY;                    // 
  EP2FIFOCFG = 0x00;            // AUTOOUT=0, 8 bit data bus
  
  // core needs to see AUTOOUT=0 to AUTOOUT=1 switch to arm endp's
  
  SYNCDELAY;                    // 
  EP2FIFOCFG = 0x10;            // AUTOOUT=1, 8 bit data bus
  
  SYNCDELAY;                    // 
  EP6FIFOCFG = 0x0C;            // AUTOIN=1, ZEROLENIN=1, WORDWIDE=1

  SYNCDELAY;

  PINFLAGSAB = 0xE0;			// FLAGA - indexed, FLAGB - EP6FF
  SYNCDELAY;
  PINFLAGSCD = 0x08;			// FLAGC - EP2EF, FLAGD - indexed
  SYNCDELAY;
  
  PORTCCFG =0x00;                 // Write 0x00 to PORTCCFG to configure it as an I/O port
  OEC = 0x04;                     // Configure PC2 as output	
  PC2=0;					      // initialze PC2 state to "low"		 
}

void TD_Poll( void )
{ // Called repeatedly while the device is idle
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

  // ...nothing to do... slave fifo's are in AUTO mode...

  PC2=1; // asserting it high to show that the Slave Firmware has started running

}

BOOL TD_Suspend( void )          
{ // Called before the device goes into suspend mode
   return( TRUE );
}

BOOL TD_Resume( void )          
{ // Called after the device resumes
   return( TRUE );
}

//-----------------------------------------------------------------------------
// Device Request hooks
//   The following hooks are called by the end point 0 device request parser.
//-----------------------------------------------------------------------------
BOOL DR_GetDescriptor( void )
{
   return( TRUE );
}

BOOL DR_SetConfiguration( void )   
{ // Called when a Set Configuration command is received
  
  if( EZUSB_HIGHSPEED( ) )   //usb high-speed mode
  { // ...FX2 in high speed mode
    EP6AUTOINLENH = 0x02;
    SYNCDELAY;
    EP8AUTOINLENH = 0x02;   // set core AUTO commit len = 512 bytes
    SYNCDELAY;
    EP6AUTOINLENL = 0x00;
    SYNCDELAY;
    EP8AUTOINLENL = 0x00;
  }
  else
  { // ...FX2 in full speed mode
    EP6AUTOINLENH = 0x00;
    SYNCDELAY;
    EP8AUTOINLENH = 0x00;   // set core AUTO commit len = 64 bytes
    SYNCDELAY;
    EP6AUTOINLENL = 0x40;
    SYNCDELAY;
    EP8AUTOINLENL = 0x40;
  }
      
  Configuration = SETUPDAT[ 2 ];
  return( TRUE );        // Handled by user code
}

BOOL DR_GetConfiguration( void )   
{ // Called when a Get Configuration command is received
   EP0BUF[ 0 ] = Configuration;
   EP0BCH = 0;
   EP0BCL = 1;
   return(TRUE);          // Handled by user code
}

BOOL DR_SetInterface( void )       
{ // Called when a Set Interface command is received
   AlternateSetting = SETUPDAT[ 2 ];
   return( TRUE );        // Handled by user code
}

BOOL DR_GetInterface( void )       
{ // Called when a Set Interface command is received
   EP0BUF[ 0 ] = AlternateSetting;
   EP0BCH = 0;
   EP0BCL = 1;
   return( TRUE );        // Handled by user code
}

BOOL DR_GetStatus( void )
{
   return( TRUE );
}

BOOL DR_ClearFeature( void )
{
   return( TRUE );
}

BOOL DR_SetFeature( void )
{
   return( TRUE );
}

#define VX_B7 0xB2
#define VX_B8 0xB3

BOOL DR_VendorCmnd( void )
{
 switch (SETUPDAT[1])
  {
 case VX_B7: // read GPIFTRIG register

			  
		FIFORESET = 0x80;
		SYNCDELAY;
		FIFORESET = 0x02;
		SYNCDELAY;
		FIFORESET = 0x00;
		SYNCDELAY;
		EP0BUF[0] = EP68FIFOFLGS;
		EP0BUF[1] = EP2FIFOBCL;
		EP0BCH = 0;
		EP0BCL = 2;
		EP0CS |= bmHSNAK;	
	
		Header_insert = TRUE;
		break;

	case VX_B8:
		EP0BUF[0] = EP68FIFOFLGS;
		EP0BUF[1] = EP2FIFOBCL;
		EP0BCH = 0;
		EP0BCL = 2;
		EP0CS |= bmHSNAK;	
		break;
   default:
   return( TRUE );
  }
}

//-----------------------------------------------------------------------------
// USB Interrupt Handlers
//   The following functions are called by the USB interrupt jump table.
//-----------------------------------------------------------------------------

// Setup Data Available Interrupt Handler
void ISR_Sudav( void ) interrupt 0
{
   GotSUD = TRUE;         // Set flag
   EZUSB_IRQ_CLEAR( );
   USBIRQ = bmSUDAV;      // Clear SUDAV IRQ
}

// Setup Token Interrupt Handler
void ISR_Sutok( void ) interrupt 0
{
   EZUSB_IRQ_CLEAR( );
   USBIRQ = bmSUTOK;      // Clear SUTOK IRQ
}

void ISR_Sof( void ) interrupt 0
{
   EZUSB_IRQ_CLEAR( );
   USBIRQ = bmSOF;        // Clear SOF IRQ
}

void ISR_Ures( void ) interrupt 0
{
   if ( EZUSB_HIGHSPEED( ) )      //usb high-speed mode
   {
      pConfigDscr = pHighSpeedConfigDscr;
      pOtherConfigDscr = pFullSpeedConfigDscr;
   }
   else
   {
      pConfigDscr = pFullSpeedConfigDscr;
      pOtherConfigDscr = pHighSpeedConfigDscr;
   }
   
   EZUSB_IRQ_CLEAR( );
   USBIRQ = bmURES;       // Clear URES IRQ
}

void ISR_Susp( void ) interrupt 0
{
   Sleep = TRUE;
   EZUSB_IRQ_CLEAR( );
   USBIRQ = bmSUSP;
}

void ISR_Highspeed( void ) interrupt 0
{
   if ( EZUSB_HIGHSPEED( ) )		//usb high-speed mode
   {
      pConfigDscr = pHighSpeedConfigDscr;
      pOtherConfigDscr = pFullSpeedConfigDscr;
   }
   else
   {
      pConfigDscr = pFullSpeedConfigDscr;
      pOtherConfigDscr = pHighSpeedConfigDscr;
   }

   EZUSB_IRQ_CLEAR( );
   USBIRQ = bmHSGRANT;
}
void ISR_Ep0ack( void ) interrupt 0
{
}
void ISR_Stub( void ) interrupt 0
{
}
void ISR_Ep0in( void ) interrupt 0
{
}
void ISR_Ep0out( void ) interrupt 0
{
}
void ISR_Ep1in( void ) interrupt 0
{
}
void ISR_Ep1out( void ) interrupt 0
{
}
void ISR_Ep2inout( void ) interrupt 0
{
}
void ISR_Ep4inout( void ) interrupt 0
{
}
void ISR_Ep6inout( void ) interrupt 0
{
}
void ISR_Ep8inout( void ) interrupt 0
{
}
void ISR_Ibn( void ) interrupt 0
{
}
void ISR_Ep0pingnak( void ) interrupt 0
{
}
void ISR_Ep1pingnak( void ) interrupt 0
{
}
void ISR_Ep2pingnak( void ) interrupt 0
{
}
void ISR_Ep4pingnak( void ) interrupt 0
{
}
void ISR_Ep6pingnak( void ) interrupt 0
{
}
void ISR_Ep8pingnak( void ) interrupt 0
{
}
void ISR_Errorlimit( void ) interrupt 0
{
}
void ISR_Ep2piderror( void ) interrupt 0
{
}
void ISR_Ep4piderror( void ) interrupt 0
{
}
void ISR_Ep6piderror( void ) interrupt 0
{
}
void ISR_Ep8piderror( void ) interrupt 0
{
}
void ISR_Ep2pflag( void ) interrupt 0
{
}
void ISR_Ep4pflag( void ) interrupt 0
{
}
void ISR_Ep6pflag( void ) interrupt 0
{
}
void ISR_Ep8pflag( void ) interrupt 0
{
}
void ISR_Ep2eflag( void ) interrupt 0
{
}
void ISR_Ep4eflag( void ) interrupt 0
{
}
void ISR_Ep6eflag( void ) interrupt 0
{
}
void ISR_Ep8eflag( void ) interrupt 0
{
}
void ISR_Ep2fflag( void ) interrupt 0
{
}
void ISR_Ep4fflag( void ) interrupt 0
{
}
void ISR_Ep6fflag( void ) interrupt 0
{
}
void ISR_Ep8fflag( void ) interrupt 0
{
}
void ISR_GpifComplete( void ) interrupt 0
{
}
void ISR_GpifWaveform( void ) interrupt 0
{
}

// ...debug LEDs: accessed via movx reads only ( through CPLD )
// it may be worth noting here that the default monitor loads at 0xC000
xdata volatile const BYTE LED0_ON  _at_ 0x8800;
xdata volatile const BYTE LED0_OFF _at_ 0x8000;
xdata volatile const BYTE LED1_ON  _at_ 0x9800;
xdata volatile const BYTE LED1_OFF _at_ 0x9000;
xdata volatile const BYTE LED2_ON  _at_ 0xA800;
xdata volatile const BYTE LED2_OFF _at_ 0xA000;
xdata volatile const BYTE LED3_ON  _at_ 0xB800;
xdata volatile const BYTE LED3_OFF _at_ 0xB000;
// use this global variable when (de)asserting debug LEDs...
BYTE xdata ledX_rdvar = 0x00;
BYTE xdata LED_State = 0;
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

//This function controls the state of D4 and D5 LEDs on the Slave FX2LP DVK based upon the state of EP2 and EP6 FIFOs. 
//Also it blinks LED D2 while the firmware on the device is running
void LED_Control()
{
if (!( EP24FIFOFLGS & EP2EMPTY ))		//LED D4 turns on whenever EP2 has got data to transfer to Master i.e. EP2 is not Empty
LED_On(bmBIT2);
else
LED_Off(bmBIT2);

if (!( EP68FIFOFLGS & EP6FULL ))		//LED D5 turns on whenever EP6 can accept data from Master i.e. EP6 is not Full
LED_On(bmBIT3);
else
LED_Off(bmBIT3);

//For blinking LED D2
//LED D2 blinks to indicate that firmware is running.
if (++LED_Count == Blink_Rate)		//Blink_rate=10000 for Seven_segment enabled and 30000 otherwise
  {
    if (LED_Status)
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
