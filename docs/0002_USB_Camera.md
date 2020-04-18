# USB Camera

## 参考文档

* [EZ-USB® FX3 USB Video Class Example](https://www.cypress.com/documentation/code-examples/ez-usb-fx3-usb-video-class-example)
* https://github.com/ZengjfOS/USB/blob/fx3_uvc_example

## 数据处理主线程

* [从0开始创建FX3工程之一 - 框架](https://blog.csdn.net/a379039233/article/details/74390152)
* `void UVCAppThread_Entry ( uint32_t input) {}`

## active开关

* [When is CY_U3P_USB_EVENT_SETCONF prompted in FX3](https://community.cypress.com/thread/42281?start=0&tstart=0)

```CPP
/* This is the Callback function to handle the USB Events */
static void
CyFxUVCApplnUSBEventCB (
    CyU3PUsbEventType_t evtype, /* Event type */
    uint16_t            evdata  /* Event data */
    )
{
    switch (evtype)
    {
        case CY_U3P_USB_EVENT_SETCONF:
            /* Stop the application before re-starting. */
            if (glIsApplnActive)
            {
                CyFxUVCApplnStop ();
            }
            CyFxUVCApplnStart ();                                     // 开始传输
            break;
        case CY_U3P_USB_EVENT_RESET:
        case CY_U3P_USB_EVENT_DISCONNECT:
            /* Stop the video streamer application. */
            if (glIsApplnActive)
            {
                CyFxUVCApplnStop ();                                  // 停止传输
            }
            break;

        default:
            break;
    }
}
```

## 一张图片总结

* 一张图片就是一个数组；
* 一张图片在传输的过程中被拆分成小usb的frame进行传输，每一个frame有一个header，用于表示当前frame的信息；
* 一张图片结束最后一个usb frame是有`End of frame`标记的，一个bit位，从而表示当前一张图片结束了，frame的header中的序号可以知道起始frame了；
* 大体处理如链接：https://github.com/ZengjfOS/USB/blob/fx3_uvc_example/cyfxuvcinmem.c#L577
* 具体处理细节暂时不细究了，目的也就是为了理解摄像头到底是怎么把一阵图片发出来的，Linux USB驱动解析：https://github.com/ZengjfOS/USB/blob/master/docs/0023_USB_Camera_UVC.md
