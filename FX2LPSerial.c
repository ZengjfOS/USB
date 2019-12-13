
#include "fx2.h"
#include "fx2regs.h"
#include "FX2LPserial.h"

void FX2LPSerial_Init()  // initializes the registers for using Timer2 as baud rate generator for a Baud rate of 38400.
{
	T2CON = 0x34 ;
	RCAP2H  = 0xFF ;
	RCAP2L = 0xD9;
	SCON0 = 0x5A ;
	TI = 1;

CPUCS = ((CPUCS & ~bmCLKSPD) | bmCLKSPD1) ;	//Setting up the clock frequency


	FX2LPSerial_XmitString("\r\n->Initialized.... \n\n") ;		//Clearing the output screen
	 
}

void FX2LPSerial_XmitChar(char ch) reentrant // prints a character
{
	while (TI == 0) ;
	TI = 0 ;
	SBUF0 = ch ;	//print the character
}

void FX2LPSerial_XmitHex1(BYTE b) // intermediate function to print the 4-bit nibble in hex format
{
	if (b < 10)
		FX2LPSerial_XmitChar(b + '0') ;
	else
		FX2LPSerial_XmitChar(b - 10 + 'A') ;
}

void FX2LPSerial_XmitHex2(BYTE b) // prints the value of the BYTE variable in hex
{
	FX2LPSerial_XmitHex1((b >> 4) & 0x0f) ;
	FX2LPSerial_XmitHex1(b & 0x0f) ;
}

void FX2LPSerial_XmitHex4(WORD w) // prints the value of the WORD variable in hex
{
	FX2LPSerial_XmitHex2((w >> 8) & 0xff) ;
	FX2LPSerial_XmitHex2(w & 0xff) ;
}

void FX2LPSerial_XmitString(char *str) reentrant
{
	while (*str)
		FX2LPSerial_XmitChar(*str++) ;
}


