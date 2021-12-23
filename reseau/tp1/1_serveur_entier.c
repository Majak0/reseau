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
            "Reçoit une valeur entière d'un client, et l'affiche.\n"
            "\n"
            "Usage: %s [-h] [-d] [port]\n"
            "Paramètres:\n"
            "  -h        affiche cette aide\n"
            "  -d        active les messages de debug de la bilbiothèque\n"
            "  port      port d'écoute (défaut: %d)\n",
            basename(progname), DEFAULT_PORT);
    exit(EXIT_FAILURE);
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

    msg("Attente d'un client");
    int sock = attendre_client_tcp(lsock, debug);
    die_if(sock < 0, NULL);

    msg("Réception de la valeur");
    unsigned valeur;            /* valeur reçue */
    int nread = read(sock, &valeur, sizeof valeur);
    die_if(nread != sizeof valeur, "nread = %d, != %d", nread, sizeof valeur);

    printf("Valeur reçue: %u\n", valeur);

    msg("Fermeture de la connexion");
    die_if(close(sock) != 0, NULL);

    msg("Fermeture du serveur");
    die_if(close(lsock) != 0, NULL);
}
