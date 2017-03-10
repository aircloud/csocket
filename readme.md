### c语言编写的socket通信服务器

#### 程序说明

我这边没有做可视化界面。

测试环境为Linux 16.04

informLinuxClient.cpp   
informLinuxServer.cpp   
两个文件为参考书提供的简短示例程序。

threadpool.h   
threadpool.c    
为线程调度函数

main.c   
为服务端主程序

mainclient.c
为客户端主程序


#### 接口说明

数据包格式为struct，有info和type两个字段，type取值：

1.Connect to the origin server   
2.Disconnect to the origin server   
3.Get now time  
4.Get the server name     
5.Get the info of all the clients connected to the same server   
6.Send message to a certain client   

info表示信息，通过`,`和`\n`来区隔，其实大多数时候客户端只是接受信息并且呈现即可。


