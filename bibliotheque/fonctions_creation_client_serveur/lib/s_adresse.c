#include "./client__serveur.h"

//       Fonction retournant l'adresse IP en notation non pointee
//       et en format reseau de son parametre
//       Utilise la fonction gethostbyname
//       Retourne 0 en cas d'erreur

int adresse (char * nom, int trace){

    struct hostent *adresses;
    struct in_addr adresse;
    int i, max;

    adresses = gethostbyname(nom);
    if ( adresses == (struct hostent *) NULL) {
        if ( trace != 0) {
            fprintf(stderr, "adresse: pas de machine portant le nom %s\n",nom);
        }
        return 0;
    }
        /* Copie de la premiere adresse pour transformation */
    memcpy((void*)&(adresse.s_addr),
           *(adresses->h_addr_list),
               adresses->h_length);

    if ( trace != 0) {
        fprintf(stderr,"adresse : le nom canonique de %s est %s\n", nom
                , adresses->h_name);
        

        fprintf(stderr,
            "adresse: adresse hexadecimale en format interne : %8.8lx \n",
            adresse.s_addr);


            /* Transformation en notation pointee pour affichage */
        fprintf(stderr,"adresse : adresse IP pointee :  %s\n",inet_ntoa(adresse));
    }
        /* Retourne la premiere adresse en format reseau */
    return adresse.s_addr;
}
