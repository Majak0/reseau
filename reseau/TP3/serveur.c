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
#include <unistd.h>
#include <sys/wait.h>

/* Affiche l'aide et quitte le programme.
 */
noreturn void usage(char *progname)
{
    fprintf(stderr,
            "Usage: %s [option] [port]\n"
            "Options:\n"
            "  -h        affiche cette aide\n"
            "  -d        active les messages de debug\n"
            "  -m N      taille maximum du tableau acceptée par le serveur;\n"
            "            utiliser -1 pour ne pas mettre de limite (défaut: %d)\n"
            "  -n N      nombre maximum de valeurs à afficher pour un tableau;\n"
            "            utiliser -1 pour ne ne pas mettre de limite (défaut: %d)\n"
            "  port      port d'écoute du serveur (défaut: %d)\n",
            basename(progname), SERVER_SIZE_MAX, ARRAY_PRINT_MAX, DEFAULT_PORT);
    exit(EXIT_SUCCESS);
}

/* Gestion d'un client via la socket 'sock'.  Accepte des tableaux de taille au
 * plus 'size_max'.  Les affichages sont limités à 'array_print_max' éléments.
 * Retourne EXIT_SUCCESS, ou EXIT_FAILURE en cas de problème.
 */
int gerer_client_tcp(int sock, ssize_t size_max, ssize_t array_print_max)
{
    sprintf(msg_prefix, "[%ld] ", (long)getpid());
    msg("%sConnexion d'un client en cours", msg_prefix);

    /* Réception du tableau. */
    int *tab = NULL;
    ssize_t tsize = recevoir_tableau(sock, &tab, size_max);
    if (tsize == -1) {
        err("erreur pendant la réception de la requête du client");
        return EXIT_FAILURE;
    }
    char prefix[100];
    sprintf(prefix, "- %sAvant", msg_prefix);
    imprimer_tableau(prefix, tab, tsize, array_print_max);

    /* Tassement du tableau. */
    ssize_t tsize2 = tasser_tableau(tab, tsize);
    sprintf(prefix, "- %sAprès", msg_prefix);
    imprimer_tableau(prefix, tab, tsize2, array_print_max);

    /* Envoi du tabluea tassé. */
    int ret = envoyer_tableau(sock, tab, tsize2);
    free(tab);
    if (ret == -1) {
        err("%serreur pendant l'envoi de la réponse au client", msg_prefix);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* Fonction de gestion du signal SIGCHLD.  Appelle waitpid() pour chaque fils
 * mort.
 */
void handle_child(int __attribute__((unused)) sig)
{
    int status;
    int child;
    while ((child = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            msg_if(debug, "processus fils %d terminé normalement (%d)",
                   child, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            msg_if(debug, "processus fils %d terminé anormalement (%s)",
                   child, strsignal(WTERMSIG(status)));
        }
    }
}

int main(int argc, char *argv[])
{
    ssize_t array_print_max = ARRAY_PRINT_MAX;
    ssize_t size_max = SERVER_SIZE_MAX;
    int port = DEFAULT_PORT;

    msg("Programme serveur");

    /* Récupération des paramètres. */
    int opt;
    while ((opt = getopt(argc, argv, "hdm:n:")) != -1) {
        switch (opt) {
        case 'h':
            usage(argv[0]);
        case 'd':
            ++debug;
            break;
        case 'm':
            size_max = strtol(optarg, 0, 0);
            break;
        case 'n':
            array_print_max = strtol(optarg, 0, 0);
            break;
        case '?':
            exit(EXIT_FAILURE);
        }
    }
    if (optind < argc)
        port = atoi(argv[optind]);

    struct sigaction sa;
    sa.sa_handler = handle_child;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    msg("Création du serveur sur le port %d", port);
    int lsock = creer_serveur_tcp(port, SERVER_BACKLOG, debug);
    die_if(lsock < 0, "erreur à la création du serveur");

    /* Boucle d'attente des clients. */
    int sock;
    while ((sock = attendre_client_tcp(lsock, debug)) >= 0) {
        switch (fork()) {
        case -1:
            err("problème de cŕeation d'un processus fils");
            break;
        case 0:                 /* processus fils */
            close(lsock);
            int ret = gerer_client_tcp(sock, size_max, array_print_max);
            msg("[%ld] Fermeture de la connexion", (long)getpid());
            die_if(close(sock) != 0, NULL);
            exit(ret);
        }
        close(sock);
    }
    err("erreur d'attente du client");
    close(lsock);

    /* Attente des derniers fils. */
    while (wait(NULL) > 0 || errno == EINTR) {
    }
    return EXIT_FAILURE;
}
