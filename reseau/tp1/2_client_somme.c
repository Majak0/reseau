#include <client_serveur.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "param.h"
#include "utils.h"

/* Affiche l'aide et quitte le programme.
 */
void usage(char *progname)
{
    fprintf(stderr,
            "Envoie une liste de valeurs à un serveur qui en retourne la somme.\n"
            "La somme retournée est affichée.\n"
            "\n"
            "Usage: %s [-h] [-d] N val1 val2 ... valN [adresse [port]]\n"
            "Paramètres:\n"
            "  -h        affiche cette aide\n"
            "  -d        active les messages de debug de la bilbiothèque\n"
            "  N         nombre de valeurs à envoyer\n"
            "  val1...   liste des valeurs à envoyer\n"
            "  adresse   adresse du serveur (défaut: %s)\n"
            "  port      port d'écoute du serveur (défaut: %d)\n",
            basename(progname), DEFAULT_HOST, DEFAULT_PORT);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int debug = 0;              /* passé aux fonctions de la bibliothèque
                                 * client_serveur */
    char *addr = DEFAULT_HOST;  /* adresse du serveur */
    int port = DEFAULT_PORT;    /* port d'écoute du serveur */

    /* Récupération des paramètres. */
   if (argc > 1 && strcmp(argv[1], "-h") == 0)
        usage(argv[0]);
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debug = 1;
        argc--;
        argv++;
    }
    if (argc < 2)
        usage(argv[0]);

    /* Récupération de la liste des valeurs. */
    int nvals = strtoul(argv[1], NULL, 0);
    die_if(nvals <= 0, "nombre de valeurs invalide: %d", nvals);
    die_if(!(nvals + 2 <= argc),
           "nombre insuffisant de valeurs (%d/%d)", argc - 2, nvals);
    int *vals = malloc(nvals * sizeof vals[0]);
    die_if(vals == NULL, "allocation du tableau de taille %d", nvals);
    int i;
    for (i = 0; i < nvals; i++)
        vals[i] = strtol(argv[i + 2], NULL, 0);
    i += 2;

    /* Adresse:port du serveur */
    if (i < argc)
        addr = argv[i++];
    if (i < argc)
        port = strtol(argv[i++], NULL, 0);

    msg("Connexion au serveur %s:%d", addr, port);
    int sock = creer_client_tcp(addr, port, debug);
    die_if(sock < 0, NULL);

    msg("Envoi du tableau");
    int nwrit1 = write(sock, &nvals, sizeof nvals);
    die_if(nwrit1 != sizeof nvals, "nwrit1 = %d, != %d", nwrit1, sizeof nvals);

    int taille = nvals * sizeof vals[0]; /* taille en octets */
    int nwrit2 = write(sock, vals, taille);
    die_if(nwrit2 != taille, "nwrit2 = %d, != %d", nwrit2, taille);

    msg("Réception de la somme");
    int somme;                  /* somme des valeurs */
    int nread = read(sock, &somme, sizeof somme);
    die_if(nread != sizeof somme, "nread = %d, != %d", nread, sizeof somme);

    printf("Somme reçue: %d\n", somme);

    msg("Fermeture de la connexion");
    die_if(close(sock) != 0, NULL);
    free(vals);
}
