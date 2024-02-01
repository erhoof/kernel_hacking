This module creates ioctl device only, no /dev /proc some device

So to use that you need to
```
$ dmesg // to see major number
$ mknod /dev/ioctltest c 508 0 // 508 is major, 0 is minor
```
