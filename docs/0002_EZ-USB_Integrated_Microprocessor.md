# EZ-USB Integrated Microprocessor

## 参考文档

* [EZ-USB® Technical Reference Manual](http://www.cypress.com/documentation/technical-reference-manuals/ez-usb-technical-reference-manual)

## EZ-USB Integrated Microprocessor

* 16KB Pgm/Data RAM就认为是ROM比较合适（程序从EEPROM来），然后只有0.5K RAM和4KB的Endpoint RAM；
* The CPU communicates with the SIE using a set of registers occupying on-chip RAM addresses 0xE500-0xE6FF. These registers are grouped and described by function in individual chapters of this reference manual and summarized in register order.
* The EZ-USB chips add eight interrupt sources to the standard 8051 interrupt system:
  * INT2: USB Interrupt
  * INT3: I2C Bus Interrupt
  * INT4: FIFO/GPIF Interrupt
  * INT4: External Interrupt 4
  * INT5: External Interrupt 5
  * INT6: External Interrupt 6
  * USART1: USART1 Interrupt
  * WAKEUP: USB Resume Interrupt
* The EZ-USB chips provide 27 individual USB-interrupt sources which share the INT2 interrupt, and 14 individual FIFO/GPIF-interrupt sources which share the INT4 interrupt.
