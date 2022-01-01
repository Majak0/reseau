// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <libgen.h>
#include <netdb.h>


#define MAXLINE 1024

struct in_addr resolve_name_to_addr(const char *name)
{
    struct addrinfo hints = {
            .ai_flags = 0,
            .ai_family = AF_INET,
            .ai_socktype = 0,
            .ai_protocol = 0,
    };
    struct addrinfo *addr;
    int error = getaddrinfo(name, NULL, &hints, &addr);
    //die_if(error, "getaddrinfo(\"%s\"): %s", name, gai_strerror(error));
    struct in_addr res = ((struct sockaddr_in *)addr->ai_addr)->sin_addr;
    freeaddrinfo(addr);
    return res;
}

// Driver code
int main(int argc, char *argv[]) {
    int sockfd;
    int server = scanf(argv[1],"%d");
    const char *serverCasted = argv[1];
    int port = scanf(argv[2],"%d");
    char buffer[MAXLINE];
    char *msg;


//    for (int i = 3; i < argc; ++i) {
//        strcat(msg, argv[i]);
//        //msg+=argv[i];
//    }
    msg = " kikou";
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n, len;

    sendto(sockfd, (const char *)msg, strlen(msg),
           MSG_CONFIRM, (const struct sockaddr *) &servaddr,
           sizeof(servaddr));
    printf("Hello message sent.\n");

    n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *) &servaddr,
                 &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);

    close(sockfd);
    return 0;
}