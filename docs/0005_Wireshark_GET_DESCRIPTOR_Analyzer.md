# Wireshark GET DESCRIPTOR Analyzer

**纯个人想法**：  
* URB可以认为是保证数据传送到对应的设备的数据格式，相当于总线仲裁数据；
* 我们一般说的数据分析USB协议分析，分析的是实际到设备以后的数据包，这里就是SETUP包、DATA0、DATA1、响应包等；

## GET DESCRIPTOR Request DEVICE
* raw data:
  ```
  0000   1c 00 f0 7a 0f a1 05 ac ff ff 00 00 00 00 0b 00   ..ðz.¡.¬ÿÿ......
  0010   00 01 00 28 00 00 02 08 00 00 00 00 80 06 00 01   ...(............
  0020   00 00 12 00                                       ....
  ```
* data analyzer:
  ```
  Frame 2903: 36 bytes on wire (288 bits), 36 bytes captured (288 bits) on interface 0
  USB URB
      [Source: host]
      [Destination: 1.40.0]
      USBPcap pseudoheader length: 28
      IRP ID: 0xffffac05a10f7af0
      IRP USBD_STATUS: USBD_STATUS_SUCCESS (0x00000000)
      URB Function: URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE (0x000b)
      IRP information: 0x00, Direction: FDO -> PDO
          0000 000. = Reserved: 0x00
          .... ...0 = Direction: FDO -> PDO (0x0)
      URB bus id: 1
      Device address: 40
      Endpoint: 0x00, Direction: OUT
          0... .... = Direction: OUT (0)
          .... 0000 = Endpoint number: 0
      URB transfer type: URB_CONTROL (0x02)
      Packet Data Length: 8
      [Response in: 2904]
      Control transfer stage: Setup (0)
  URB setup
      bmRequestType: 0x80
          1... .... = Direction: Device-to-host
          .00. .... = Type: Standard (0x0)
          ...0 0000 = Recipient: Device (0x00)
      bRequest: GET DESCRIPTOR (6)
      Descriptor Index: 0x00
      bDescriptorType: 0x01
      Language Id: no language specified (0x0000)
      wLength: 18
  ```

## GET DESCRIPTOR Response DEVICE
* raw data:
  ```
  0000   1c 00 f0 7a 0f a1 05 ac ff ff 00 00 00 00 08 00   ..ðz.¡.¬ÿÿ......
  0010   01 01 00 28 00 80 02 12 00 00 00 01 12 01 00 02   ...(............
  0020   ff ff ff 40 b4 04 13 86 01 a0 00 00 00 01         ÿÿÿ@´.... ....
  ```
* data analyzer:
  ```
  Frame 2904: 46 bytes on wire (368 bits), 46 bytes captured (368 bits) on interface 0
  USB URB
      [Source: 1.40.0]
      [Destination: host]
      USBPcap pseudoheader length: 28
      IRP ID: 0xffffac05a10f7af0
      IRP USBD_STATUS: USBD_STATUS_SUCCESS (0x00000000)
      URB Function: URB_FUNCTION_CONTROL_TRANSFER (0x0008)
      IRP information: 0x01, Direction: PDO -> FDO
          0000 000. = Reserved: 0x00
          .... ...1 = Direction: PDO -> FDO (0x1)
      URB bus id: 1
      Device address: 40
      Endpoint: 0x80, Direction: IN
          1... .... = Direction: IN (1)
          .... 0000 = Endpoint number: 0
      URB transfer type: URB_CONTROL (0x02)
      Packet Data Length: 18
      [Request in: 2903]
      [Time from request: 0.000115000 seconds]
      Control transfer stage: Data (1)
  DEVICE DESCRIPTOR
      bLength: 18
      bDescriptorType: 0x01 (DEVICE)
      bcdUSB: 0x0200
      bDeviceClass: Vendor Specific (0xff)
      bDeviceSubClass: 255
      bDeviceProtocol: 255
      bMaxPacketSize0: 64
      idVendor: Cypress Semiconductor Corp. (0x04b4)
      idProduct: CY7C68013 EZ-USB FX2 USB 2.0 Development Kit (0x8613)
      bcdDevice: 0xa001
      iManufacturer: 0
      iProduct: 0
      iSerialNumber: 0
      bNumConfigurations: 1
  ```

## GET DESCRIPTOR Status

* raw data:
  ```
  0000   1c 00 f0 7a 0f a1 05 ac ff ff 00 00 00 00 08 00   ..ðz.¡.¬ÿÿ......
  0010   01 01 00 28 00 80 02 00 00 00 00 02               ...(........
  ```
* data analyzer:
  ```
  Frame 2905: 28 bytes on wire (224 bits), 28 bytes captured (224 bits) on interface 0
  USB URB
      [Source: 1.40.0]
      [Destination: host]
      USBPcap pseudoheader length: 28
      IRP ID: 0xffffac05a10f7af0
      IRP USBD_STATUS: USBD_STATUS_SUCCESS (0x00000000)
      URB Function: URB_FUNCTION_CONTROL_TRANSFER (0x0008)
      IRP information: 0x01, Direction: PDO -> FDO
          0000 000. = Reserved: 0x00
          .... ...1 = Direction: PDO -> FDO (0x1)
      URB bus id: 1
      Device address: 40
      Endpoint: 0x80, Direction: IN
          1... .... = Direction: IN (1)
          .... 0000 = Endpoint number: 0
      URB transfer type: URB_CONTROL (0x02)
      Packet Data Length: 0
      [Request in: 2903]
      [Time from request: 0.000116000 seconds]
      Control transfer stage: Status (2)
  ```
