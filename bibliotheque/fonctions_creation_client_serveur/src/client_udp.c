/*
    Example de client UDP utilisant la fonction 

         creer_client_udp
         int creer_client_udp ( char * adresse, int  port, 
             struct sockaddr_in * pserver, 
             struct sockaddr_in * pclient,int debug )
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "./client_serveur.h"

main( int argc, char **argv ) {
    int socket;             /* descripteur de la socket */
    size_t nombre;         /* longueur message */
    ssize_t n;         /* retour de recvfrom ou sento */
    
    /* structures sockaddr_in pour  serveur et client */
    struct sockaddr_in serveur, client;
    socklen_t lserveur=(socklen_t) sizeof(serveur);
    
    char * message_client="Bonjour serveur";
    char  message_serveur[256];
    
    socket = creer_client_udp ( "127.0.0.1", 9999, &serveur, &client, 1);

    if ( socket < 0 ) {
        fprintf(stderr,"Client: retour de la primitive socket en erreur\n");
        perror("Client: pb de creation socket :");
        return EXIT_FAILURE;
    }
        /* Envoi du message au serveur */
    nombre = (size_t)strlen(message_client)+1;
    n=sendto(socket, (void *) message_client, nombre, 0,
             (struct sockaddr *) &serveur, (socklen_t) sizeof(serveur));
    if ( n != (ssize_t)nombre) {
        fprintf(stderr,"Client : echec de l\envoi du message\n");
        perror("Clent : erreur avec sendto :");
        (void) close (socket);
        return EXIT_FAILURE;
    }

        /* Maintenant on recoit le message du serveur */
    
    n=recvfrom(socket, (void *) message_serveur, (size_t) 15, 0,
               (struct sockaddr *) &serveur, &lserveur);

    if ( n != (size_t) 15 ){
        fprintf(stderr, "Client : reception du message serveur incorrecte\n");
        perror("Client : pb avec recvfrom :");
        (void) close(socket);
        return EXIT_FAILURE;
    }
    printf("Message recu du serveur : %s\n");
    (void) close(socket);
    return EXIT_SUCCESS;
}
    
