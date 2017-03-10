/*
 *	本程序的主要目的在于说明socket编程的基本过程，所以服务器/客户端的交互过程非常简单，
 *  只是由客户端向服务器传送一个学生信息的结构。
 */
//informLinuxServer.cpp：运行时没有参数，使用端口6666进行侦听：
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>

#define SERVER_PORT	6666 //侦听端口

 extern int errno;

//客户端向服务器传送的结构：
struct student
{
    char info[512];
    int  type;
};

 int main()
 {	
 	int sockfd, comfd;
 	struct sockaddr_in serverAddr, clientAddr;
 	int ret, iClientSize;
 	struct student stu;
 	void *ptr;

	// 创建一个socket：
	// 流式套接字（SOCK_STREAM）：流式套接字提供可靠的、面向连接的通信流。典型的有TCP协议，能保证数据传输的正确性和顺序。
 	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
 	{
 		printf("socket() failed! code:%d\n", errno);
 		return -1;
 	}

	//  绑定：
	/*
		把主机字节顺序转化成网络字节顺序的函数：
		u_long htonl(u_long hostlong);
		u_short htons(u_short hostshort);
	*/
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(SERVER_PORT);
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		bzero(&(serverAddr.sin_zero), 8);
		if(bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		{
			printf("bind() failed! code:%d\n", errno);
			close(sockfd);
			return -1;
		}
		
	// 侦听控制连接请求：
		if(listen(sockfd, 5) == -1)
		{
			printf("listen() failed! code:%d\n", errno);
			close(sockfd);
			return -1;
		}
		
		printf("Waiting for client connecting!\n");
		printf("tips : Ctrl+c to quit!\n");

		//接受客户端连接请求：
		iClientSize = sizeof(struct sockaddr_in);
		while(1){
			comfd = accept(sockfd, (struct sockaddr *)&clientAddr,(socklen_t *) &iClientSize);

			printf("ensure the socket slept before");

			printf("Accepted client: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

			//接收客户端的数据：		
			int nLeft = sizeof(stu);
			ptr = &stu;
			while(nLeft >0)
			{
			//接收数据：
				ret = recv(comfd, ptr, nLeft, 0);
				if(ret <= 0)
				{
					printf("recv() failed!\n");
					// close(sockfd);//关闭套接字
					close(comfd);
					return -1;
				}

				nLeft -= ret;
				ptr = (char *)ptr + ret;
			}

			printf("info: %s\ntype:%d\n", stu.info, stu.type);
			close(comfd);
		}

	close(sockfd);//关闭套接字
	return 0;
}
