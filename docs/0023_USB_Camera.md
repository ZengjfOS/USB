# USB Camera

貌似这份代码没有完整实现UVC，所以没啥用

## 参考文档

* [Interfacing FX2LP™ with Image Sensor – KBA95736](https://community.cypress.com/docs/DOC-9329)
* [USB2.0 Camera Interface Using FX2LP™ and Lattice CrossLink FPGA - KBA222479](https://community.cypress.com/docs/DOC-14406)
* [Create Your Own USB Vendor Commands Using FX2LP™](https://www.cypress.com/file/202971/download)
* [USB68013种的arm endpoint是什么意思](http://bbs.21ic.com/icview-828608-1-1.html)
  * arm相当于使Endpoint处于待命的状态，随时准备数据传输！
  * re-arm就是再次待命。
* [UVC v4l2](https://github.com/ZengjfOS/V4L2/blob/master/docs/0001_uvc/README.md)

## Source

https://github.com/ZengjfOS/USB/tree/UVCCamera

## USB描述符

* lsusb
  ```
  Bus 003 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
  Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
  Bus 001 Device 003: ID 04b4:0036 Cypress Semiconductor Corp.
  Bus 001 Device 002: ID 2109:3431 VIA Labs, Inc. Hub
  Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
  ```
* lsusb -vd 04b4:0036
  ```
  Bus 001 Device 005: ID 04b4:0036 Cypress Semiconductor Corp.
  Couldn't open device, some information will be missing
  Device Descriptor:
    bLength                18
    bDescriptorType         1
    bcdUSB               2.00
    bDeviceClass          239 Miscellaneous Device
    bDeviceSubClass         2
    bDeviceProtocol         1 Interface Association
    bMaxPacketSize0        64
    idVendor           0x04b4 Cypress Semiconductor Corp.
    idProduct          0x0036
    bcdDevice            0.00
    iManufacturer           1
    iProduct                2
    iSerial                 0
    bNumConfigurations      1
    Configuration Descriptor:
      bLength                 9
      bDescriptorType         2
      wTotalLength       0x00ce
      bNumInterfaces          2
      bConfigurationValue     1
      iConfiguration          0
      bmAttributes         0x80
        (Bus Powered)
      MaxPower              500mA
      Interface Association:
        bLength                 8
        bDescriptorType        11
        bFirstInterface         0
        bInterfaceCount         2
        bFunctionClass         14 Video
        bFunctionSubClass       3 Video Interface Collection
        bFunctionProtocol       0
        iFunction               2
      Interface Descriptor:
        bLength                 9
        bDescriptorType         4
        bInterfaceNumber        0
        bAlternateSetting       0
        bNumEndpoints           1
        bInterfaceClass        14 Video
        bInterfaceSubClass      1 Video Control
        bInterfaceProtocol      0
        iInterface              2
        VideoControl Interface Descriptor:
          bLength                13
          bDescriptorType        36
          bDescriptorSubtype      1 (HEADER)
          bcdUVC               1.10
          wTotalLength       0x0051
          dwClockFrequency       48.000000MHz
          bInCollection           1
          baInterfaceNr( 0)       1
        VideoControl Interface Descriptor:
          bLength                18
          bDescriptorType        36
          bDescriptorSubtype      2 (INPUT_TERMINAL)
          bTerminalID             1
          wTerminalType      0x0201 Camera Sensor
          bAssocTerminal          0
          iTerminal               0
          wObjectiveFocalLengthMin      0
          wObjectiveFocalLengthMax      0
          wOcularFocalLength            0
          bControlSize                  3
          bmControls           0x00000000
        VideoControl Interface Descriptor:
          bLength                13
          bDescriptorType        36
          bDescriptorSubtype      5 (PROCESSING_UNIT)
          bUnitID                 2
          bSourceID               1
          wMaxMultiplier      16384
          bControlSize            3
          bmControls     0x00000000
          iProcessing             0
          bmVideoStandards     0x00
        VideoControl Interface Descriptor:
          bLength                28
          bDescriptorType        36
          bDescriptorSubtype      6 (EXTENSION_UNIT)
          bUnitID                 3
          guidExtensionCode         {ffffffff-ffff-ffff-ffff-ffffffffffff}
          bNumControl             0
          bNrPins                 1
          baSourceID( 0)          2
          bControlSize            3
          bmControls( 0)       0x00
          bmControls( 1)       0x00
          bmControls( 2)       0x00
          iExtension              0
        VideoControl Interface Descriptor:
          bLength                 9
          bDescriptorType        36
          bDescriptorSubtype      3 (OUTPUT_TERMINAL)
          bTerminalID             4
          wTerminalType      0x0101 USB Streaming
          bAssocTerminal          0
          bSourceID               3
          iTerminal               0
        Endpoint Descriptor:
          bLength                 7
          bDescriptorType         5
          bEndpointAddress     0x88  EP 8 IN
          bmAttributes            3
            Transfer Type            Interrupt
            Synch Type               None
            Usage Type               Data
          wMaxPacketSize     0x0040  1x 64 bytes
          bInterval               8
      Interface Descriptor:
        bLength                 9
        bDescriptorType         4
        bInterfaceNumber        1
        bAlternateSetting       0
        bNumEndpoints           1
        bInterfaceClass        14 Video
        bInterfaceSubClass      2 Video Streaming
        bInterfaceProtocol      0
        iInterface              0
        VideoStreaming Interface Descriptor:
          bLength                            14
          bDescriptorType                    36
          bDescriptorSubtype                  1 (INPUT_HEADER)
          bNumFormats                         1
          wTotalLength                   0x0047
          bEndPointAddress                  130
          bmInfo                              0
          bTerminalLink                       4
          bStillCaptureMethod                 1
          bTriggerSupport                     0
          bTriggerUsage                       0
          bControlSize                        1
          bmaControls( 0)                     0
        VideoStreaming Interface Descriptor:
          bLength                            27
          bDescriptorType                    36
          bDescriptorSubtype                  4 (FORMAT_UNCOMPRESSED)
          bFormatIndex                        1
          bNumFrameDescriptors                1
          guidFormat                            {32595559-0000-0010-8000-00aa00389b71}
          bBitsPerPixel                      16
          bDefaultFrameIndex                  1
          bAspectRatioX                       8
          bAspectRatioY                       6
          bmInterlaceFlags                 0x00
            Interlaced stream or variable: No
            Fields per frame: 2 fields
            Field 1 first: No
            Field pattern: Field 1 only
          bCopyProtect                        0
        VideoStreaming Interface Descriptor:
          bLength                            30
          bDescriptorType                    36
          bDescriptorSubtype                  5 (FRAME_UNCOMPRESSED)
          bFrameIndex                         1
          bmCapabilities                   0x01
            Still image supported
          wWidth                            640
          wHeight                           480
          dwMinBitRate                147456000
          dwMaxBitRate                147456000
          dwMaxVideoFrameBufferSize      614400
          dwDefaultFrameInterval         333333
          bFrameIntervalType                  1
          dwFrameInterval( 0)            333333
        Endpoint Descriptor:
          bLength                 7
          bDescriptorType         5
          bEndpointAddress     0x82  EP 2 IN
          bmAttributes            2
            Transfer Type            Bulk
            Synch Type               None
            Usage Type               Data
          wMaxPacketSize     0x0200  1x 512 bytes
          bInterval               0
  ```