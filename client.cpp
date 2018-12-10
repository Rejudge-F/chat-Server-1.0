// client.cpp
// create by zhangfeng
// date: 2018/12/10

#include <signal.h>
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
#define MAX_DATA_SIZE 4096

struct User {
    char name[20];
    char passwd[20];
};

int main(int argc,char *argv[]) {
    if(argc != 5) {
        puts("Usage: ./Client <IP> <PORT> <user_name> <user_password>");
        exit(1);
    }
    User user;
    strcpy(user.name, argv[3]);
    strcpy(user.passwd, argv[4]);
    
    printf("user_name: %s\nuser_password:%s\n", user.name, user.passwd);
    
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    
    connect(client_sock, (sockaddr *) & server_addr, sizeof(server_addr));
    if(client_sock < 0) {
        perror("connect");
        exit(1);
    }
    
    char send_buff[MAX_DATA_SIZE], recv_buff[MAX_DATA_SIZE];
    
    printf("loading....\n");
    
    // identify
    send(client_sock, (char *) & user, sizeof(user), 0);
    read(client_sock, recv_buff, MAX_DATA_SIZE);
    if(recv_buff[0] == 'n') {
        puts("账号或者密码错误，请重试");
        close(client_sock);
        exit(1);
    } else {
        puts("登录成功");
        memset(recv_buff, 0, MAX_DATA_SIZE);
    }
    
    
    int pid = fork();
    if(pid < 0) {
        perror("fork");
        exit(1);
    } else if(pid == 0) {
        while(true) {
            fgets(send_buff, MAX_DATA_SIZE, stdin);
            //printf("#Client: %s\n", send_buff);
            send(client_sock, send_buff, strlen(send_buff), 0);
            memset(send_buff, 0, sizeof(send_buff));
        }
    } else {
        while(true) {
            ssize_t len = recv(client_sock, recv_buff, MAX_DATA_SIZE, 0);
            if(len <= 0) {
                puts("Server Closed");
                perror("recv");
                close(client_sock);
                exit(1);
            }
            printf("#Server: %s", recv_buff);
            memset(recv_buff, 0, sizeof(recv_buff));
        }
        kill(pid, SIGKILL);
    }
    close(client_sock);
    return 0;
}
