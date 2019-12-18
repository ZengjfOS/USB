;;-----------------------------------------------------------------------------
;;   File:      dscr.a51
;;   Contents:  This file contains descriptor data tables.  
;;
;;   Copyright (c) 2003 Cypress Semiconductor, Inc. All rights reserved
;;-----------------------------------------------------------------------------
   
DSCR_DEVICE   equ   1  ;; Descriptor type: Device
DSCR_CONFIG   equ   2  ;; Descriptor type: Configuration
DSCR_STRING   equ   3  ;; Descriptor type: String
DSCR_INTRFC   equ   4  ;; Descriptor type: Interface
DSCR_ENDPNT   equ   5  ;; Descriptor type: Endpoint
DSCR_DEVQUAL  equ   6  ;; Descriptor type: Device Qualifier

DSCR_DEVICE_LEN   equ   18
DSCR_CONFIG_LEN   equ    9
DSCR_INTRFC_LEN   equ    9
DSCR_ENDPNT_LEN   equ    7
DSCR_DEVQUAL_LEN  equ   10

ET_CONTROL   equ   0   ;; Endpoint type: Control
ET_ISO       equ   1   ;; Endpoint type: Isochronous
ET_BULK      equ   2   ;; Endpoint type: Bulk
ET_INT       equ   3   ;; Endpoint type: Interrupt

public      DeviceDscr, DeviceQualDscr, HighSpeedConfigDscr, FullSpeedConfigDscr, StringDscr, UserDscr

;; TGE  DSCR   SEGMENT   CODE

;;-----------------------------------------------------------------------------
;; Global Variables
;;-----------------------------------------------------------------------------

;; TGE      rseg DSCR                 ;; locate the descriptor table in on-part memory.

            cseg at 100H               ;; TODO: this needs to be changed before release
DeviceDscr:   
      db   DSCR_DEVICE_LEN      ;; Descriptor length
      db   DSCR_DEVICE          ;; Decriptor type
      dw   0002H                ;; Specification Version (BCD)
      db   00H                  ;; Device class
      db   00H                  ;; Device sub-class
      db   00H                  ;; Device sub-sub-class
      db   64                   ;; Maximum packet size
      dw  0B404H                ;; Vendor ID
      dw   0310H                ;; Product ID (Sample Device)
      dw   0000H                ;; Product version ID
      db   1                    ;; Manufacturer string index
      db   2                    ;; Product string index
      db   0                    ;; Serial number string index
      db   1                    ;; Number of configurations

DeviceQualDscr:
      db   DSCR_DEVQUAL_LEN     ;; Descriptor length
      db   DSCR_DEVQUAL         ;; Decriptor type
      dw   0002H                ;; Specification Version (BCD)
      db   00H                  ;; Device class
      db   00H                  ;; Device sub-class
      db   00H                  ;; Device sub-sub-class
      db   64                   ;; Maximum packet size
      db   1                    ;; Number of configurations
      db   0                    ;; Reserved

HighSpeedConfigDscr:   
      db   DSCR_CONFIG_LEN      ;; Descriptor length
      db   DSCR_CONFIG          ;; Descriptor type
      db   (HighSpeedConfigDscrEnd-HighSpeedConfigDscr) mod 256 ;; Total Length (LSB)
      db   (HighSpeedConfigDscrEnd-HighSpeedConfigDscr)  /  256 ;; Total Length (MSB)
      db   1                    ;; Number of interfaces
      db   1                    ;; Configuration number
      db   0                    ;; Configuration string
      db   10100000b            ;; Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
      db   50                   ;; Power requirement (div 2 ma)

;; Interface Descriptor
      db   DSCR_INTRFC_LEN      ;; Descriptor length
      db   DSCR_INTRFC          ;; Descriptor type
      db   0                    ;; Zero-based index of this interface
      db   0                    ;; Alternate setting
      db   1                    ;; Number of end points 
      db   0ffH                 ;; Interface class
      db   00H                  ;; Interface sub class
      db   00H                  ;; Interface sub sub class
      db   0                    ;; Interface descriptor string index
      
