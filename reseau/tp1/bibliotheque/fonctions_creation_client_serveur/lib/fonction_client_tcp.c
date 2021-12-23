/*
  Client TCP : la fonction retourne la socket cree < 0 en cas d'erreur
*/

#include "./client__serveur.h"

extern int errno;
int creer_client_tcp ( char * adresse_s, int port, int debug){

    int soket, retour;
    struct sockaddr_in serveur;

    if ( debug) {
        fprintf(stderr,"%s\n%s\n%s\n%s\n",VERSION,AUTEUR,COPYRIGHT,LICENCE);
    }
        /*
          RAZ et initialisation emplissage des la structure sockaddr_in
          avec le type 
        */
    bzero( (char *) &serveur, sizeof(serveur)); 
    serveur.sin_family = AF_INET;
        /* Appel de la fonction adresse */
    retour=adresse(adresse_s,0);
    if (retour == 0 ){  /* Adresse non valide */
        if ( debug ) {    
            fprintf(stderr,"Client: adresse %s invalide\n",adresse_s);
        }
        return -1;
    }
    else
    {
        if ( debug ) {    
            fprintf(stderr,
                    "Client: adresse hexadecimale en format reseau %8.8lx\n",
                    retour);
        }
    }
    
        /*      conversion de l'adresse decimale pointee
                en adresse internet 32 bits
    
                serveur.sin_addr.s_addr = inet_addr(adresse);
        */
    serveur.sin_addr.s_addr = retour;
    serveur.sin_port = htons(port);
        /*
          Open a tcp socket, internet type
        */
    if ( debug ) {    
        fprintf(stderr,"Client: execution de la primitive socket\n");
    }    
    soket = socket( AF_INET, SOCK_STREAM, 0);
    if ( soket < 0 ){
        perror("Client: ne peut creer la socket : ");
        return(soket);
    }
    if ( debug ) {    
        fprintf(stderr,"Client: retour correct de la primitive socket\n");
    }     
        /*
          Connexion au serveur
        */
    if ( debug ) { 
        fprintf(stderr,"Client: tentative de connexion au serveur d\'adresse %s au port %d\n", adresse_s, port);
    }
    
    retour = connect( soket, (struct sockaddr *) &serveur,
                      sizeof(serveur));
    if ( retour  <0 ) {
        if (debug ) {
            perror("Client: le client ne peut se connecter au serveur");
        }
        
        (void) close (soket);
        return(-2);
    }
    if ( debug ) { 
        fprintf(stderr,"Client: connexion au serveur reussie\n");
        fprintf(stderr,"Client: fin de la fonction creer_client_tcp\n");
    }
    
    return(soket);
}
