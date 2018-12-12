// server.cpp
// create by ReJ
// date: 2018/12/12
#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>
#include <string>
#include <mysql/mysql.h>
#include <string.h>
#include <sys/select.h>
#include <set>
#define max(a, b) (a > b ? a : b)
#define HOST "localhost"
#define USER_NAME "root"
#define PASSWORD "Zhangfeng//0"
#define DATABASE "users"
#define BACKLOG 128
#define MAX_DATA_SIZE 1024

class User {
public:
    User(){};
    User(int _sock, int _id, char * _name, char * _password) {
        id = _id;
        sock = _sock;
        strcpy(name, _name);
        strcpy(password, _password);
    }
    bool operator < (const User & b) const {
        return id < b.id;
    }
    int id;
    int sock;
    char IP[15];
    char name[15];
    char password[20];
};

class Server {
public:
    Server(int port) {
        server_sock = socket(AF_INET, SOCK_STREAM, 0);
        client_sock = socket(AF_INET, SOCK_STREAM, 0);
        memset(&client_addr, 0, sizeof(client_addr));
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        //Work();
    }

    bool mysql_check_login(User & login_user) {
        MYSQL mysql_conn;
        MYSQL_RES * ptr_res;
        MYSQL_ROW result_row;
        char sql[256] = ("select * from clients where id = \'");
        strcat(sql, std::to_string(login_user.id).c_str());
        strcat(sql, const_cast<char *>("\' and user_password = \'"));
        strcat(sql, login_user.password);
        strcat(sql, const_cast<char *>("\'"));
        printf("sql: %s\n", sql);
        mysql_init(&mysql_conn);
        if(mysql_real_connect(&mysql_conn, HOST, USER_NAME, PASSWORD, DATABASE, 0, NULL, CLIENT_FOUND_ROWS)) {
            int res = mysql_query(&mysql_conn, sql);
            if(res) {
                puts("select error");
                return false;
            } else {
                ptr_res = mysql_store_result(&mysql_conn);
                int row = mysql_num_rows(ptr_res);
                if(row == 0) {
                    printf("%d is not valid...\n", login_user.id);
                    return false;
                } else {
                    printf("accept %d...\n", login_user.id);
                    result_row = mysql_fetch_row(ptr_res);
                    strcpy(login_user.name, result_row[1]);
                    return true;
                }
            }
        } else {
            perror("mysql connect failed");
            return false;
        }

    }

    void printtime() {
        time_t timeep;
        time(&timeep);
        printf("%s", ctime(&timeep));
    }

    bool Work() {
        if(bind(server_sock, (sockaddr *) & server_addr, sizeof(server_addr)) < 0) {
            perror("bind failed");
            exit(1);
        }
        puts("success to bind...");
        int lis;
        if((lis = listen(server_sock, BACKLOG)) < 0) {
            perror("listen failed");
            exit(1);
        }
        puts("success to listen server...");
        timeval timeout = {0};
        int server_max_fd = 0, client_max_fd = 0;

        socklen_t size = sizeof(client_addr);

        while(true) {
            FD_ZERO(&server_fd);
            FD_ZERO(&client_fd);
            FD_ZERO(&stdin_fd);
            FD_SET(server_sock, &server_fd);
            server_max_fd = max(server_max_fd, server_sock);
            timeout.tv_usec = 500;
            // select accept
            switch (select(server_max_fd + 1, &server_fd, NULL, NULL, &timeout)) {
                case -1: {
                    perror("select failed");
                    exit(1);
                    break;
                }
                case 0: break;
                default : {
                    if(FD_ISSET(server_sock, &server_fd)) {
                        client_sock = accept(server_sock, (sockaddr * ) & client_addr, &size);
                        if(client_sock < 0) {
                            perror("accept failed");
                            exit(1);
                        }
                        if(read(client_sock, (char *) & client, sizeof(User)) < 0) {
                            perror("read failed");
                            close(client_sock);
                        } else {
                            printf("user_name: %d\nuser_password: %s\n", client.id, client.password);
                            if(mysql_check_login(client)) {
                                client.sock = client_sock;
                                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client.IP, sizeof(client.IP));
                                total_user.insert(client);
                                printf("%s has login...", client.name);
                                printf("now has %d users online...\n", (int)total_user.size());
                                strcpy(send_buff, "yes");
                                write(client_sock, send_buff, sizeof(send_buff));

                            } else {
                                strcpy(send_buff, "no");
                                write(client_sock, send_buff, sizeof(send_buff));
                                printf("Reject login!");
                                break;
                            }
                            memset(send_buff, 0, sizeof(send_buff));
                            client_max_fd = max(client_max_fd, client_sock);
                        }
                    }
                    break;          
                }
            } 
            /***************select accept end********************/ 

            for(auto user : total_user) {
                FD_SET(user.sock, &client_fd);
            }

            /***************select read start*******************/
            switch (select(client_max_fd + 1, &client_fd, NULL, NULL, &timeout)) {
                case -1: {
                    perror("select failed");
                    exit(1);
                }    
                case 0: break;
                default :{
                    for(auto user : total_user) {
                        if(FD_ISSET(user.sock, &client_fd)) {
                            ssize_t recv_len = read(user.sock, recv_buff, MAX_DATA_SIZE);
                            if(recv_len <= 0) {
                                printf("%d has offline...\n", user.id);
                                total_user.erase(user);
                                //close(user.sock);
                            } else {
                                printtime();
                                printf("%s: %s", user.name, recv_buff);
                                memset(recv_buff, 0, sizeof(recv_buff));
                            }
                        }
                    }         
                }
            }
            /************select read end***********************/
            
            FD_SET(STDIN_FILENO, &stdin_fd);

            /************select write start********************/
            
            switch (select(STDIN_FILENO + 1, &stdin_fd, NULL, NULL, &timeout)) {
                case -1: {
                    perror("select failed");
                    exit(1);
                }
                case 0: break;
                default : {
                    if(FD_ISSET(STDIN_FILENO, &stdin_fd)) {
                        read(STDIN_FILENO, send_buff, MAX_DATA_SIZE);
                        for(auto user : total_user) {
                            ssize_t len = write(user.sock, send_buff, strlen(send_buff));
                            if(len < 0) {
                                perror("send faied");
                            } else {
                                printf("send Mesage to %s succenss!\n", user.name);
                            }
                        }
                    }          
                }
            }
            /***************select write end*******************/
        }
    }
private:
    int client_sock;
    int server_sock;
    User client;
    std::set<User> total_user;
    sockaddr_in server_addr, client_addr;
    char IP[MAX_DATA_SIZE], recv_buff[MAX_DATA_SIZE], send_buff[MAX_DATA_SIZE];
    fd_set server_fd, client_fd, stdin_fd;
};

int main(int argc, char * argv[]) {
    if(argc != 2) {
        puts("Usage: ./server <PORT>");
        exit(1);
    }
    Server chat_server(atoi(argv[1]));
    chat_server.Work();
    return 0;
}
