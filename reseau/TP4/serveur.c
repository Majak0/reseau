#include "utils.h"
#include <errno.h>
#include <libgen.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER_BACKLOG 16
#define BUFSIZE 4096

static int debug = 0;

/* Affiche l'aide et quitte le programme.
 */
noreturn void usage(char *progname)
{
    fprintf(stderr,
            "Usage: %s [options] port\n"
            "Options:\n"
            "  -h        affiche cette aide\n"
            "  -d        active les messages de debug\n"
            "  port      port d'écoute du serveur\n",
            basename(progname));
    exit(EXIT_SUCCESS);
}

/* Fait la résolution de nom inverse : adresse IP -> nom de domaine.
 */
char *resolve_addr_to_name(const struct sockaddr_in *addr)
{
    static char name[256];
    int error = getnameinfo((const struct sockaddr *)addr, sizeof *addr,
                            name, sizeof name, NULL, 0, 0);
    die_if(error, "getnameinfo(%s): %s",
           inet_ntoa(addr->sin_addr), gai_strerror(error));
    return name;
}

/* Fonction de geestion d'un client. */
void handle_client(int sock)
{
    char *buf = malloc(BUFSIZE);
    die_if(buf == NULL, "malloc(%d)", BUFSIZE);
    ssize_t nread;
    while ((nread = read(sock, buf, BUFSIZE)) > 0 || errno == EINTR) {
        if (nread > 0)
            fwrite(buf, 1, nread, stdout);
    }
    free(buf);
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    /* Récupération des paramètres. */
    int opt;
    while ((opt = getopt(argc, argv, "hd")) != -1) {
        switch (opt) {
        case 'h':
            usage(argv[0]);
        case 'd':
            ++debug;
            break;
        case '?':
            exit(EXIT_FAILURE);
        }
    }
    die_if(argc < optind + 1, "numéro de port manquant");
    const int port = atoi(argv[optind]);

    /* Création de la socket. */
    int lsock = socket(AF_INET, SOCK_STREAM, 0);
    die_if(lsock < 0, "erreur de création de socket");

    /* Association de la socket au port donné. */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    int error;
    error = bind(lsock, (struct sockaddr *)&server_addr, sizeof server_addr);
    die_if(error == -1, "ne peut obtenir le port %d",
           (int)htons(server_addr.sin_port));

    /* Mise en écoute de la socket. */
    msg("Serveur en écoute sur le port %d", (int)htons(server_addr.sin_port));
    error = listen(lsock, SERVER_BACKLOG);
    die_if(error != 0, "ne peut mettre la socket en écoute");

    /* Boucle principale : attente des clients. */
    int sock;
    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof client_addr;
    while ((sock = accept(lsock, (struct sockaddr *)&client_addr,
                          &client_addrlen)) != -1) {
        msg("Connexion de %s:%d (%s)",
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),
            resolve_addr_to_name(&client_addr));
        handle_client(sock);
        msg("Fin de la connexion");
        close(sock);
    }
    err("erreur à l'attente d'un client. Terminaison du serveur");
    close(lsock);
    exit(EXIT_FAILURE);
}
