#include "array.h"
#include "utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

/* Valeur maximum des éléments pour remplir_tableau().
 */
#define VMAX 99

/* Seuil à partir duquel remplir_tableau() utilise un autre algorithme plus
 * rapide (2^20 == 1024 * 1024).
 */
#define FILL_THRESHOLD (1u << 20)

int debug = 0;
char msg_prefix[100] = "";

void imprimer_tableau(const char *prefix, int *tab, size_t tsize, ssize_t tmax)
{
    size_t i;
    printf("%s (taille = %zu):", prefix, tsize);
    if (tmax < 0 || tsize <= (size_t)tmax) {
        /* Affichage complet du tableau. */
        for (i = 0; i < tsize; i++)
            printf(" %d", tab[i]);
    } else {
        /* Limite l'affichage à 'tmax' éléments. */
        for (i = 0; i < (size_t)tmax / 2; i++)
            printf(" %d", tab[i]);
        printf(" [...]");
        for (/* i */; i < (size_t)tmax; i++)
            printf(" %d", tab[tsize - (size_t)tmax + i]);
    }
    putchar('\n');
}

void remplir_tableau(int *tab, size_t tsize, size_t zeros)
{
    if (zeros > tsize)
        zeros = tsize;
    if (tsize < FILL_THRESHOLD) {
        /* Algorithme simple par mélange. */
        msg_if(debug > 1, "%sRemplissage aléatoire du tableau", msg_prefix);
        size_t i;
        for (i = 0; i < zeros; i++)
            tab[i] = 0;
        for (/* i */; i < tsize; i++)
            tab[i] = 1 + rand() % VMAX;
        for (i = 0; i < tsize; i++) {
            size_t j = i + rand() % (tsize - i);
            size_t tmp = tab[i];
            tab[i] = tab[j];
            tab[j] = tmp;
        }
    } else {
        /* Répartition régulière par l'algorithme de Bresenham (plus rapide). */
        msg_if(debug > 1, "%sRemplissage régulier du tableau", msg_prefix);
        ssize_t e = tsize;
        ssize_t dx = e * 2;
        ssize_t dy = zeros * 2;
        while (tsize-- > 0) {
            if ((e = e - dy) > 0) {
                tab[tsize] = 1 + rand() % VMAX;
            } else {
                tab[tsize] = 0;
                e = e + dx;
            }
        }
    }
}

size_t tasser_tableau(int *tab, size_t tsize)
{
    size_t tsize2 = 0;
    size_t i;
    for (i = 0; i < tsize; i++) {
        if (tab[i] != 0)
            tab[tsize2++] = tab[i];
    }
    return tsize2;
}

int envoyer_tableau(int sock, const int *tab, size_t tsize)
{
    msg_if(debug, "%sEnvoi de la taille du tableau: %zu éléments",
           msg_prefix, tsize);
    unsigned sent_size = tsize;
    warn_if(sent_size != tsize, "%staille tronquée à l'envoi: %u au lieu de %zu",
            msg_prefix, sent_size, tsize); // 'tsize' est trop grand pour un 'unsigned'
    ssize_t nw = write(sock, &sent_size, sizeof sent_size);
    if (nw != sizeof sent_size) {
        err("%serreur d'écriture de la taille du taleau (%zd/%zu)",
            msg_prefix, nw, sizeof sent_size);
        return -1;
    }

    size_t bsize = tsize * sizeof *tab;
    size_t nwrit;
    msg_if(debug, "%sEnvoi du contenu du tableau: %zu octets",
           msg_prefix, bsize);
    write_all(sock, tab, bsize, &nwrit);
    if (nwrit != bsize) {
        err("%s%zd/%zd octets écrits", msg_prefix, nwrit, bsize);
        return -1;
    }
    msg_if(debug, "%sTableau envoyé.", msg_prefix);

    return 0;
}

ssize_t recevoir_tableau(int sock, int **ptab, ssize_t tmax)
{
    msg_if(debug, "%sRéception de la taille du tableau", msg_prefix);
    unsigned recvd_size;
    ssize_t nr = read(sock, &recvd_size, sizeof recvd_size);
    if (nr != sizeof recvd_size) {
        err("%serreur de lecture de la taille du tableau (%zd/%zu)",
            msg_prefix, nr, sizeof recvd_size);
        return -1;
    }
    size_t tsize = recvd_size;
    msg_if(debug, "%sTaille du tableau reçue: %zu éléments", msg_prefix, tsize);
    if (tmax >= 0 && tsize > (size_t)tmax) {
        errno = EMSGSIZE;
        err("%stableau trop grand (%zu > %zd): abandon",
            msg_prefix, tsize, tmax);
        return -1;
    }

    int *tab = *ptab;
    size_t bsize = tsize * sizeof *tab;
    int alloc = 0;
    if (tab == NULL) {
        msg_if(debug, "%sAllocation mémoire: %zu octets", msg_prefix, bsize);
        alloc = 1;
        if ((tab = malloc(bsize)) == NULL) {
            err("%séchec de malloc(%zu)", msg_prefix, bsize);
            return -1;
        }
    }

    size_t nread;
    msg_if(debug, "%sRéception du contenu du tableau: %zu octets",
           msg_prefix, bsize);
    read_all(sock, tab, bsize, &nread);
    if (nread != bsize) {
        err("%s%zd/%zu octets lus", msg_prefix, nread, bsize);
        if (alloc)
            free(tab);
        return -1;
    }
    *ptab = tab;
    msg_if(debug, "%sTableau reçu.", msg_prefix);

    return tsize;
}
