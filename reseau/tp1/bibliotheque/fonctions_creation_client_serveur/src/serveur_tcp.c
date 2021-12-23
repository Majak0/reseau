/*
        Server TCP utilisant les fonctions :

        int creer_serveur_tcp ( int port, int max_client, int debug)
        int attendre_client_tcp ( int socket_serveur, int debug)
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "./client_serveur.h"

void  fin_fils (int n_signal){
    if ( n_signal != SIGCHLD )
        fprintf(stderr,"Serveur: etonnant la fonction de traitement de SIGCHILD ets appelee avec un signal different de SIGCHILD\n");
    (void) wait((void*) NULL);
    fprintf(stderr,"Serveur: code de retour du fils purge\n");
    (void) signal( SIGCHLD, fin_fils);
    return ;
}

    

int main ( int argc, char ** argv ) {
    int socket_serveur, socket_client, fils, retour;
    char message[256];
    char * salut = "Bonjour client";

        /* Capture du signal de fin de fils */
    (void) signal( SIGCHLD, fin_fils);
    socket_serveur = creer_serveur_tcp ( 6666 , 4 , 1);
    if ( socket_serveur < 0 ) {
        fprintf(stderr,"Echec de la creation du serveur\n");
        exit(EXIT_FAILURE);
    }

    while ( 1 ) {
        
        socket_client = attendre_client_tcp ( socket_serveur, 1);
    
        if ( socket_client < 0 ) {
            fprintf(stderr, "Echec de l\'attente du client\n");
            ( void )close( socket_serveur);
            exit(EXIT_FAILURE);
        }
    
        
        if ( (fils = fork()) < 0 ){
            fprintf(stderr, "Echec du fork\n");
            ( void ) close( socket_serveur);
            ( void ) close( socket_client);
            exit (EXIT_FAILURE);
        }

        if ( fils == 0 )
        {
            ( void ) close(socket_serveur);
            retour = read(socket_client, message, 16);
            if ( retour != 16 ) {
                fprintf(stderr,"Le serveur n\a pas recu le message de 14 car\n");
                (void) close( socket_client);
                exit(EXIT_FAILURE);
            }

            fprintf(stdout, "Message recu : %s\n", message);
       
            retour = write( socket_client, salut, 15);
            if ( retour != 15 ) {
                fprintf(stderr, "Echec de l\'evoi du message au client\n");
                (void) close( socket_client);
                exit(EXIT_FAILURE);
            }
            (void) close( socket_client);
            exit(EXIT_SUCCESS);
        }
        fprintf(stderr, "Serveur : fermeture de la socket client\n");
        close( socket_client);
    }
}
