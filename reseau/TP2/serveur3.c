#include "common.h"
#include "utils.h"
#include <client_serveur.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/wait.h>
#include <unistd.h>

void sighandler(int sig)
{
    switch (sig) {
    case SIGCHLD:
        wait(NULL);
        break;
    }
}

noreturn void handle_connection(int sock)
{
    char *path = read_path(sock);
    int port2;
    die_if(read(sock, &port2, sizeof port2) != sizeof port2,
           "échec de la lecture du numéro de port retour");
    close(sock);
    msg("Fichier %s demandé sur le port %d", path, port2);

    int sock2 = creer_client_tcp(adresse_client(), port2, debug);
    die_if(sock2 < 0, "échec de la connexion retour vers le client");

    /* Redirection de la sortie standard, puis exécution de "cat fichier". */
    die_if(dup2(sock2, STDOUT_FILENO) == -1, "échec de dup2()");
    close(sock2);
    execlp("cat", "cat", path, (char*)0);
    die("échec de execlp()");
}

int main(int argc, char *argv[])
{
    get_options(&argc, argv, "");

    struct sigaction sa;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    int lsock = creer_serveur_tcp(server_port, BACKLOG, debug);
    die_if(lsock < 0,
           "échec de la création du serveur sur le port %d", server_port);

    while (1) {
        int sock = attendre_client_tcp(lsock, debug);
        die_if(sock < 0, "échec de l'attente de la connexion d'un client");

        switch (fork()) {
        case 0:
            close(lsock);
            handle_connection(sock);
        case -1:
            err("échec de fork()");
            break;
        }
        close(sock);
    }
}
