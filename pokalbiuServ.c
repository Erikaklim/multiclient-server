#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BACKLOG 10
#define BUFLEN 100

int main(int argc, char const* argv[])
{
    fd_set master; 
    fd_set read_fds; 
    int fdmax;        

    int listener;     
    int newfd;        
    struct sockaddr_in6 server_address, client_address;
    socklen_t addrlen;
    int PORT;
    char buf[256];   
    int nbytes;
    char s[INET6_ADDRSTRLEN];
    int optval = 1;
    char name[BUFLEN];
    char names[10][10] = {};
    int size = sizeof(names)/sizeof(names[0]);
    int i, j;

    PORT = atoi(argv[1]);

    FD_ZERO(&master);   
    FD_ZERO(&read_fds);

    server_address.sin6_family = AF_INET6;
    server_address.sin6_addr = in6addr_any;
    server_address.sin6_port = htons(PORT);
 
    if((listener = socket(AF_INET6, SOCK_STREAM, 0)) < 0){
        perror("failed to create a socket");
        exit(EXIT_FAILURE);
    }

    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))){
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
 
    if(bind(listener, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    FD_SET(listener, &master);

    fdmax = listener;

    int cnt = 0;
    
    for(;;) {
        read_fds = master;
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }
        
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { 
                if (i == listener) {
                 
                    addrlen = sizeof client_address;
                    newfd = accept(listener,
                        (struct sockaddr *)&client_address,
                        &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master);
                        if (newfd > fdmax) {    
                            fdmax = newfd;
                        }

                        getpeername(newfd, (struct sockaddr *)&client_address, &addrlen);
                        if(inet_ntop(AF_INET6, &client_address.sin6_addr, s, sizeof(s))) {  
                            printf("Connection accepted from %s : %d on socket: %d\n",s, ntohs(client_address.sin6_port), newfd);
                        }


                            if(send(newfd, "ATSIUSKVARDA\n", 14, 0) < 0){
                                perror("Send failed");
                                exit(EXIT_FAILURE);
                            } 

                            if(recv(newfd, name, BUFLEN, 0) < 0){
                                perror("Receive failed");
                                exit(EXIT_FAILURE);
                            }

                            strcpy(names[cnt], name);
                            ++cnt;

                            if(send(newfd, "\nVARDASOK\n", 11, 0) < 0){
                            perror("Send failed");
                            exit(EXIT_FAILURE);
  
                        };
                        }
                } else {
                        
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                
                        if (nbytes == 0) {
                           
                            printf("Socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); 
                        FD_CLR(i, &master);
                    } else {

                        char new_message[BUFLEN] = "\nPRANESIMAS";
                        char name[10];
                        
                        strcpy(name, names[i-4]);

                        name[strcspn(name, "\n")] = 0;
                        name[strcspn(name, "\r")] = 0;
                        strcat(new_message, name);
                        strcat(new_message, ": ");
                        buf[strcspn(buf, "\r")] = 0;
                        strcat(new_message, buf);
                        strcat(new_message, "\n");

                        for(j = 0; j <= fdmax; j++) {
                            if (FD_ISSET(j, &master)) {
                                if (j != listener) {
                                    if (send(j, new_message, strlen(new_message), 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }

                        memset(buf, 0, sizeof(buf));

                        
                    }
                }
            }
        }
    } 
    
    return 0;
}

    