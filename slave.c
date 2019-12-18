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
#include "syncdly.h"            // SYNCDELAY macro

extern BOOL GotSUD;             // Received setup data flag
extern BOOL Sleep;
extern BOOL Rwuen;
extern BOOL Selfpwr;

BYTE Configuration;             // Current configuration
BYTE AlternateSetting;          // Alternate settings

//Sensor Configuration Registers 
BYTE xdata REG01[3]={0xB3,0x00,0x10};  //R1 value{register addres,higher byte of data, lower byte of data}  default
BYTE xdata REG02[3]={0xB1,0x00,0x02};   
BYTE xdata REG03[3]={0x0C,0x00,0x01};
BYTE xdata REG04[3]={0x0C,0x00,0x00};
BYTE xdata REG05[3]={0xB5,0x00,0x01};    
BYTE xdata REG06[3]={0xB5,0x00,0x00};    
BYTE xdata REG07[3]={0x1C,0x00,0x02};
BYTE xdata REG08[3]={0x31,0x00,0x1F};
BYTE xdata REG09[3]={0x32,0x00,0x1A};
BYTE xdata REG10[3]={0x33,0x00,0x12};
BYTE xdata REG11[3]={0xAF,0x00,0x00};
BYTE xdata REG12[3]={0x2B,0x00,0x03};
BYTE xdata REG13[3]={0x10,0x00,0x40};

//Sensor Registers for configuring Resolution : 640*480
BYTE xdata REG14[3]={0x04,0x02,0x80};
//BYTE xdata REG15[3]={0x05,0x01,0xE4}; //1e4,484 
//BYTE xdata REG16[3]={0x06,0x00,0xAD}; //AD,173  

//increased Horizontal and Vertical blanking
BYTE xdata REG15[3]={0x05,0x01,0xE4}; //1e4,600 
BYTE xdata REG16[3]={0x06,0x00,0xFA}; //FA,250 

BOOL vendorCmdIssued=FALSE;			//Used to check if Utility has issued the vendor command to start/sync the transmission.

//-----------------------------------------------------------------------------
// Task Dispatcher hooks
//   The following hooks are called by the task dispatcher.
//-----------------------------------------------------------------------------
void TD_Init( void )
{ // Called once at startup

  CPUCS = 0x02;	//12MHz operation , CPUCS[4:3] = 10.
	IFCONFIG = 0x43; // Slave FIFO mode of FX2LP
   
  // EP2 512 BULK IN 4x
  SYNCDELAY;                    // see TRM section 15.14
  EP2CFG = 0xE0;                // BUF[1:0]=00 for 4x buffering:IN
  
  // EP4, EP6 and EP8 are not used in this implementation...
  SYNCDELAY;                    // 
  EP6CFG = 0x7F;                // Clear Valid bit
  
  SYNCDELAY;                    // 
  EP4CFG &= 0x7F;               // clear valid bit
  SYNCDELAY;                    // 
  
	EP8CFG &= 0x7F;               // clear valid bit

  SYNCDELAY;
  FIFORESET = 0x80;             // activate NAK-ALL to avoid race conditions
  SYNCDELAY;                    // see TRM section 15.14
  FIFORESET = 0x02;             // reset, FIFO 2
  SYNCDELAY;                     
  FIFORESET = 0x04;             // reset, FIFO 4
  SYNCDELAY;                     
  FIFORESET = 0x06;             // reset, FIFO 6
  SYNCDELAY;                     
  FIFORESET = 0x08;             // reset, FIFO 8
  SYNCDELAY;                     
  FIFORESET = 0x00;             // deactivate NAK-ALL
  
  SYNCDELAY;                     
  EP2FIFOCFG = 0x00;            // MANUAL MODE, 8 bit operation
	
	SYNCDELAY;                     
  PINFLAGSAB = 0x0C;						//EP2FF

  FIFOPINPOLAR = 0x07; // SLWR is configured as active HIGH and flag C(EP2EF) is made active high and flag B(EP2FF) is active high

  SYNCDELAY;  
  PORTACFG=0x00;
  SYNCDELAY;
  OEA=0x00;
  SYNCDELAY;
}

void TD_Poll( void )
{ // Called repeatedly while the device is idle

  if(vendorCmdIssued)     // if Vendor command issued
  {
			while(PA6);  // to insert the header when Frame Valid FV is low. So it will wait if FV is high
		      	
		  FIFORESET = 0x80; // activate NAK-ALL to avoid race conditions
			SYNCDELAY;
			EP2FIFOCFG = 0x00; //switching to manual mode
			SYNCDELAY;
			FIFORESET = 0x02; // Reset FIFO 2
			SYNCDELAY;
			FIFORESET = 0x00; //Release NAKALL
			SYNCDELAY;
			
			EP2FIFOBUF[0]=0xFF;    // 5bytes of Header used by the Preview Utility to detect the Start of Transmission
			EP2FIFOBUF[1]=0x00;
			EP2FIFOBUF[2]=0x0F;
			EP2FIFOBUF[3]=0x00;
			EP2FIFOBUF[4]=0xFF;
			
			EP2BCH=0x02; 	//512 bytes committed
			SYNCDELAY;
			EP2BCL=0x00;
			SYNCDELAY;
      
			EP2FIFOCFG = 0x08; //Switching to Auto mode
			SYNCDELAY;		

  		vendorCmdIssued=FALSE;     
			}
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
  
    if( EZUSB_HIGHSPEED( ) )
  { // ...FX2 in high speed mode

    EP2AUTOINLENH = 0x02;
    SYNCDELAY;
     
    EP2AUTOINLENL = 0x00;
    SYNCDELAY;

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

BOOL DR_VendorCmnd( void )
{
	 
	switch(SETUPDAT[1])
	{ //TPM handle new commands
		case 0xAA:					//Start Video 
		
			EZUSB_InitI2C();				// ...I2C initialization
			
			//0x48 = 0x90>>1
			//Configuring the Sensor Registers
			EZUSB_WriteI2C(0x48,0x03,REG01);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG02);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG03);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG04);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG05);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG06);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG07);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG08);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG09);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG10);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG11);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG12);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG13);EZUSB_WaitForEEPROMWrite(0x48);
					
			//Setting Sensor Resolution to 640*480
			EZUSB_WriteI2C(0x48,0x03,REG14);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG15);EZUSB_WaitForEEPROMWrite(0x48);
			EZUSB_WriteI2C(0x48,0x03,REG16);EZUSB_WaitForEEPROMWrite(0x48);
			
			vendorCmdIssued = TRUE;
			break;

		case 0xAC:				//Frame Synchronization
			vendorCmdIssued = TRUE;
			break;
	}
  	return( FALSE );  
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
   if ( EZUSB_HIGHSPEED( ) )
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
   if ( EZUSB_HIGHSPEED( ) )
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
