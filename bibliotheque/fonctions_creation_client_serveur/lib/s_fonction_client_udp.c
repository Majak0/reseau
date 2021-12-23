/*
  Fonction client utilisant le protocole UDP = mode datagram
*/

#include "./client__serveur.h"

int creer_client_udp ( char * adresse, int  port, 
                       struct sockaddr_in * pserver, 
                       struct sockaddr_in * pclient, 
                       int debug ) {


    int sockfd, retour;             /* Socket et retour primitive*/

    if ( debug) {
        fprintf(stderr,"%s\n%s\n%s\n%s\n",VERSION,AUTEUR,COPYRIGHT,LICENCE);
    }

        /*
          Ceation de la socket UDP
        */
    if ( debug ){
        fprintf(stderr, "Client: appel de la primitive socket\n");
    }
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( debug ){
        fprintf(stderr, "Client: retour de la primitive socket\n");
    }
  if ( sockfd  < 0 ) {
    if ( debug ){
      fprintf(stderr, "Client: retour de la primitive socket avec une erreur\n");
    }
    perror("Client impossible de creer la socket : ");
    return(sockfd);
  }

  /* 
       Initialisation de la structure concernant le client 
  */
  bzero( (char *) pclient , sizeof(*pclient) ); 
  (*pclient).sin_family = AF_INET;    /* Type socket */
  (*pclient).sin_addr.s_addr = htonl(INADDR_ANY); 
                                    /* N'importe quelle adresse */
  (*pclient).sin_port = htons ( 0 );  /* N'importe quel port */

  /*
    Reservation d'un port quelconque pour le client
  */
  if ( debug ){
    fprintf(stderr, 
	    "Client: appel de la primitive bind pour attribution d'un port\n");
  }
  retour = bind ( sockfd, (struct sockaddr *)pclient,
		  sizeof ( *pclient) );

  if ( retour < 0 ){
    if ( debug ){
      fprintf(stderr, "Client: retour de la primitive socket avec une erreur\n");
    }
  perror("Client: echec de la demande d\'attribution d\'un port :");
  (void) close (sockfd);
  }

  /*
    Initialisation de la structure contenant les infos sur le serveur
  */

  /* RAZ de la structure */
  bzero( (char *) pserver, sizeof(*pserver) ); 
  (*pserver).sin_family = AF_INET;    /* Socket UDP */

  (*pserver).sin_addr.s_addr = inet_addr( adresse ); 
                                          /* Adresse passe en parametre */
  (*pserver).sin_port = htons ( port );     
                                          /* Port utilise par le serveur*/

  if ( debug ){
    fprintf(stderr, 
	    "Client: fin de fonction et retour du descripteur de socket\n");
  }
  return (sockfd);
}
    
