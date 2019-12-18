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
DSCR_INTRFC_LEN   equ    8
DSCR_ENDPNT_LEN   equ    7
DSCR_DEVQUAL_LEN  equ   10   
CY_USB_INTRFC_DESCR equ 04H

ET_CONTROL   equ   0   ;; Endpoint type: Control
ET_ISO       equ   1   ;; Endpoint type: Isochronous
ET_BULK      equ   2   ;; Endpoint type: Bulk
ET_INT       equ   3   ;; Endpoint type: Interrupt

;;Video Class-Specific Descriptor Types
CS_UNDEFINED     equ 0x20
CS_DEVICE        equ 0x21
CS_CONFIGURATION equ 0x22
CS_STRING        equ 0x23
CS_INTERFACE     equ 0x24
CS_ENDPOINT      equ 0x25

;;VC Interface Descriptor Subtypes
VC_DESCRIPTOR_UNDEFINED equ 0x00
VC_HEADER               equ 0x01
VC_INPUT_TERMINAL       equ 0x02
VC_OUTPUT_TERMINAL      equ 0x03
VC_SELECTOR_UNIT        equ 0x04
VC_PROCESSING_UNIT      equ 0x05
VC_EXTENSION_UNIT       equ 0x06

SC_UNDEFINED                  equ 0x00
SC_VIDEOCONTROL               equ 0x01
SC_VIDEOSTREAMING             equ 0x02

EP_BULK_VIDEO  equ  82H
EP_INT_VIDEO   equ  88H

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
      db   0EFH                  ;; Device class
      db   02H                  ;; Device sub-class
      db   01H                  ;; Device sub-sub-class
      db   64                   ;; Maximum packet size
      dw   0B404H                ;; Vendor ID
      dw   3600H                ;; Product ID (Sample Device)
      dw   0000H                ;; Product version ID
      db   1                    ;; Manufacturer string index
      db   2                    ;; Product string index
      db   0                    ;; Serial number string index
      db   1                    ;; Number of configurations

DeviceQualDscr:
      db   DSCR_DEVQUAL_LEN     ;; Descriptor length
      db   DSCR_DEVQUAL         ;; Decriptor type
      dw   0002H                ;; Specification Version (BCD)
      db   0EFH                  ;; Device class
      db   02H                  ;; Device sub-class
      db   01H                  ;; Device sub-sub-class
      db   64                   ;; Maximum packet size
      db   1                    ;; Number of configurations
      db   0                    ;; Reserved

HighSpeedConfigDscr:   
      db   DSCR_CONFIG_LEN      ;; Descriptor length
      db   DSCR_CONFIG          ;; Descriptor type
      db   (HighSpeedConfigDscrEnd-HighSpeedConfigDscr) mod 256 ;; Total Length (LSB)
      db   (HighSpeedConfigDscrEnd-HighSpeedConfigDscr)  /  256 ;; Total Length (MSB)
      db   2                    ;; Number of interfaces
      db   1                    ;; Configuration number
      db   0                    ;; Configuration string
      db   10000000b            ;; Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
      db   0FAH                  ;; Power requirement (div 2 ma)

;; Interface Assoication Descriptor
      db   DSCR_INTRFC_LEN      ;; Descriptor length
      db   0BH          ;; Descriptor type
      db   0                    ;; Zero-based index of this interface
      db   2                    ;; Number of video i/f 
      db   0EH                  ;; CC_VIDEO : Video i/f class code
      db   03H                  ;; SC_VIDEO_INTERFACE_COLLECTION : Subclass code
      db   00H                  ;; Protocol : Not used - PC_PROTOCOL_UNDEFINED
      db   2                    ;; String desc index for interface

;; Standard Video Control Interface Descriptor
      db   09H                  ;; Descriptor size */
      db   CY_USB_INTRFC_DESCR  ;; Interface Descriptor type */
      db   00H                  ;; Interface number */
      db   00H                  ;; Alternate setting number */
      db   01H                  ;; Number of end points */
      db   0EH                  ;; CC_VIDEO : Interface class */
      db   01H                  ;; CC_VIDEOCONTROL : Interface sub class */
      db   00H                  ;; Interface protocol code */
      db   02H                  ;; Interface descriptor string index */

;; Class specific VC Interface Header Descriptor */
      db   0DH                  ;; Descriptor size 
      db   24H                  ;; Class Specific I/f Header Descriptor type */
      db   01H                  ;; Descriptor Sub type : VC_HEADER */
      dw   1001H               ;; Revision of class spec : 1.1 */
      dw   5100H               ;; Total Size of class specific descriptors (till Output terminal) */
      dw   006CH
      dw   0DC02H           ;; Clock frequency : 48MHz(Deprecated) */
      db   01H                  ;; Number of streaming interfaces */
      db   01H                  ;; Video streaming I/f 1 belongs to VC i/f */

