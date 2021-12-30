#include "array.h"
#include "param.h"
#include "utils.h"
#include <client_serveur.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Retourne "s" si x > 1; "" sinon.
 */
#define ESSE(x) ("s" + ((x) < 2))

/* Affiche l'aide et quitte le programme.
 */
noreturn void usage(char *progname)
{
    fprintf(stderr,
            "Usage: %s [option] size zeros [server [port]]\n"
            "Options:\n"
            "  -h        affiche cette aide\n"
            "  -d        active les messages de debug\n"
            "  -n N      nombre maximum de valeurs à afficher pour un tableau;\n"
            "            utiliser -1 pour ne ne pas mettre de limite (défaut: %d)\n"
            "  size      taille du tableau à envoyer\n"
            "  zeros     nombre de zéros dans le tableau\n"
            "  server    adresse du serveur (défaut: %s)\n"
            "  port      port d'écoute du serveur (défaut: %d)\n",
            basename(progname), ARRAY_PRINT_MAX, DEFAULT_HOST, DEFAULT_PORT);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    ssize_t array_print_max = ARRAY_PRINT_MAX;
    char *server = DEFAULT_HOST;
    int port = DEFAULT_PORT;

    msg("Programme client");
    srand(time(NULL));

    /* Récupération des paramètres. */
    int opt;
    while ((opt = getopt(argc, argv, "hdn:")) != -1) {
        switch (opt) {
        case 'h':
            usage(argv[0]);
        case 'd':
            ++debug;
            break;
        case 'n':
            array_print_max = strtol(optarg, 0, 0);
            break;
        case '?':
            exit(EXIT_FAILURE);
        }
    }
    die_if(argc < optind + 2,
           "taille du tableau et/ou nombre de zéros manquants");

    size_t tsize = strtoul(argv[optind], 0, 0);
    size_t zeros = strtoul(argv[optind + 1], 0, 0);
    if (zeros > tsize)
        zeros = tsize;
    if (optind + 2 < argc)
        server = argv[optind + 2];
    if (optind + 3 < argc)
        port = atoi(argv[optind + 3]);

    /* Création du tableau. */
    int *tab;
    msg("Création du tableau de %zu élément%s avec %zu zéro%s",
        tsize, ESSE(tsize), zeros, ESSE(zeros));
    if ((tab = malloc(tsize * sizeof *tab)) == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }
    remplir_tableau(tab, tsize, zeros);
    imprimer_tableau("- Avant", tab, tsize, array_print_max);

    /* Échange avec le serveur. */
    msg("Connexion au serveur: %s:%d", server, port);
    int sock = creer_client_tcp(server, port, debug);
    die_if(sock < 0, "connexion impossible");

    msg("Envoi du tableau au serveur");
    die_if(envoyer_tableau(sock, tab, tsize) == -1,
           "erreur pendant l'envoi du tableau au serveur");

    msg("Attente de la réponse du serveur");
    ssize_t tsize2 = recevoir_tableau(sock, &tab, tsize);
    close(sock);
    die_if(tsize2 == -1,
          "erreur pendant la réception de la réponse du serveur");

    /* Affichage du résultat. */
    imprimer_tableau("- Après", tab, tsize2, array_print_max);
    free(tab);
    return EXIT_SUCCESS;
}
