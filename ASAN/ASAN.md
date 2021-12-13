# **ASAN**

## **简介**

- Address Sanitizer 地址消毒

- 快速定位C/C++内存错误的工具，可检测的问题类型：越界访问、内存泄漏、访问已释放的内存等
- github:https://github.com/google/sanitizers

## **原理**

**插桩模块（Instrumentation module）**

- 影子内存（Shadow Memory）：对所有内存访问都会去检查该内存所对应的影子内存的状态。

  注：影子内存：如8字节对齐，若只用了前4个字节，则后4个字节将被标记为影子内存，刚申请的时候，影子内存被置为fa，使用后释放被置为fd。

  

- 保护区（Poison Redzone）：对栈上对象和全局对象前后创建保护区，以更好的检测溢出问题。

**运行时库（Runtime library）**

- 替换掉malloc/free函数，为堆对象创建前后的保护区。
- 对错误信息进行输出，包括堆栈信息。



## **使用**

- 编译时加入编译选项：-fsanitize=address
- 可加入编译选项：-fno-omit-frame-pointer，出错时打印更清晰的栈信息



## **demo**

**use_after_free.c**

```c
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
    int *nums = (int *)malloc(10 * sizeof(int));

    free(nums);

    nums[1] = 100;

    return 0;
}
```



**编译运行**

- gcc use_after_free.c -o use_after_free -fsanitize=address -g -fno-omit-frame-pointer
- export LD_PRELOAD=/lib64/libasan.so.5
- ./use_after_free



**结果**

```c
[root@VM-20-2-centos ASAN]# ./use_after_free 
=================================================================
==987094==ERROR: AddressSanitizer: heap-use-after-free on address 0x604000000014 at pc 0x0000004007ee bp 0x7ffc9a460cf0 sp 0x7ffc9a460ce0
WRITE of size 4 at 0x604000000014 thread T0
    #0 0x4007ed in main /root/Memory-Problem/ASAN/use_after_free.c:10
    #1 0x7facf1d3e492 in __libc_start_main (/lib64/libc.so.6+0x23492)
    #2 0x4006cd in _start (/root/Memory-Problem/ASAN/use_after_free+0x4006cd)

0x604000000014 is located 4 bytes inside of 40-byte region [0x604000000010,0x604000000038)
freed by thread T0 here:
    #0 0x7facf21cf7e0 in __interceptor_free (/lib64/libasan.so.5+0xef7e0)
    #1 0x4007ae in main /root/Memory-Problem/ASAN/use_after_free.c:8
    #2 0x7facf1d3e492 in __libc_start_main (/lib64/libc.so.6+0x23492)

previously allocated by thread T0 here:
    #0 0x7facf21cfba8 in __interceptor_malloc (/lib64/libasan.so.5+0xefba8)
    #1 0x40079e in main /root/Memory-Problem/ASAN/use_after_free.c:6
    #2 0x7facf1d3e492 in __libc_start_main (/lib64/libc.so.6+0x23492)

SUMMARY: AddressSanitizer: heap-use-after-free /root/Memory-Problem/ASAN/use_after_free.c:10 in main
Shadow bytes around the buggy address:
  0x0c087fff7fb0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c087fff7fc0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c087fff7fd0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c087fff7fe0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c087fff7ff0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x0c087fff8000: fa fa[fd]fd fd fd fd fa fa fa fa fa fa fa fa fa
  0x0c087fff8010: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff8020: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff8030: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff8040: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c087fff8050: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==987094==ABORTING
```

如上图中所示，fd地址块标记的是fd，表示我们对已经释放过的内存进行了访问，造成了错误。从图中的调用栈，我们也可以精确的看到出错的原因已经具体的位置。



