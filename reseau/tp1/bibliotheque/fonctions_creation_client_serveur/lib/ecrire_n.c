/*      Ecrit les n caracteres sur le descripteur fd
        Reitere l operation d ecriture tant que les n caracteres n ont pas
        ete ecrits et que la primitive write ne retourne pas un resultat
        negatif = erreur ou nul = fin de fichier.
        
        Retour : nombre de caracteres ecrits
                 < 0 en cas d erreur
*/
#include "./client__serveur.h"

int ecrire_n ( fd , ptr , noctets )
register int fd ;     /* Descripteur a ecrire : fichier, socket, tube ... */
register char * ptr;  /* Tampon contenant les caracteres a ecrire */
register int noctets; /* Nombre de caracteres a ecrire */
{
    int restant ;      /* Nombre de caracteres restant */
    int necrits;       /* Nombre de caracteres ecrits */ 
    restant=noctets;
    while ( restant > 0 )
    {
        necrits = write ( fd , ptr , restant );
        if ( necrits < 0 )    /* Erreur durant l ecriture */
        {
            perror("ecrire_n : erreur durant l ecriture\n");
            return necrits;
        }
        if ( necrits == 0 )
                break;      /* Fin de fichier */
        /* On continue l ecriture  */
        restant -= necrits;     /* nombre de caracteres restants */
                 /* on pointe sur le prochain caractere a ecrire */
        ptr += necrits;
    }
    /* C est fini on retourne le nombre de caracteres ecrits */
   return ( noctets - restant );
}

