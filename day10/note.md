## Days10

#### 关于二进制的位运算
- 某位变成0：运用&运算(与)
- 某位变成1：运用|运算(或)
- 某位反转 ：运用^运算(异或)

#### 内存的分配
- 1个字节的内存分配会出现内存碎片化严重的问题，因此需要将扩大内存的分配，可以将内存扩大至4KB为单位进
行分配，因此在分配内存时，需要将内存以4KB = 0x1000字节为单位进行向上舍入

#### 关于数的舍入
- 以某个数为单位向下舍入,将该数在单位后面全变为0即可。
- 以某个数为单位向上舍入,将该数加上单位-1然后再向下舍入即可。

#### 关于编程
- 在作者的程序中，都是一个struct对应着一个Struct Manager


#### refreshsub函数
- 该函数中直接传入了(vx0,vy0)与（vx1,vy1)两组变量，即已经将一个矩形的坐标全部传入
函数当中了，为什么不直接使用这个变量而作者却选择利用每个图层的信息呢？
简答:这里以鼠标和背景图为例，假如鼠标从(vx0,vy0)移动到(vx1,vy1),两次传入refreshsub函数的参数为
(vx0,vy0,vx0',vy0')和(vx1,vy1,vx1',vy1')。那么当刷新到鼠标那一层时，鼠标在先前
的位置应该是没有颜色的，但是直接使用传入的值,则不能知道鼠标最新的位置
- 改良后的函数有一点难以理解，主要是对几种情况进行了区分。自己还要好好思考下才能理解。
- 循环中都多个判断是，利用先判断再循环的解决思路。即将在循环中确定范围转化为先确定范围再循环。
