# USB Camera UVC

貌似这份代码没有完整实现UVC，所以没啥用

## 参考文档

* [Interfacing FX2LP™ with Image Sensor – KBA95736](https://community.cypress.com/docs/DOC-9329)
* [USB2.0 Camera Interface Using FX2LP™ and Lattice CrossLink FPGA - KBA222479](https://community.cypress.com/docs/DOC-14406)
* [Create Your Own USB Vendor Commands Using FX2LP™](https://www.cypress.com/file/202971/download)
* [USB68013种的arm endpoint是什么意思](http://bbs.21ic.com/icview-828608-1-1.html)
  * arm相当于使Endpoint处于待命的状态，随时准备数据传输！
  * re-arm就是再次待命。
* [UVC v4l2](https://github.com/ZengjfOS/V4L2/blob/master/docs/0001_uvc/README.md)
* [camera驱动部分视频数据流分析（uvc）](https://blog.csdn.net/qq160816/article/details/62419489)
* [USB Video Class 1_5 Specification](https://www.usb.org/document-library/video-class-v15-document-set)


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

## UVC

* https://github.com/ZengjfOS/V4L2/blob/master/docs/0002_v4l2_capture/README.md
* Kernel
  * drivers/media/usb/uvc/uvc_video.c
    * `static void uvc_video_complete(struct urb *urb)`这个函数相当于MIPI CSI的帧中断函数；  
    * urb->complete = uvc_video_complete; (收到数据后此函数被调用,它又调用video->decode(urb, video, buf); ==> uvc_video_decode_isoc/uvc_video_encode_bulk => uvc_queue_next_buffer => wake_up(&buf->wait);)
* 一帧图片的获取流程
  ```
  * drivers/media/usb/uvc/uvc_v4l2.c
    * const struct v4l2_ioctl_ops uvc_ioctl_ops 
      * .vidioc_streamon = uvc_ioctl_streamon
        * ret = uvc_queue_streamon(&stream->queue, type);
          * ret = vb2_streamon(&queue->queue, type);
            * drivers/media/common/videobuf2/videobuf2-v4l2.c
              * vb2_core_streamon(q, type);
                * ret = vb2_start_streaming(q);
                  * ret = call_qop(q, start_streaming, q, atomic_read(&q->owned_by_drv_count));
                    * drivers/media/usb/uvc/uvc_queue.c
                      * static const struct vb2_ops uvc_queue_qops
                        * .start_streaming = uvc_start_streaming
                          * ret = uvc_video_enable(stream, 1);
                            * ret = uvc_commit_video(stream, &stream->ctrl);
                            * ret = uvc_init_video(stream, GFP_KERNEL);
                              * ret = uvc_init_video_isoc(stream, best_ep, gfp_flags);
                                * urb->complete = uvc_video_complete;
                                  * stream->decode(urb, stream, buf, buf_meta);
                                    * drivers/media/usb/uvc/uvc_video.c
                                      * int uvc_video_init(struct uvc_streaming *stream)
                                        * stream->decode = uvc_video_decode_isoc;
                                        * stream->decode = uvc_video_decode_bulk;
                              * ret = uvc_init_video_bulk(stream, ep, gfp_flags);
                                * usb_fill_bulk_urb(urb, stream->dev->udev, pipe, stream->urb_buffer[i], size, uvc_video_complete, stream);
  ```
  ![images/UVC_USB_Get_Buffer.svg](images/UVC_USB_Get_Buffer.svg)
* uvc_video_decode_isoc，参考《USB_Video_Class_1.x.pdf》中"2.4.3.3 Video and Still Image Payload Headers"小节：
  Every Payload Transfer containing video or still-image sample data must start with a Payload Header. 
  ```C
  static void uvc_video_decode_isoc(struct urb *urb, struct uvc_streaming *stream,
                          struct uvc_buffer *buf, struct uvc_buffer *meta_buf)
  {
          u8 *mem;
          int ret, i;
  
          for (i = 0; i < urb->number_of_packets; ++i) {
                  if (urb->iso_frame_desc[i].status < 0) {
                          uvc_trace(UVC_TRACE_FRAME, "USB isochronous frame "
                                  "lost (%d).\n", urb->iso_frame_desc[i].status);
                          /* Mark the buffer as faulty. */
                          if (buf != NULL)
                                  buf->error = 1;
                          continue;
                  }
  
                  /* Decode the payload header. */
                  mem = urb->transfer_buffer + urb->iso_frame_desc[i].offset;
                  do {
                          ret = uvc_video_decode_start(stream, buf, mem,
                                  urb->iso_frame_desc[i].actual_length);
                          if (ret == -EAGAIN)
                                  uvc_video_next_buffers(stream, &buf, &meta_buf);
                  } while (ret == -EAGAIN);
  
                  if (ret < 0)
                          continue;
  
                  uvc_video_decode_meta(stream, meta_buf, mem, ret);
  
                  /* Decode the payload data. */
                  uvc_video_decode_data(stream, buf, mem + ret,
                          urb->iso_frame_desc[i].actual_length - ret);
  
                  /* Process the header again. */
                  uvc_video_decode_end(stream, buf, mem,
                          urb->iso_frame_desc[i].actual_length);
  
                  if (buf->state == UVC_BUF_STATE_READY)
                          uvc_video_next_buffers(stream, &buf, &meta_buf);
          }
  }
  ```
* 这个时候我们就会疑问，一帧结束了我们才调用complete，这是为什么呢？一张图片那么大，不可能一次传完，肯定分很多小帧，这个我们可能就要去找出`urb->number_of_packets`是谁决定的；
  * static int uvc_init_video_isoc(struct uvc_streaming *stream, struct usb_host_endpoint *ep, gfp_t gfp_flags)
    * urb->number_of_packets = npackets;
    * npackets = uvc_alloc_urb_buffers(stream, size, psize, gfp_flags);
      * npackets = DIV_ROUND_UP(size, psize); 
        * 以上size是图片数据大小，psize是单次USB端点数据传输大小；
* uvc_alloc_urb_buffers
  ```C
  static int uvc_alloc_urb_buffers(struct uvc_streaming *stream,
          unsigned int size, unsigned int psize, gfp_t gfp_flags)
  {
          unsigned int npackets;
          unsigned int i;
  
          /* Buffers are already allocated, bail out. */
          if (stream->urb_size)
                  return stream->urb_size / psize;
  
          /* Compute the number of packets. Bulk endpoints might transfer UVC
           * payloads across multiple URBs.
           */
          npackets = DIV_ROUND_UP(size, psize);
          if (npackets > UVC_MAX_PACKETS)
                  npackets = UVC_MAX_PACKETS;
  
          /* Retry allocations until one succeed. */
          for (; npackets > 1; npackets /= 2) {
                  for (i = 0; i < UVC_URBS; ++i) {
                          stream->urb_size = psize * npackets;
  #ifndef CONFIG_DMA_NONCOHERENT
                          stream->urb_buffer[i] = usb_alloc_coherent(
                                  stream->dev->udev, stream->urb_size,
                                  gfp_flags | __GFP_NOWARN, &stream->urb_dma[i]);
  #else
                          stream->urb_buffer[i] =
                              kmalloc(stream->urb_size, gfp_flags | __GFP_NOWARN);
  #endif
                          if (!stream->urb_buffer[i]) {
                                  uvc_free_urb_buffers(stream);
                                  break;
                          }
                  }
  
                  if (i == UVC_URBS) {
                          uvc_trace(UVC_TRACE_VIDEO, "Allocated %u URB buffers "
                                  "of %ux%u bytes each.\n", UVC_URBS, npackets,
                                  psize);
                          return npackets;
                  }
          }
  
          uvc_trace(UVC_TRACE_VIDEO, "Failed to allocate URB buffers (%u bytes "
                  "per packet).\n", psize);
          return 0;
  }
  ```
* 如上可知，获取一张UVC图片，本身是一个URB请求完成一帧图片的获取，在初始化URB的时候，会算好要请求多少次，完成一次请求；