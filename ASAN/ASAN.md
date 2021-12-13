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

![image-20211213235552082](C:\Users\45043\AppData\Roaming\Typora\typora-user-images\image-20211213235552082.png)

如图中所示，fd地址块标记的是fd，表示我们对已经释放过的内存进行了访问，造成了错误。从图中的调用栈，我们也可以精确的看到出错的原因已经具体的位置。



