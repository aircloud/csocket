/* 
    XiaoTao.Nie 3130104419
    All right reserved;

    usage:
    gcc -pthread mainclient.c threadpool.c -o client
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "threadpool.h"
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

/*
    time_t timep;
    time (&timep);
    printf("%s", ctime(&timep));
*/    

//#define SERVER_PORT 5555 //侦听端口
extern int errno;

struct student
{
    char info[512];
    int  type;
};

struct addressInfo{
    char ip[64];
    int port;
    int c_comfd;
    bool status;
};

struct addressInfo allAddress[20];

void int2str(int i, char *s) {
      sprintf(s,"%d",i);
}

void* work(void* sockfd)
{

    struct student stu;
    struct student toBeSend;
    int comfd= *(int *)sockfd;
    int nLeft = sizeof(stu);
    void *ptr = &stu;
    int ret;
    int end=0;

    int j;

    char *delim = ",";

    while(!end){
        ret = recv(comfd, ptr, nLeft, 0);
        if(ret>0){
            nLeft -= ret;
            ptr = (char *)ptr + ret;
        }
        if(nLeft<=0){
            if(stu.type==6) printf("Get message from another client:\n %stype:%d\n", stu.info, stu.type);
            else if(strlen(stu.info)==0)printf("Get message from server:\ntype:%d\n", stu.type);
            else  printf("Get message from server:\n %stype:%d\n", stu.info, stu.type);

            switch(stu.type){
                case 2:
                    close(comfd);
                    return NULL;
            }
            nLeft=sizeof(stu);ptr = &stu;
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{   
    int sockfd;
    struct sockaddr_in serverAddr;
    struct student stu;
    char c;
    int choice;
    int serverPort;
    char serverIp[20];
    int connected = 0;
    int toclientID = 0;
    char toclientIDChar[10];
    int end = 0 ;

    struct threadpool *pool = threadpool_init(10, 20);

    // 创建一个socket：
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket() failed! code:%d\n", errno);
        return -1;
    }

    //初始化工作结束...
        
    // 设置服务器的地址信息：

    printf("These are all functions:\n");
    printf(" 1.Connect to the origin server:\n");
    printf(" 2.Disconnect to the origin server\n");
    printf(" 3.Get now time\n");
    printf(" 4.Get the server name\n");
    printf(" 5.Get the info of all the clients connected to the same server\n");
    printf(" 6.Send message to a certain client\n");
    printf(" 7.Quit the program\n");
    printf("----------------------------------------------------------------------\n");
    
    while(!end){
        printf("please input the id of the function tochoose:\n");
        scanf("%d",&choice);
        switch (choice) {

            case 1:
                printf("Please input ip and port:");
                scanf("%s%d",serverIp,&serverPort);

                serverAddr.sin_family = AF_INET;
                serverAddr.sin_port = htons(serverPort);
                serverAddr.sin_addr.s_addr = inet_addr(serverIp);
                bzero(&(serverAddr.sin_zero), 8);

                printf("connecting!\n");
                if(connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
                {
                    printf("connect() failed! code:%d\n", errno);
                    close(sockfd);
                    return -1;
                }
                printf("Connected!\n");
                threadpool_add_job(pool, work, (void *)(&sockfd));
                connected=1;
                break;

            case 2:
                if(!connected){
                    printf("Please connect to the server first\n");
                    break;
                }
                strcpy(stu.info,"");
                stu.type = 2;
                if(send(sockfd, (char *)&stu, sizeof(stu), 0) == -1)
                {
                    printf("send() failed!\n");
                    close(sockfd);
                    return -1;
                }
                connected=0;
                break;    
            case 3:
                if(!connected){
                    printf("Please connect to the server first\n");
                    break;
                }
                strcpy(stu.info,"");
                stu.type = 3;
                if(send(sockfd, (char *)&stu, sizeof(stu), 0) == -1)
                {
                    printf("send() failed!\n");
                    close(sockfd);
                    return -1;
                }
                break;  
            case 4:
                if(!connected){
                    printf("Please connect to the server first\n");
                    break;
                }
                strcpy(stu.info,"");
                stu.type = 4;
                if(send(sockfd, (char *)&stu, sizeof(stu), 0) == -1)
                {
                    printf("send() failed!\n");
                    close(sockfd);
                    return -1;
                }
                break;        
            case 5:
                if(!connected){
                    printf("Please connect to the server first\n");
                    break;
                }
                strcpy(stu.info,"");
                stu.type = 5;
                if(send(sockfd, (char *)&stu, sizeof(stu), 0) == -1)
                {
                    printf("send() failed!\n");
                    close(sockfd);
                    return -1;
                }
                break; 
            case 6:
                if(!connected){
                    printf("Please connect to the server first\n");
                    break;
                }
                printf("Please input the message and the client id\n");             
                scanf("%s%d",stu.info,&toclientID);
                int2str(toclientID,toclientIDChar);
                strcat(stu.info,",");
                strcat(stu.info,toclientIDChar);
                stu.type=6;
                if(send(sockfd, (char *)&stu, sizeof(stu), 0) == -1)
                {
                    printf("send() failed!\n");
                    close(sockfd);
                    return -1;
                }
                break; 
            case 7:
                strcpy(stu.info,"");
                stu.type = 2;
                if(send(sockfd, (char *)&stu, sizeof(stu), 0) == -1)
                {
                    printf("send() failed!\n");
                    close(sockfd);
                    return -1;
                }
                connected=0;
                threadpool_destroy(pool);
                end = 1;
                return 0;
            default:
                break;
        }
    }

}