;; Input (Camera) Terminal Descriptor */
      db   12H                   ;;    /* Descriptor size */
      db   24H                   ;;    /* Class specific interface desc type */
      db   02H                   ;;    /* Input Terminal Descriptor type */
      db   01H                   ;;    /* ID of this terminal */
      dw   0102H                ;;    /* Camera terminal type */
      db   00H                   ;;    /* No association terminal */
      db   00H                   ;;    /* String desc index : Not used */
      dw   00000H               ;;    /* No optical zoom supported */
      dw   00000H               ;;    /* No optical zoom supported */
      dw   00000H               ;;    /* No optical zoom supported */
      db   03H                  ;;    /* Size of controls field for this terminal : 3 bytes */
      db   00H                  ;;    /* bmControls field of camera terminal: No controls supported */
      db   00H                  ;;    /* bmControls field of camera terminal: No controls supported */   
      db   00H                  ;;    /* bmControls field of camera terminal: No controls supported */

;;  /* Processing Unit Descriptor */
      db  0DH                   ;;   /* Descriptor size */
      db  24H                   ;;   /* Class specific interface desc type */
      db  05H                   ;;   /* Processing Unit Descriptor type */
      db  02H                   ;;   /* ID of this terminal */
      db  01H                   ;;   /* Source ID : 1 : Conencted to input terminal */
      dw  0040H                ;;   /* Digital multiplier */
      db  03H                   ;;    /* Size of controls field for this terminal : 3 bytes */
      db  00H
      db  00H                   ;;    /* bmControls field of processing unit: Brightness control supported */
      db  00H                   ;;       
      db  00H                   ;;   /* String desc index : Not used */
      db  00H                   ;;   /*Analog video standards supported: None*/

;;   /* Extension Unit Descriptor */
     db 1CH                     ;;            /* Descriptor size */
     db 24H                     ;;            /* Class specific interface desc type */
     db 06H                     ;;            /* Extension Unit Descriptor type */
     db 03H                     ;;            /* ID of this terminal */
     dw 0FFFFH                  ;; 
     dw 0FFFFH                  ;;    /* 16 byte GUID */
     dw 0FFFFH
     dw 0FFFFH              ;;
     dw 0FFFFH
     dw 0FFFFH              ;;
     dw 0FFFFH
     dw 0FFFFH              ;;
     db 00H                     ;;            /* Number of controls in this terminal */
     db 01H                     ;;            /* Number of input pins in this terminal */
     db 02H                     ;;            /* Source ID : 2 : Connected to Proc Unit */
     db 03H                     ;;            /* Size of controls field for this terminal : 3 bytes */
     db 00H                     ;;
     db 00H                     ;;
     db 00H                     ;;  /* No controls supported */
     db 00H                     ;;      /* String desc index : Not used */

;;        /* Output Terminal Descriptor */
     db   09H              ;;           /* Descriptor size */
     db   24H              ;;           /* Class specific interface desc type */
     db   03H              ;;           /* Output Terminal Descriptor type */
     db   04H              ;;           /* ID of this terminal */
     dw   0101H           ;;           /* USB Streaming terminal type */
     db   00H              ;;           /* No association terminal */
     db   03H              ;;           /* Source ID : 3 : Connected to Extn Unit */
     db   00H              ;;           /* String desc index : Not used */
                           
		
;;        /* Video Control Status Interrupt Endpoint Descriptor */
     db     07H            ;;               /* Descriptor size */
     db     DSCR_ENDPNT    ;;               /* Endpoint Descriptor Type */
     db     EP_INT_VIDEO   ;;               /* Endpoint address and description */
     db     ET_INT         ;;               /* Interrupt End point Type */
     dw     4000H          ;;               /* Max packet size = 64 bytes */
     db     08H            ;;               /* Servicing interval : 8ms */
    
;;     /* Class Specific Interrupt Endpoint Descriptor */
     db     05H             ;;              /* Descriptor size */
     db     25H             ;;              /* Class Specific Endpoint Descriptor Type */
     db     ET_INT          ;;              /* End point Sub Type */
     dw     4000H           ;;              /* Max packet size = 64 bytes */
    
;;     /* Standard Video Streaming Interface Descriptor (Alternate Setting 0) */
     db     09H             ;;              /* Descriptor size */
     db     DSCR_INTRFC     ;;              /* Interface Descriptor type */
     db     01H             ;;              /* Interface number */
     db     00H             ;;              /* Alternate setting number */
     db     01H             ;;              /* Number of end points : Zero Bandwidth */
     db     0EH             ;;              /* Interface class : CC_VIDEO */
     db     02H             ;;              /* Interface sub class : CC_VIDEOSTREAMING */
     db     00H             ;;              /* Interface protocol code : Undefined */
     db     00H             ;;              /* Interface descriptor string index */

