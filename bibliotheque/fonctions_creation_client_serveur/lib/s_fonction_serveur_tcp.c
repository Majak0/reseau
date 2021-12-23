/*
  Server TCP : 
              int creer_serveur_tcp ( int port, int max_client, int debug)
	      int attendre_client_tcp ( int socket_serveur, int debug)
*/

#include "./client__serveur.h"

extern int errno;

static struct sockaddr_in client , serveur;

int creer_serveur_tcp ( int port, int max_client, int debug){


    int soket, retour;
 
    if ( debug) {
        fprintf(stderr,"%s\n%s\n%s\n%s\n",VERSION,AUTEUR,COPYRIGHT,LICENCE);
    }

        /*
          Creation de la socket TCP
        */
    if (debug) {
        fprintf(stderr,
                "Serveur: appel de la primitive socket avec les parametres AF_INET, SOCK_STREAM, 0\n");
    }
    
    soket = socket( AF_INET, SOCK_STREAM, 0);
    if ( soket < 0 ){
        perror("Impossible de creer la socket : ");
        return (-1);
    }
    
    if (debug) {
        fprintf(stderr,
                "Serveur: retour positif de la primitive socket avec la valeur %d\n",soket);
    }
    
    
        /*
          Bind de l'adresse locale
        */
    bzero( (char *) &serveur, sizeof(serveur)); /* RAZ */
    serveur.sin_family = AF_INET;     /* type socket */


       /*
            on accepte les connexions venant de n importe ou
       */
    
    serveur.sin_addr.s_addr = htonl (INADDR_ANY);
    serveur.sin_port = htons(port);

    if (debug) {
        fprintf(stderr,"Serveur: appel de la primitive bind pour le port %d\n", port);
    }
    
    retour=bind(soket, (struct sockaddr *) &serveur, sizeof(serveur));
    if ( retour  < 0 ){
        perror("Serveur: echec de la reservation du port : ");
        return ( -2);
    }
    
    if (debug) {
        fprintf(stderr,"Serveur: retour de de la primitive bind avec le resultat : %d\n",retour);
    }
    
    /*
      On accepte 5 connections ( maximum )
    */
    listen( soket, max_client);

    if (debug) {
        fprintf(stderr,"Serveur: retour de de la primitive listen\n");
        fprintf(stderr,"Serveur: fin de la fonction creation_serveur_tcp et retour\n");
    }    
    
    return(soket);
}


int attendre_client_tcp ( int socket_serveur, int debug){
    int clilen, socket_client;
    static int no_client=0;
    
        /*
          Attente de la connexion d'un client
        */
    clilen = sizeof(client);
    if ( debug ) {
        fprintf(stderr,"Serveur: attente de la connexion d\'un client\n");
    }

    socket_client = accept (socket_serveur, (struct sockaddr *)  &client, &clilen);
    no_client++;

    if ( debug ) {    
        fprintf(stderr,"Serveur: connecte au client %d par descripteur %d\n", no_client, socket_client  );
    }
    

    if ( socket_client < 0 ) {
        perror(
            "Serveur: probleme durant de la primitive accept par le serveur :");
        return(-1);
    }
    if ( debug ) {
        fprintf(stderr,"Serveur: fin de la fonction attendre_client_tcp\n");
    }
    
    return(socket_client);
}

char *  adresse_client(){
    return inet_ntoa (client.sin_addr);
}
