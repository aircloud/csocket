/* 
    XiaoTao.Nie 3130104419
    All right reserved;

    usage:
    gcc -pthread main.c threadpool.c -o server
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

#define SERVER_PORT 5555 //侦听端口
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

void* work(void* comfd0)
{

    struct student stu;
    struct student toBeSend;
    int comfd= *(int *)comfd0;
    char tempstr1[25];
    int nLeft = sizeof(stu);
    void *ptr = &stu;
    int ret;
    int end=0;
    time_t timep;
    int j,k;
    char* message;

    int returnComfd;
    char *delim = ",";

    printf("before recv:%d\n",comfd);

    while(!end){
        ret = recv(comfd, ptr, nLeft, 0);
        if(ret>0){
            nLeft -= ret;
            ptr = (char *)ptr + ret;
        }
        if(nLeft<=0){

            switch(stu.type){
                case 2:
                    strcpy(toBeSend.info,"");
                    toBeSend.type=2;
                    send(comfd, (char *)&toBeSend, sizeof(toBeSend), 0);
                    for(k=0;k<20;k++){
                        if(allAddress[k].c_comfd==comfd && allAddress[k].status){
                            allAddress[k].status=false;
                            break;
                        }    
                    }
                    close(comfd);
                    return NULL;
                        
                case 3:
                    time (&timep);
                    strcpy(toBeSend.info,ctime(&timep));toBeSend.type=3;
                    printf("get request for current time:%s", ctime(&timep));
                    send(comfd, (char *)&toBeSend, sizeof(toBeSend), 0);
                    break;
                case 4:
                    strcpy(toBeSend.info,"Ubuntu 16.04.1 LTS\n"); 
                    printf("get request for the server name:%s\n",toBeSend.info);
                    toBeSend.type=4;
                    send(comfd, (char *)&toBeSend, sizeof(toBeSend), 0);
                    break; 
                case 5:
                    strcpy(toBeSend.info,""); 
                    printf("get request for all connectors:\n");
                     for(j=0;j<20;j++){
                        if(allAddress[j].c_comfd!=0){
                                printf("the %d %s:%d,%d\n",j,allAddress[j].ip,allAddress[j].port,allAddress[j].c_comfd);
                                strcat(toBeSend.info,allAddress[j].ip);
                                strcat(toBeSend.info,",");
                                int2str(allAddress[j].port, tempstr1);
                                strcat(toBeSend.info,tempstr1);
                                strcat(toBeSend.info,",");
                                int2str(allAddress[j].c_comfd, tempstr1);
                                strcat(toBeSend.info,tempstr1);
                                strcat(toBeSend.info,",");
                                if(allAddress[j].status)
                                  strcat(toBeSend.info,"online");
                                else  strcat(toBeSend.info,"offline");
                                if(allAddress[j].c_comfd==comfd)
                                    strcat(toBeSend.info,"(you)");
                                strcat(toBeSend.info,"\n");
                        }
                        else continue;
                    }
                    toBeSend.type=5;
                    send(comfd, (char *)&toBeSend, sizeof(toBeSend), 0);
                    printf("%s\n",toBeSend.info);
                    break;
                case 6:
                    message=strtok(stu.info,delim);
                    strcpy(toBeSend.info,message);toBeSend.type=6;
                    if(returnComfd=atoi(strtok(NULL,delim))){
                        printf("get request for send message to %d and the message is\n",returnComfd);   
                        printf("message: %s\n", message);  
                        send(returnComfd, (char *)&toBeSend, sizeof(toBeSend), 0);
                    }
                    else{
                        printf("ERROR:LACK MESSAGE TARGET ID");
                        break;
                    }
                default:
                    break;    
    
            }

            printf("info: %s\ntype:%d\n", stu.info, stu.type);
            nLeft=sizeof(stu);ptr = &stu;
        }
    }
}

int main(void)
{
    int sockfd, comfd[20];
    struct sockaddr_in serverAddr, clientAddr;
    int ret, iClientSize;
    void *ptr;
    int i = 0,j = 0,k = 0;

    struct threadpool *pool = threadpool_init(10, 20);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket() failed! code:%d\n", errno);
        return -1;
    }
    
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

    iClientSize = sizeof(struct sockaddr_in);

    for(j=0;j<20;j++){
        allAddress[j].c_comfd=0;
    }

    while(1){
        if((comfd[i] = accept(sockfd, (struct sockaddr *)&clientAddr,(socklen_t *) &iClientSize)) == -1)
        {
            printf("accept() failed! code:%d\n", errno);
            close(sockfd);
            return -1;
        }
        printf("Accepted client: %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        strcpy(allAddress[i].ip, inet_ntoa(clientAddr.sin_addr));
        allAddress[i].port=ntohs(clientAddr.sin_port);
        allAddress[i].c_comfd=comfd[i];
        allAddress[i].status=true;

        if(comfd[i]>0){
             printf("before thread:%d\n",comfd[i]);
             threadpool_add_job(pool, work, (void *)(&comfd[i]));
             i++;
        }
    }


    // sleep(5);
    // threadpool_destroy(pool);
    return 0;
}