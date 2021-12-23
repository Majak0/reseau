#include <errno.h>
#include <client_serveur.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "param.h"
#include "utils.h"

/* Le programme s'arrête lorsque 'terminaison_demandee' vaut 1.  Normalement
 * modifié à la réception des signaux SIGINT et SIGUSR1.
 */
volatile sig_atomic_t terminaison_demandee = 0;

/* Affiche l'aide et quitte le programme. */
void usage(char *progname)
{
    fprintf(stderr,
            "Reçoit une liste de valeurs entières d'un client.\n"
            "Calcule la somme des valeurs reçues et la retourne au client.\n"
            "Les clients sont traités en parallèle.\n"
            "\n"
            "Usage: %s [-h] [-d] port\n"
            "Paramètres:\n"
            "  -h        affiche cette aide\n"
            "  -d        active les messages de debug de la bilbiothèque\n"
            "  port      port d'écoute (défaut: %d)\n",
            basename(progname), DEFAULT_PORT);
    exit(EXIT_FAILURE);
}

/* Utilise waitpid() pour acquitter la mort d'un fils.  Affiche le statut
 * correspondant sur la sortie d'erreur.
 */
void fin_fils(void)
{
    int status;
    long pid = waitpid(-1, &status, WNOHANG);
    if (pid == -1) {
        err("waitpid()");
    } else {
        if (WIFEXITED(status))
            msg("Fils %ld terminé, retour = %d", pid, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            msg("Fils %ld tué par signal %d (%s)",
                pid, WTERMSIG(status), sys_siglist[WTERMSIG(status)]);
        else
            msg("Fils %ld: statut inconnu: %#08x", pid,  status);
    }
}

/* Gestionnaire de signal, mutualisé pour tous les signaux.
 *
 * NB: la gestion des signaux telle qu'elle est écrite ici comporte des erreurs.
 * Il n'est normalement pas autorisé d'utiliser fprintf() dans un gestionnaire
 * de signal (cf. notion de fonction "Async-signal-safe"). La fonction fprintf()
 * est indirectement utilisée par les appels à msg() et err().
 */
void signal_handler(int sig)
{
    int saved_errno = errno;    /* permet de restaurer la valeur à la fin */
    switch (sig) {
    case SIGCHLD:
        fin_fils();
        break;
    case SIGINT:
    case SIGUSR1: {
        msg("Signal %d (%s) reçu, terminaison du serveur",
            sig, sys_siglist[sig]);
        terminaison_demandee = 1;
        break;
        }
    }
    errno = saved_errno;
}

/* Fonction de gestion d'un client connecté sur la socket 'sock'.
 */
void gestion_client(int sock)
{
    long self = getpid();       /* pour préfixer les messages */

    msg("[%ld] Réception du tableau", self);
    int nvals;          /* nombre de valeurs reçues */
    int nread1 = read(sock, &nvals, sizeof nvals);
    die_if(nread1 != sizeof nvals,
           "[%ld] nread1 = %d, != %d", self, nread1, sizeof nvals);
    int *vals = malloc(nvals * sizeof vals[0]);
    die_if(vals == NULL,
           "[%ld] allocation du tableau de taille %d", self, nvals);

    int taille = nvals * sizeof vals[0]; /* taille en octets */
    int nread2 = read(sock, vals, taille);
    die_if(nread2 != taille, "[%ld] nread2 = %d != %d", self, nread2, taille);

    /* Calcul de la somme. */
    int somme = 0;
    for (int i = 0; i < nvals; i++)
        somme += vals[i];

    msg("[%ld] Envoi de la somme (%d)", self, somme);
    int nwrit = write(sock, &somme, sizeof somme);
    die_if(nwrit != sizeof somme,
           "[%ld] nwrit = %d != %d", self, nwrit, sizeof somme);

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

    msg("Installation des gestionnaires de signaux");
    struct sigaction sa = {
        .sa_handler = signal_handler,
        .sa_flags   = SA_RESTART,
    };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);
    sa.sa_flags &= ~SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    msg("Création du serveur sur le port %d", port);
    int lsock = creer_serveur_tcp(port, SERVER_BACKLOG, debug);
    die_if(lsock < 0, NULL);

    while (1) {
        msg("Attente d'un client");
        int sock = attendre_client_tcp(lsock, debug);
        if (terminaison_demandee)
            break;
        die_if(sock < 0, NULL);

        msg("Création d'un processus fils");
        long fils = fork();
        die_if(fils < 0, NULL);
        if (fils > 0) {
            msg("Processus fils no.%ld créé", fils);
        } else {
            /* Processus fils (fils == 0) */
            /* Fermeture de la socket d'écoute devenue inutile .*/
            close(lsock);
            gestion_client(sock);
            msg("[%ld] Fermeture de la connexion", (long)getpid());
            die_if(close(sock) != 0, NULL);
            exit(EXIT_SUCCESS);
        }

        msg("Fermeture de la socket de connexion");
        err_if(close(sock) != 0, NULL);
    }
    msg("Fermeture du serveur");
    die_if(close(lsock) != 0, NULL);
}
