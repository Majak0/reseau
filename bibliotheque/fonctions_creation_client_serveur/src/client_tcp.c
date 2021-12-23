/*
  Client TCP : utilisant :

  int creer_client_tcp ( char * adresse, int port, int debug)
*/
#include <stdio.h>
#include <stdlib.h>
#include "./client_serveur.h"


int main ( int argc, char ** argv ){
    int socket, retour;
    char * message = "Bonjour serveur"; /* 16 car avec \0 */
    char message_serveur[256];
    char * serveur;
    int port;


    serveur = "127.0.0.1";
    port = 6666;
    switch ( argc ){
        case 3:
            port = atoi (*(argv+2));
        case 2 :
            serveur=*(argv+1);            
    }
    fprintf(stderr,"Connexion au serveur : %s au port : %d\n", serveur, port);
    socket =  creer_client_tcp ( serveur, port , 1);
    if ( socket < 0 ) {
        fprintf(stderr, "Echec de la connexion au serveur\n");
        (void)close(socket);
        return EXIT_FAILURE;
    }
    retour = write(socket,message, 16 );
    if ( retour !=  16 ) {
        fprintf(stderr, "Echec de l\'envoi du message au serveur\n");
        return EXIT_FAILURE;
    }
    
    retour = read(socket,message_serveur,15);
    if ( retour != 15 ) {
        fprintf(stderr, "Echec de la reception du message du serveur : %d\n", retour);
        return EXIT_FAILURE;
    }

    fprintf(stderr,"Message du serveur : %s\n", message_serveur);

    (void)close(socket);
    return EXIT_SUCCESS;
}
