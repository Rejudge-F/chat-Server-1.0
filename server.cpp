
//
//  main.cpp
//  Server
//
//  Created by 张峰 on 2018/11/28.
//  Copyright © 2018年 张峰. All rights reserved.
//
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG 20
#define MAX_CON_NO 10
#define MAX_DATA_SIZE 1024

int main(int argc,char *argv[]) {
    if(argc != 2) {
        puts("Usage: ./server <PORT>");
        exit(1);
    }
    puts("* * * * * * * * * * * * * * *");
    puts("*       connecting...       *");
    puts("* * * * * * * * * * * * * * *");
    
    int server_sock, client_sock;
    sockaddr_in server_addr, client_addr;
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1) {
        perror("socket");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(server_sock, (sockaddr *) & server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }
    int listening = listen(server_sock, BACKLOG);
    if(listening < 0) {
        perror("listen");
        exit(1);
    }
    socklen_t size = sizeof(client_addr);
    client_sock = accept(server_sock, (sockaddr *) & client_addr, &size);
    if(client_sock < 0) {
        perror("accept");
        exit(1);
    }
    char IP[MAX_DATA_SIZE];
    printf("<IP: %s> has connect!\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, IP, sizeof(IP)));
    
    char send_buff[MAX_DATA_SIZE], recv_buff[MAX_DATA_SIZE];
    int pid = fork();
    if(pid < 0) {
        perror("fork");
        exit(1);
    } else if(pid > 0) {
        while(true) {
            fgets(send_buff, sizeof(send_buff), stdin);
            //printf("#Server: %s\n", send_buff);
            int len = (int)send(client_sock, send_buff, strlen(send_buff), 0);
            if(len != strlen(send_buff)) {
                perror("Server send");
                exit(1);
            }
            memset(send_buff, 0, MAX_DATA_SIZE);
        }
    } else {
        int len = 0;
        while(true) {
            recv(client_sock, recv_buff, MAX_DATA_SIZE, 0);
            len = (int)strlen(recv_buff);
            if(len < 0) {
                perror("Server recvie");
                exit(1);
            }  else {
                recv_buff[len] = '\0';
            }
            printf("#Client: %s", recv_buff);
            memset(recv_buff, 0, MAX_DATA_SIZE);
        }
    }
    close(server_sock);
    close(client_sock);
    return 0;
}


