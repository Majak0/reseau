#include "common.h"
#include "utils.h"
#include <client_serveur.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    get_options(&argc, argv, "serveur fichier");
    die_if(argc != 3, "serveur et/ou fichier manquant sur la ligne de commande");
    char *server = argv[1];
    const char *path = argv[2];

    msg("Connexion au serveur: %s:%d\n", server, server_port);
    int sock = creer_client_tcp(server, server_port, debug);
    die_if(sock < 0, "échec de la connexion au serveur");

    msg("Demande du fichier: %s", path);
    write_path(sock, path);
    close(sock);

    int port2 = server_port - 1;
    msg("Deuxième numéro de port: %d", port2);
    int sock2 = -1;
    int retry = RETRY;
    do {
        sock2 = creer_client_tcp(server, port2, debug);
        if (sock2 < 0 && retry > 0) {
            warn("échec de la connexion, attente avant de réessayer (%d)",
                 retry);
            sleep(DELAY);
        }
    } while (sock2 < 0 && retry-- > 0);
    die_if(sock2 < 0, "échec de la connexion au deuxième port du serveur");

    /* Redirection de l'entrée standard, puis exécution de "cat". */
    die_if(dup2(sock2, STDIN_FILENO) == -1, "échec de dup2()");
    close(sock2);
    execlp("cat", "cat", (char*)0);
    die("échec de execlp()");
}
