#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAXLINE 1024

// Driver code
int main(int argc, char *argv[]) {

    int sockfd;
    char buffer[MAXLINE];
    char *hello ;
    long int adrIp;
    inet_pton(AF_INET, argv[1], (void *)&adrIp);
    unsigned short port = strtol(argv[2], NULL, 10);
    if (argc > 2) {
        printf(" ");
        for (int i = 3; i < argc; ++i) {
            strcat(hello, argv[i]);
            strcat(hello, " ");
        }
    }
    else if(argc < 3)
    {
        hello = "No text";
    }

    struct sockaddr_in     servaddr;

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = adrIp;

    int n, len;

    if ( sendto(sockfd, (const char *)hello, strlen(hello),
           MSG_CONFIRM, (const struct sockaddr *) &servaddr,
           sizeof(servaddr)) ==-1)
    {perror("sendto"); return -1;}


    n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *) &servaddr,
                 &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);

    close(sockfd);
    return 0;
}