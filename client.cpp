
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
#include <string>

#define BACKLOG 20
#define MAX_CON_NO 10
#define MAX_DATA_SIZE 1024

int main(int argc, char * argv[]) {
    if(argc != 3) {
        puts("Usage: ./client <IP> <PORT>");
        exit(0);
    }
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    if(client_sock < 0) {
        perror("init client");
        exit(1);
    }
    
    puts("success to init client...");
    
    if(connect(client_sock, (sockaddr *) & server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(1);
    }
    
    puts("success connect server...");
    char sendBuff[MAX_DATA_SIZE];
    while(true) {
        fgets(sendBuff, MAX_DATA_SIZE, stdin);
        write(client_sock, sendBuff, strlen(sendBuff));
    }
    return 0;
}
