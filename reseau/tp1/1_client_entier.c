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
            "Envoie une valeur entière à un serveur.\n"
            "\n"
            "Usage: %s [-h] [-d] valeur [adresse [port]]\n"
            "Paramètres:\n"
            "  -h        affiche cette aide\n"
            "  -d        active les messages de debug de la bilbiothèque\n"
            "  valeur    valeur à envoyer\n"
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
    unsigned valeur = strtoul(argv[1], NULL, 0);
    if (argc > 2)
        addr = argv[2];
    if (argc > 3)
        port = strtol(argv[3], NULL, 0);

    msg("Connexion au serveur %s:%d", addr, port);
    int sock = creer_client_tcp(addr, port, debug);
    die_if(sock < 0, NULL);

    msg("Envoi de la valeur (%u)", valeur);
    int nwrit = write(sock, &valeur, sizeof valeur);
    die_if(nwrit != sizeof valeur, "nwrit = %d, != %d", nwrit, sizeof valeur);

    msg("Fermeture de la connexion");
    die_if(close(sock) != 0, NULL);
}