;;    /* Class-specific Video Streaming Input Header Descriptor */
     db     0EH             ;;           /* Descriptor size */
     db     24H             ;;           /* Class-specific VS I/f Type */
     db     01H             ;;           /* Descriptotor Subtype : Input Header */
     db     01H             ;;           /* 1 format desciptor follows */
     dw     4700H          ;;           /* Total size of Class specific VS descr: 41 Bytes */
     db     EP_BULK_VIDEO   ;;           /* EP address for BULK video data */
     db     00H             ;;             /* No dynamic format change supported */
     db     04H             ;;             /* Output terminal ID : 4 */
     db     01H             ;;             /* Still image capture method 1 supported */
     db     00H             ;;             /* Hardware trigger NOT supported */
     db     00H             ;;             /* Hardware to initiate still image capture NOT supported */
     db     01H             ;;             /* Size of controls field : 1 byte */
     db     00H             ;;             /* D2 : Compression quality supported - No Compression */    
 
;;    /* Class specific Uncompressed VS format descriptor */
     db     1BH           ;;              /* Descriptor size */
     db     24H           ;;              /* Class-specific VS I/f Type */
     db     04H           ;;              /* Subtype : uncompressed format I/F */
     db     01H           ;;              /* Format desciptor index (only one format is supported) */
     db     01H           ;;              /* number of frame descriptor followed */
     dw     5955H
     dw     5932H      ;;    /* GUID used to identify streaming-encoding format: YUY2  */
     dw     0000H
     dw     1000H      ;;
     dw     8000H
     dw     00AAH      ;;
     dw     0038H
     dw     9B71H      ;;
     db     10H             ;;            /* Number of bits per pixel used to specify color in the decoded video frame.
                            ;;            0 if not applicable: 10 bit per pixel */
     db     01H             ;;            /* Optimum Frame Index for this stream: 1 */
     db     08H             ;;            /* X dimension of the picture aspect ratio: Non-interlaced in
	 	         	        ;;           progressive scan */
     db     06H             ;;            /* Y dimension of the picture aspect ratio: Non-interlaced in
	                  	    ;;	   progressive scan*/
     db     00H             ;;            /* Interlace Flags: Progressive scanning, no interlace */
     db     00H             ;;            /* duplication of the video stream restriction: 0 - no restriction */
         
         
         
;;    /* Class specific Uncompressed VS Frame descriptor */
     db     1EH               ;;        /* Descriptor size */
     db     24H               ;;        /* Descriptor type*/
     db     05H               ;;        /* Subtype: uncompressed frame I/F */
     db     01H               ;;        /* Frame Descriptor Index */
     db     01H               ;;        /* Still image capture method 1 supported, fixed frame rate */
     dw     8002H           ;;         /* Width in pixel: 320-QVGA */
     dw     0E001H           ;;         /* Height in pixel 240-QVGA */
     dw     0000H
     dw     0CA08H       ;;   /* Min bit rate bits/s. Not specified, taken from MJPEG */
     dw     0000H
     dw     0CA08H       ;;   /* Max bit rate bits/s. Not specified, taken from MJPEG */
     dw     0060H
     dw     0900H       ;;   /* Maximum video or still frame size in bytes(Deprecated) */
     dw     1516H
     dw     0500H       ;;   /* Default Frame Interval */
     db     01H               ;;        /* Frame interval(Frame Rate) types: Only one frame interval supported */
     dw     1516H
     dw     0500H       ;;   /* Shortest Frame Interval */    

;;     /* Endpoint Descriptor for BULK Streaming Video Data */
     db     07H                 ;;        /* Descriptor size */
     db     DSCR_ENDPNT        ;;    /* Endpoint Descriptor Type */
     db     EP_BULK_VIDEO      ;;    /* Endpoint address and description */
     db     ET_BULK           ;;              /* BULK End point */
     db     00H                ;;    /* High speed max packet size is always 512 bytes. */
     db     02H                ;;
     db     00H               ;;           /* Servicing interval for data transfers */

HighSpeedConfigDscrEnd:   

FullSpeedConfigDscr:   
      db   09H      ;; Descriptor length
      db   DSCR_CONFIG          ;; Descriptor type
      db   (FullSpeedConfigDscrEnd-FullSpeedConfigDscr) mod 256 ;; Total Length (LSB)
      db   (FullSpeedConfigDscrEnd-FullSpeedConfigDscr)  /  256 ;; Total Length (MSB)
      db   1                    ;; Number of interfaces
      db   1                    ;; Configuration number
      db   0                    ;; Configuration string
      db   10100000b            ;; Attributes (b7 - buspwr, b6 - selfpwr, b5 - rwu)
      db   32H                   ;; Power requirement (div 2 ma)

      db   09H                           /* Descriptor size */
      db   DSCR_INTRFC          ;; Descriptor type
      db   00H                           /* Interface number */
      db   00H                           /* Alternate setting number */
      db   00H                           /* Number of endpoints */
      db   0FFH                           /* Interface class */
      db   00H                           /* Interface sub class */
      db   00H                           /* Interface protocol code */
      db   00H                           /* Interface descriptor string index */
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
      
