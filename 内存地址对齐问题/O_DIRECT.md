## **O_DIRECT问题描述**

- 文件透传功能，表示不经过内核区cache等待，直接向下写入。
- 不经过缓存区等待出现的问题是：读时传下去的地址要按照block_size=512对齐
- 否则出现错误：errno:22, errnoStr: Invalid argument.



## **解决方式**

- 用函数posix_memalign自动申请内存并按照传入对齐。

- **代码：code.c**

```c
// gcc code.c -o code -D_GNU_SOURCE 
// cannot find O_DIRECT 可在编译时加入-D_GNU_SOURCE参数或者加入宏#define _GNU_SOURCE

#define _GNU_SOURCE
  
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define BUF_SIZE 512

int main (int argc, char **argv)
{
    int fd = open("readText.txt", O_CREAT | O_RDWR | O_DIRECT | O_EXCL);

    // char buf[1024];
    char *buf;
    (void)posix_memalign((void **)&buf, BUF_SIZE, BUF_SIZE);
    
    int readSize = read(fd, buf, BUF_SIZE);
    if (readSize == -1) {
        printf("errno:%d, errnoStr:%s\n", errno, strerror(errno));
        return -1;
    }

    printf("exec end!\n");
    
    close(fd);
    return 0;
}
```

