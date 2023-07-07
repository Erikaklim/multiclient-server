#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
 
#define BUFLEN 100
 
int main(int argc, char *argv[])
{

    int bot_fd;
    int PORT;
    char IP[INET6_ADDRSTRLEN];
    struct sockaddr_in6 bot_address;
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    char buffer[BUFLEN];
    char name[BUFLEN];
    int num;
    int len;
    clock_t start, end;
    char patarles[5][40] = {"Ilga iesma bedroziant, suo kepsni nunesa", 
                         "Tyli kiaule gilia sakni knisa           ",
                         "Puotavo, lyg Smetonos valdzia           ",
                         "Ziuri, kaip katinas i lasinius          ",
                         "Dainuok dainele, kol turi jauna sirdele "};

    PORT = atoi(argv[1]);

    printf("Bot is working...\n");

    srand(time(NULL));

    num = rand() % 5;

    bot_address.sin6_family = AF_INET6;
    bot_address.sin6_addr = in6addr_any;
    bot_address.sin6_port = htons(PORT);

    if((bot_fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0){
        perror("failed to create a socket");
        exit(EXIT_FAILURE);
    }

    setsockopt(bot_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    if(connect(bot_fd, (struct sockaddr*)&bot_address, sizeof(bot_address)) < 0){
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    recv(bot_fd, name, BUFLEN, 0);
    send(bot_fd, "Botas", 6, 0);
    recv(bot_fd, name, BUFLEN, 0);

    printf("Bot ready...\n");

    while(1){

        int rl = recv(bot_fd, buffer, BUFLEN, 0);

        int len = 40;

        num = rand() % 5;
        
        if(rl > 0){
            if(buffer != NULL){
                printf("%s", buffer);
                memset(buffer, 0, sizeof(buffer));
            } 

        }else if (rl == 0){
            perror("Connection closed");
            exit(EXIT_FAILURE);
        }else if ( rl == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)){
            if(send(bot_fd, patarles[num], len, 0) < 0){
                perror("Send failed");
                exit(EXIT_FAILURE);
            }
        }else{
            perror("Received failed");
            exit(EXIT_FAILURE);
        }
    }

}