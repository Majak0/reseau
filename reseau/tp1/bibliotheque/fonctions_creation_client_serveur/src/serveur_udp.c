/*
  Serveur UDP utilisant la fonction :

  int creer_serveur_udp ( int port, int debug)
*/ 

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include "./client_serveur.h"

int main( int argc, char **argv ) {

    int sockfd,n ;

        /* sockaddr_in structure pour le client */
    struct sockaddr_in client;
    int lg_client=sizeof( client );

    char message[256];                   /* le message du client */
    char * bonjour="Bonjour client";

    sockfd = creer_serveur_udp ( 9999, 1 );
    
    if ( sockfd < 0 ) {
        fprintf(stderr,"Serveur: echec de la fonction  creer_serveur_udp\n");
        exit(EXIT_FAILURE);
    }

        /* Boucle infinie */
    while ( 1) {
            /*
              Lecture de donnees envoyees par le processus client 
            */
        fprintf(stderr,"Serveur: attente de donnees d\'un client\n");
        n = recvfrom ( sockfd, message, 256 , 0 , (struct sockaddr *)&client ,
                       &lg_client);
        if ( n <= 0 ) {
            fprintf( stderr, "Pas de caracteres recus du client");
            (void) close ( sockfd);
            exit (EXIT_FAILURE);
        }
        printf("Message recu du client:%s\n",message);

            /* Envoi du message au client */
        n = sendto ( sockfd, bonjour , strlen(bonjour)+1 , 0, 
                     (struct sockaddr *)&client ,lg_client );
  
        if ( n != strlen(bonjour)+1 ) {
            fprintf(stderr,
                    "Serveur: echec de l\'envoi du message au client\n");
            perror("Serveur: echec de sendto :");
            (void) close ( sockfd);
            exit (EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
