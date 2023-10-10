# lithe
### cpp11 network framework

​	支持使用协程和传统的异步回调方式进行编程。只支持Linux (Ubuntu, debian编译成功)，暂时只能使用IPV4网络。

### **内置:**

##### 1-> 日志模块  
    流式日志使用: LOG_INFO(g_logger) << "hello lithe";

##### 2-> 线程模块
    封装了pthread

##### 3-> 协程模块
    参考了腾讯微信的libco
    用POSIX的ucontext实现的上下文切换

##### 4-> n:1协程调度
    后续会支持n:m协程调度器

##### 5-> hook模块
    封装了可能会阻塞的系统函数

##### 6-> Reactor模块
    one loop per thread

##### 7-> 定时器模块
    定时器管理器使用的std::set管理定时器 自定义比较函数

##### 8-> 字节流
    可以按变长和定长方式写入字节
    实现了varint编码和zigzag编码

##### 9-> Socket模块
    封装了socket

##### 10-> Address模块
    封装的地址类

##### 11-> TcpConnection
    管理网络连接的类

##### 12-> 应用层缓冲区
    EPOLLET模式，应用层缓冲区缓冲到来的数据和即将发送的数据

##### 13-> TcpServer模块
    方便快速的构建服务器
    为了二进制兼容性, 不使用继承而是使用组合的方式使用这个类。(muduo)

##### 14-> http协议（HTTP/1.1）
    http协议解析


##### BUILD:
	1.cd lithe/net
	2.mkdir build
	3.cd build && cmake ..
	4.make



### TODO：

##### 1.封装数据库

##### 2.心跳包

##### 3.IPV6

##### 4.添加SSL层

##### 5.RPC

##### 6.尽可能多的单元测试