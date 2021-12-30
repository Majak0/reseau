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

    /* Ouverture d'un second port pour la réception. */
    int port2 = 1023;
    int lsock;
    do {
        port2++;
        lsock = creer_serveur_tcp(port2, 1, debug);
    } while (lsock < 0 && port2 < 65535);
    die_if(lsock < 0, "échec d'ouverture du port de réception");

    msg("Demande du fichier %s sur le port %d", path, port2);
    write_path(sock, path);
    die_if(write(sock, &port2, sizeof port2) != sizeof port2,
           "échec de l'envoi du numéro de port");
    close(sock);

    /* Attente de la connexion retour du serveur. */
    int sock2 = attendre_client_tcp(lsock, debug);
    die_if(sock2 < 0, "échec de l'attente de la connexion retour du serveur");
    close(lsock);

    /* Redirection de l'entrée standard, puis exécution de "cat". */
    die_if(dup2(sock2, STDIN_FILENO) == -1, "échec de dup2()");
    close(sock2);
    execlp("cat", "cat", (char*)0);
    die("échec de execlp()");
}
