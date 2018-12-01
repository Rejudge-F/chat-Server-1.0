
//
//  main.cpp
//  Server
//
//  Created by 张峰 on 2018/12/01.
//  Copyright © 2018年 张峰. All rights reserved.

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
#include <queue>
#include <vector>
#include <signal.h>

#define BACKLOG 20
#define MAX_CON_NO 10
#define MAX_DATA_SIZE 1024

static void sig_usr1(int sig) {
    exit(1);
}

int main(int argc, char * argv[]) {
    if(argc != 2) {
        perror("Usage: ./server <PORT>");
        exit(0);
    }
    
    std::vector<int> que_usr;
    int usr = 0, pid = 0;
    que_usr.clear();
    
    int client_sock, server_sock;
    sockaddr_in server_addr, client_addr;
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    if(client_sock < 0 || server_sock < 0) {
        perror("socket error");
        exit(1);
    }
    puts("success to init server...");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(server_sock, (sockaddr *) & server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }
    puts("success to bind...");
    int listening = listen(server_sock, BACKLOG);
    if(listening < 0) {
        perror("listen");
        exit(1);
    }
    puts("success to listen...");
    socklen_t sinSize = sizeof(client_addr);
    
    char IP[MAX_DATA_SIZE], sendBuff[MAX_DATA_SIZE], recvBuff[MAX_DATA_SIZE];
    
    while(true) {
        client_sock = accept(server_sock, (sockaddr *) & client_addr, &sinSize);
        if(client_sock < 0) {
            perror("accept");
            exit(0);
        } else {
            que_usr.push_back(client_sock);
            usr++;
        }
        printf("<IP: %s> join in.\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, IP, MAX_DATA_SIZE));
        printf("Now has %d users online.\n", usr);
        if(pid != 0) {
            kill(pid, SIGUSR1);
        }
        pid = fork();
        if(pid < 0) {
            perror("fork");
            exit(1);
        } else if(pid == 0) {
            while(1) {
                ssize_t recvSize = read(client_sock, recvBuff, MAX_DATA_SIZE);
                if(recvSize <= 0) {
                    close(client_sock);
                    exit(0);
                } else {
                    recvBuff[recvSize] = '\0';
                }
                printf ("%s : %s", IP, recvBuff);
                memset(recvBuff, 0, sizeof(recvBuff));
            }
        }
        
        pid = fork();
        if(pid < 0) {
            perror("fork");
            exit(1);
        } else if(pid == 0){
            signal(SIGUSR1, sig_usr1);
            while(1) {
                fgets(sendBuff, sizeof(sendBuff), stdin);
                for(int i = 0; i < usr; i++) {
                    write(que_usr[i], sendBuff, strlen(sendBuff));
                }
            }
            memset(sendBuff, 0, sizeof(sendBuff));
        }
    }
    close(client_sock);
    return 0;
}


