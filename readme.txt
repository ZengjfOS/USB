
This directory contains 8051 firmware for the Cypress EZ-USB FX2LP chip.

The purpose of this code is to demonstrate how to design a UVC Framework for the 
EZUSB FX2LP device. 

The code is written in C and uses both the EZ-USB FX library and frameworks.

Firmware provided with the application note AN61345 (Designing with EZ-USB®  
FX2LP™ Slave FIFO Interface) is used as the base firmware to design this firmware.  
The following are the differences in firmware between that provided with the AN61345 
application note and this firmware:

• Descriptor file (dscr.a51): Vendor class interface changed to UVC interface.   
  The UVC class consists of a Camera terminal > Processing unit > Empty Extension 
  unit > Output terminal. The output terminal is the video streaming interface. 
  According to the UVC specification, there is an INT type video control endpoint and 
  a BULK type video streaming endpoint. The whole structure is similar to the AN75779 
  app note firmware for the FX3 device. There is one YUY2 format descriptor and 
  a 640x480@30fps resolution. 

• Main file (fw.c): The difference in the main file is in handling video streaming 
  requests for UVC class in the SetupCommand() callback function. Video streaming requests 
  such as SET_CUR and GET_CUR are handled in this file. Handling such class-specific 
  requests is similar to the AN75779 application note firmware for the FX3 device.

• Slave FIFO Config file (slave.c): FX2LP device register settings are modified to set 
  one BULK IN endpoint, buffer size of 2 KB with quad buffering in slave FIFO mode, 
  and Empty flag to check buffer availability. 

