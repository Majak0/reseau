/*
    Code réalisé par Maxime Jacquot lui même ce maxi BG suprême :)
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 1024

int main(int argc, char *argv[]) {
    int sockfd;
    char buffer[MAXLINE];
    int port = atoi(argv[1]);
    struct sockaddr_in servaddr, cliaddr;

    // if pour ne pas avoir le warning 'argc jamais utilisé'
    if (argc<0) { printf("Nombre d'arguments : %d",argc);}

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,
              sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int n;
    socklen_t len;
    len = sizeof(cliaddr);  //len is value/result

    n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                 MSG_WAITALL, ( struct sockaddr *) &cliaddr,
                 &len);

    char *client = inet_ntoa(cliaddr.sin_addr);

    char *msgClient;
    msgClient = (char *) malloc(2*sizeof(buffer));
    strcat(msgClient,"Bonjour ");
    strcat(msgClient,buffer);

    buffer[n] = '\0';
    // Affichage des informations du client + message avec 'Bonjour' ajouté au début puis envoie de la réponse du serveur
    printf("CLIENT : %s:%d\n",client, port);
    sendto(sockfd, (const char *)msgClient, strlen(msgClient),
           MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
           len);
    return 0;
}