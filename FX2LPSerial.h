
#ifndef _INCLUDED_FX2LPSERIAL_H
#define _INCLUDED_FX2LPSERIAL_H

#define FX2LP_SERIAL
#ifdef FX2LP_SERIAL


extern void FX2LPSerial_Init() ;

extern void
FX2LPSerial_XmitChar(char ch) reentrant;

extern void
FX2LPSerial_XmitHex1(BYTE b) ;

extern void
FX2LPSerial_XmitHex2(BYTE b) ;

extern void
FX2LPSerial_XmitHex4(WORD w) ;

extern void
FX2LPSerial_XmitString(char *str) reentrant;

#endif
#endif
