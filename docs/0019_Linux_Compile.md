# Linux Compile

## Source Code

* git clone https://github.com/DreamSourceLab/DSLogic-fw

## Steps

* apt-cache search sdcc
* sudo apt-get install sdcc
* ./autogen.sh
* ./configure
* make

## Dir info

```Console
pi@raspberrypi:~/zengjf/DSLogic-fw $ ls
aclocal.m4  autom4te.cache  config.h.in    configure     DSLogic.asm  DSLogic.re
AUTHORS     autostuff       config.log     configure.ac  DSLogic.c    DSLogic.rs
autogen.sh  config.h        config.status  COPYING       DSLogic.lst  DSLogic.sy
pi@raspberrypi:~/zengjf/DSLogic-fw $ ls
aclocal.m4      config.log     DSLogic.lst  INSTALL        Makefile     usb.c
AUTHORS         config.status  DSLogic.rel  interface.asm  Makefile.am  usb.lst
autogen.sh      configure      DSLogic.rst  interface.c    Makefile.in  usb.rel
autom4te.cache  configure.ac   DSLogic.sym  interface.lst  NEWS         usb.rst
autostuff       COPYING        fx2lib       interface.rel  README       usb.sym
config.h        DSLogic.asm    hw           interface.rst  stamp-h1
config.h.in     DSLogic.c      include      interface.sym  usb.asm
pi@raspberrypi:~/zengjf/DSLogic-fw $ ls hw/
dscr.a51  dscr.rst    DSLogic.ihx  DSLogic.mem  Makefile.in
dscr.lst  dscr.sym    DSLogic.lk   Makefile
dscr.rel  DSLogic.fw  DSLogic.map  Makefile.am
pi@raspberrypi:~/zengjf/DSLogic-fw $ ls -alh hw/DSLogic.fw
-rw-r--r-- 1 pi pi 8.0K Nov 30 08:42 hw/DSLogic.fw
```

## 反汇编

* 安装dis51
  * http://plit.de/asem-51/dis51.html
    * http://plit.de/asem-51/dis51-0.5.tar.gz
    * wget http://plit.de/asem-51/dis51-0.5.tar.gz
    * tar xvf dis51-0.5.tar.gz
    * make
  * sudo apt-get install dis51
* packihx DSLogic.ihx > DSLogic.hex
* 反汇编：
  * cat DSLogic.hex | dis51 > DSLogic.a51
  * cat DSLogic.hex | dis51 0x80 0x00 > DSLogic.a51
    * CSEG：code segment
    * LJMP：The LJMP instruction transfers program execution to the specified 16-bit address. 
    * DB：The DB statement initializes memory with one or more byte values. label is a symbol that is assigned the current memory address. expression is a byte value that is stored in memory. Each expression may be a symbol, a string, or an expression.