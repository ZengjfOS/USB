# USB Specification

## 参考文档

* [EZ-USB® Technical Reference Manual](http://www.cypress.com/documentation/technical-reference-manuals/ez-usb-technical-reference-manual)——1.2 The USB Specification

## 简要

* USB分master(Host)、slave，输入(IN)输出(OUT)端点是站在Master的角度来定义的；
* 除了USB远程唤醒以外，所有的操作均是由USB Master发起；
* USB通信的时候和I2C通信的时候数据方式类似：
  * 站在地址、数据、响应三个阶段进行分析；
  * USB Token相当于I2C发送地址阶段，当然功能不止于I2C地址那么简单；
  * USB Data相当于I2C发送数据阶段，当然功能不止于I2C数据那么简单；
  * USB Handshake相当于I2C发送响应阶段，当然功能不止于I2C响应那么简单；
  * 由于USB比I2C功能更完善，不过思维模型可以从I2C上借鉴；
* DATA0/DATA1出现原因是为了防止握手数据包出错；
* SETUP tokens是控制命令，8 Bytes；
* SOF在全速模式和高速模式下不一样；
* Host发送数据：
  * OUT token packet;
  * Data packet
  * handshake packet;
    * ACK;
    * NAK;              (host re-sends the data at a later time)
    * Back nothing;     (host re-sends the data at a later time)
* Host接收数据：
  * Salve从来不会主动发送数据给主设备，除了远程唤醒功能；
  * 和HOST发送数据类似
* USB Frames(SOF)，不管是全速、高速，都是1ms计数一次，高速1ms中的8次计数不会增加，可以用于数据同步；
* 传输方式：
  * Bulk传输：8/16/32/64 Bytes全速，512 Bytes高速，Host在适当的时候进行数据发送；
  * Interrupt传输：64 Bytes全速，1024Byte高速，实时性相对高的场合；
  * Isochronous传输：实时性要求比较高的场合，音视频流，1023 Bytes全速，1024 Bytes高速，没有handshake，不使用数据切换机制；
  * Control传输：
    * SETUP阶段；
    * 数据阶段(可选)；
    * 状态阶段；
  * 除了Control传输，前面都是针对应用场合，控制传输是在设备配置阶段进行处理的；
* USB枚举：
  * 首先硬件、固件正常情况下；
  * USB设备插入Host，使用USB Host的默认地址0；
  * USB Host通过默认地址发送获取设备描述符请求，控制指令；
  * USB设备反馈ID数据给USB Host；
  * USB Host分配一个唯一的地址并发送给USB设备，控制指令；
  * USB Host获取全部的描述符，从而知道设备端点数、供电、带宽以及需要什么驱动；
* Serial Interface Engine(SIE)
  * 每个USB设备都有SIE；
  * SIE主要进行数据编解码USB总线上的数据包；
  * 自动获取PID，数据校验，提取数据；
  * 会自动处理handshake；
* Endpoint
  *  Since USB is a serial bus, a device endpoint is actually a FIFO which sequentially empties or fills with USB data bytes. 
  * 最大4 bit地址 + 1 bit方向位，最多32个Endpoint IN和OUT；
  * From the EZ-USB’s point of view, an endpoint is a buffer full of bytes received or held for transmission over the bus. The EZ-USB reads host data from an OUT endpoint buffer, and writes data for transmission to the host to an IN endpoint buffer.
  * EZ-USB contains three 64-byte endpoint buffers, plus 4 KB of buffer space that can be configured 12 ways. The three 64-byte buffers are common to all configurations.
  * The three 64-byte buffers are designated EP0, EP1IN and EP1OUT. EP0 is the default CONTROL endpoint, a bidirectional endpoint that uses a single 64-byte buffer for both IN and OUT data. EZ-USB firmware reads or fills the EP0 buffer when the (optional) data stage of a CONTROL transfer is required.
  * **Note** The eight SETUP bytes in a CONTROL transfer do not appear in the 64-byte EP0 endpoint buffer. Instead, to simplify programming, the EZ-USB automatically stores the eight SETUP bytes in a separate buffer (SETUPDAT, at 0xE6B8-0xE6BF).
  * EP1IN and EP1OUT use separate 64 byte buffers. EZ-USB firmware can configure these endpoints as BULK or INTERRUPT.  These endpoints, as well as EP0, are accessible only by EZ-USB firmware. This is in contrast to the large endpoint buffers EP2, EP4, EP6 and EP8, which are designed to move high bandwidth data directly on and off chip without firmware intervention. 