;; Endpoint Descriptor
      db   DSCR_ENDPNT_LEN      ;; Descriptor length
      db   DSCR_ENDPNT          ;; Descriptor type
      db   82H                  ;; Endpoint number, and direction
      db   ET_BULK              ;; Endpoint type
      db   00H                  ;; Maximum packet size (LSB)
      db   02H                  ;; Maximum packet size (MSB)
      db   00H                  ;; Polling interval

HighSpeedConfigDscrEnd:   

FullSpeedConfigDscr:   
      db   DSCR_CONFIG_LEN      ;; Descriptor length
      db   DSCR_CONFIG          ;; Descriptor type
      db   (FullSpeedConfigDscrEnd-FullSpeedConfigDscr) mod 256 ;; Total Length (LSB)
      db   (FullSpeedConfigDscrEnd-FullSpeedConfigDscr)  /  256 ;; Total Length (MSB)
      db   1                    ;; Number of interfaces
      db   1                    ;; Configuration number
      db   0                    ;; Configuration string
      db   10100000b            ;; Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
      db   50                   ;; Power requirement (div 2 ma)

;; Interface Descriptor
      db   DSCR_INTRFC_LEN      ;; Descriptor length
      db   DSCR_INTRFC          ;; Descriptor type
      db   0                    ;; Zero-based index of this interface
      db   0                    ;; Alternate setting
      db   2                    ;; Number of end points 
      db   0ffH                 ;; Interface class
      db   00H                  ;; Interface sub class
      db   00H                  ;; Interface sub sub class
      db   0                    ;; Interface descriptor string index
      
;; Endpoint Descriptor
      db   DSCR_ENDPNT_LEN      ;; Descriptor length
      db   DSCR_ENDPNT          ;; Descriptor type
      db   02H                  ;; Endpoint number, and direction
      db   ET_BULK              ;; Endpoint type
      db   40H                  ;; Maximum packet size (LSB)
      db   00H                  ;; Maximum packet size (MSB)
      db   00H                  ;; Polling interval

;; Endpoint Descriptor
      db   DSCR_ENDPNT_LEN      ;; Descriptor length
      db   DSCR_ENDPNT          ;; Descriptor type
      db   86H                  ;; Endpoint number, and direction
      db   ET_BULK              ;; Endpoint type
      db   40H                  ;; Maximum packet size (LSB)
      db   00H                  ;; Maximum packet size (MSB)
      db   00H                  ;; Polling interval

FullSpeedConfigDscrEnd:   

StringDscr:

StringDscr0:   
      db   StringDscr0End-StringDscr0      ;; String descriptor length
      db   DSCR_STRING
      db   09H,04H
StringDscr0End:

StringDscr1:   
      db   StringDscr1End-StringDscr1      ;; String descriptor length
      db   DSCR_STRING
      db   'C',00
      db   'y',00
      db   'p',00
      db   'r',00
      db   'e',00
      db   's',00
      db   's',00
StringDscr1End:

StringDscr2:   
      db   StringDscr2End-StringDscr2      ;; Descriptor length
      db   DSCR_STRING
      db   'E',00
      db   'Z',00
      db   '-',00
      db   'U',00
      db   'S',00
      db   'B',00
      db   ' ',00
      db   'F',00
      db   'X',00
      db   '2',00
      db   ' ',00
      db   'G',00
      db   'P',00
      db   'I',00
      db   'F',00
      db   ' ',00
      db   't',00
      db   'o',00
      db   ' ',00
      db   'E',00
      db   'x',00
      db   't',00
      db   ' ',00
      db   'F',00
      db   'I',00
      db   'F',00
      db   'O',00
      db   ' ',00
      db   'E',00
      db   'x',00
      db   'a',00
      db   'm',00
      db   'p',00
      db   'l',00
      db   'e',00
      db   ' ',00
      db   'u',00
      db   's',00
      db   'i',00
      db   'n',00
      db   'g',00
      db   ' ',00
      db   'S',00
      db   'i',00
      db   'n',00
      db   'g',00
      db   'l',00
      db   'e',00
      db   ' ',00
      db   'T',00
      db   'r',00
      db   'a',00
      db   'n',00
      db   's',00
      db   'a',00
      db   'c',00
      db   't',00
      db   'i',00
      db   'o',00
      db   'n',00
      db   's',00
StringDscr2End:

UserDscr:      
      dw   0000H
      end
      
