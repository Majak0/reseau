/*      Lit le desripteur n jusqu'a ce que n caractères aient ete lus
        ou que 0 soit retourne par la primitive read.
        
        Retour : >= 0 nombre de caractères lus
                 < 0 si une erreur
                      
*/
#include <unistd.h>
#include "./client__serveur.h"

int lire_n ( fd , ptr , noctets )
register int fd ;       /* Descripteur a lire , fichier, socket, tube ...*/
register char * ptr;    /* Pour ranger les caracteres lus */
register int noctets;   /* Nombre de caracteres a lire */
{
    int encore ;       /* Nombre de caracteres restant a lire */
    int nlus;          /* Nombre de caracteres lus */ 
    encore=noctets;
    while ( encore > 0 ){
        nlus = read ( fd , ptr , encore );
        if ( nlus < 0 )    /* Erreur durant la lecture */
        {
            perror("lire_n : erreur durant la lecture");
            return nlus;
        }
        if ( nlus == 0 )
                break;      /* Fin de fichier */
        /* On continue a lire */
        encore -= nlus;     /* nombre de caracteres restant */
        ptr += nlus;        /* ptr sur les caracteres courant dans le tampon */
    }
    /* C est fini  on retourne
       le nombre de caracteres lus <= au nombre demande
       encore peut etre <> 0 */
   return ( noctets - encore );
}

