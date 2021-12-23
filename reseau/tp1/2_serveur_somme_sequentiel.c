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
            "Reçoit une liste de valeurs entières d'un client.\n"
            "Calcule la somme des valeurs reçues et la retourne au client.\n"
            "Les clients sont traités séquentiellement.\n"
            "\n"
            "Usage: %s [-h] [-d] port\n"
            "Paramètres:\n"
            "  -h        affiche cette aide\n"
            "  -d        active les messages de debug de la bilbiothèque\n"
            "  port      port d'écoute (défaut: %d)\n",
            basename(progname), DEFAULT_PORT);
    exit(EXIT_FAILURE);
}

/* Fonction de gestion d'un client connecté sur la socket 'sock'.
 */
void gestion_client(int sock)
{
    msg("Réception du tableau");
    int nvals;              /* nombre de valeurs reçues */
    int nread1 = read(sock, &nvals, sizeof nvals);
    die_if(nread1 != sizeof nvals,
           "nread1 = %d, != %d", nread1, sizeof nvals);
    int *vals = malloc(nvals * sizeof vals[0]);
    die_if(vals == NULL, "allocation du tableau de taille %d", nvals);

    int taille = nvals * sizeof vals[0]; /* taille en octets */
    int nread2 = read(sock, vals, taille);
    die_if(nread2 != taille, "nread2 = %d, != %d", nread2, taille);

    /* Calcul de la somme. */
    int somme = 0;
    for (int i = 0; i < nvals; i++)
        somme += vals[i];

    msg("Envoi de la somme (%d)", somme);
    int nwrit = write(sock, &somme, sizeof somme);
    die_if(nwrit != sizeof somme, "nwrit = %d, != %d", nwrit, sizeof somme);

    free(vals);
}

int main(int argc, char *argv[])
{
    int debug = 0;              /* passé aux fonctions de la bibliothèque
                                 * client_serveur */
    int port = DEFAULT_PORT;    /* port d'écoute */

    /* Récupération des paramètres. */
    if (argc > 1 && strcmp(argv[1], "-h") == 0)
        usage(argv[0]);
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debug = 1;
        argc--;
        argv++;
    }
    if (argc > 1)
        port = strtoul(argv[1], NULL, 0);

    msg("Création du serveur sur le port %d", port);
    int lsock = creer_serveur_tcp(port, SERVER_BACKLOG, debug);
    die_if(lsock < 0, NULL);

    while (1) {
        msg("Attente d'un client");
        int sock = attendre_client_tcp(lsock, debug);
        die_if(sock < 0, NULL);
        gestion_client(sock);
        msg("Fermeture de la connexion");
        err_if(close(sock) != 0, NULL);
    }
    msg("Fermeture du serveur");
    die_if(close(lsock) != 0, NULL);
}
