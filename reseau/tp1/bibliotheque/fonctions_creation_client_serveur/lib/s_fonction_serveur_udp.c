/*
  Serveur utilisant le protocole UDP
*/

#include "./client__serveur.h"

int creer_serveur_udp ( int port, int debug) {

    int sockfd, retour ;             /* descripteur de la socket */ 
    struct sockaddr_in server_address;
        /* structure sockaddr_in du serveur */

    if ( debug) {
        fprintf(stderr,"%s\n%s\n%s\n%s\n",VERSION,AUTEUR,COPYRIGHT,LICENCE);
    }
        /*
          Creation d'une socket UDP = mode datagram
        */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sockfd  < 0 ) {
        if ( debug ) 
            fprintf(stderr, "Serveur: echec de la creation de la socket\n");
        perror("Serveur: echec de la creation de la socket :");
        return(sockfd);
    }

  /*
    RAZ et initialisation de la structure du serveur
  */
  bzero( (char *) &server_address , sizeof(server_address)); 
  server_address.sin_family = AF_INET;    /* socket INET */

                     /* Toutes les connexions sont acceptees */
  server_address.sin_addr.s_addr = htonl( INADDR_ANY ); 

                     /* le port passe en parametre */
  server_address.sin_port = htons ( port );

  if (debug ) {
      fprintf(stderr, "Serveur: appel de bind pour reservation du port \n");
  }
  retour = bind ( sockfd, (struct sockaddr *) &server_address,
		   sizeof ( server_address));

  if ( retour < 0 ) { 
    perror("Serveur: echec de la primitive bind: ");
    if ( debug ) {
      fprintf(stderr, "Serveur: echec de la reservetion du port\n");
    }
    return(retour);
  }
  /*
    La socket est cree, le port reserve ==> retour a l'appelant de la socket
    a utiliser.
  */
  if ( debug ) {
    fprintf(stderr, 
	    "Serveur : fin de la fonction de creation d\'un serveur UDP\n");
  }
  return (sockfd);
}
