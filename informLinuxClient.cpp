/*
 *	本程序的主要目的在于说明socket编程的基本过程，所以服务器/客户端的交互过程非常简单，
 *  只是由客户端向服务器传送一个学生信息的结构。
 */
//informLinuxClient.cpp：参数为 serverIP name age
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>

#define SERVER_PORT	5555 //侦听端口

extern int errno;

//客户端向服务器传送的结构：
struct student
{
    char info[512];
    int  type;
};

int main(int argc, char *argv[])
{	
	int sockfd;
	struct sockaddr_in serverAddr;
	struct student stu;
	char c;
	
	if(argc != 4)
	{
		printf("usage: informLinuxClient serverIP name age\n");
		return -1;
	}

	// 创建一个socket：
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket() failed! code:%d\n", errno);
		return -1;
	}
		
	// 设置服务器的地址信息：
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
	bzero(&(serverAddr.sin_zero), 8);
		
	//客户端发出连接请求：
	printf("connecting!\n");
	if(connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
		printf("connect() failed! code:%d\n", errno);
		close(sockfd);
		return -1;
	}

	printf("Connected!\n");
	
	strcpy(stu.info, argv[2]);
	stu.type = atoi(argv[3]);
	if(send(sockfd, (char *)&stu, sizeof(stu), 0) == -1)
	{
		printf("send() failed!\n");
		close(sockfd);
		return -1;
	}
	printf("please enter to continue...\n");
	scanf("%c",&c);
	if(send(sockfd, (char *)&stu, sizeof(stu), 0) == -1)
	{
		printf("send() failed!\n");
		close(sockfd);
		return -1;
	}

	printf("student info has been sent!\n");
	close(sockfd);//关闭套接字
	return 0;
}
