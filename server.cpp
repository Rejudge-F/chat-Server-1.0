#include <iostream>
#include <stdio.h>
#include <set>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <sys/select.h>
#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

const int MAX_CONN_USER = 128;
const int MAX_DATA_SIZE = 1024;

int main(int argc, char * argv[]) {
    if(argc != 2) {
        puts("Usage: ./server <PORT>");
        exit(0);
    }
    fd_set server_fd, client_fd;

    int server_sock, client_sock;
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    printf("Success to init socket...\n");

    std::set<int> User;
    User.clear();

    sockaddr_in server_addr = {0}, client_addr = {0};
    bzero(&server_addr, sizeof(server_addr));
    bzero(&client_addr, sizeof(client_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(server_sock, (sockaddr *) & server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(0);
    }

    printf("Success to bind server with PORT: %d...\n", atoi(argv[1]));

    if((listen(server_sock, MAX_CONN_USER)) < 0) {
        perror("listen");
        exit(0);
    }

    puts("Success to listen server...");

    timeval timeout = {0};
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    socklen_t sinSize = sizeof(client_addr);

    FD_ZERO(&server_fd);
    FD_ZERO(&client_fd);
    FD_SET(server_sock, &server_fd);

    int maxServerFd = server_sock, maxClientFd = 0, conn_user = 0;

    char IP[MAX_DATA_SIZE], sendBuff[MAX_DATA_SIZE], recvBuff[MAX_DATA_SIZE];

    while(1) {
        FD_ZERO(&server_fd);
        FD_ZERO(&client_fd);
        FD_SET(server_sock, &server_fd);
        maxServerFd = max(maxServerFd, server_sock);
        timeout.tv_sec = 1; // time for select
        switch(select(maxServerFd + 1, &server_fd, NULL, NULL, &timeout)) {
            case -1: {
                perror("select1");
                break;
            }
            case 0: break;
            default: {
                if(FD_ISSET(server_sock, &server_fd)) {
                    client_sock = accept(server_sock, (sockaddr *) & client_addr, &sinSize);
                    if(client_sock == -1) {
                        perror("accept");
                        exit(1);
                    }
                    User.insert(client_sock);
                    printf("<IP: %s> <PORT: %d> has Connect the server...\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, IP, MAX_DATA_SIZE), ntohs(client_addr.sin_port));
                    printf("Now has %d Users online...\n", (int)User.size());
                    maxClientFd = max(maxClientFd, client_sock);
                    //FD_SET(client_sock, &client_fd);
                }
                break;
            }
        }

        for(auto user : User) {
            FD_SET(user, &client_fd);
        }

        switch(select(maxClientFd + 1, &client_fd, NULL, NULL, &timeout)) {
            case -1: {
                perror("select2");
                break;
            }
            case 0: {
                break;
            }
            default: {
                for(auto i : User) {
                    if(FD_ISSET(i, &client_fd)) {
                        ssize_t len = 0;
                        len = read(i, recvBuff, sizeof(recvBuff));
                        if(len <= 0) {
                            FD_CLR(i, &client_fd);
                            User.erase(i);
                            close(i);
                            puts("close");
                        } else {
                            recvBuff[(int)len] = '\0';
                            printf("#Client: %s", recvBuff);
                            memset(recvBuff, 0, MAX_DATA_SIZE);
                        }
                    }
                }
                break;
            }
        }

        FD_ZERO(&server_fd);
        FD_SET(STDIN_FILENO, &server_fd) ;

        switch (select(STDIN_FILENO + 1, &server_fd, NULL, NULL, &timeout)) {
            case -1: {
                perror("select3");
                break;
            }
            case 0: break;
            default: {
                if(FD_ISSET(STDIN_FILENO, &server_fd)) {
                    fgets(sendBuff, sizeof(sendBuff), stdin);

                    for(auto user : User) {
                        int len = (int)write(user, sendBuff, sizeof(sendBuff));
                        if(len <= 0) {
                            printf("fail to send Message to User: %d\n", user);
                        } else {
                            printf("success to send Message to User: %d\n", user);
                        }
                    }

                    fflush(stdin);
                    memset(sendBuff, 0, sizeof(sendBuff));
                }
                break;
            }
        }

    }

    for(auto user : User) {
        close(user);
    }
    close(server_sock);
    User.clear();
    return 0;
}
