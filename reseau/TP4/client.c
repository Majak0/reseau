#include "utils.h"
#include <libgen.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

static int debug = 0;

/* Affiche l'aide et quitte le programme.
 */
noreturn void usage(char *progname)
{
    fprintf(stderr,
            "Usage: %s [options] serveur port\n"
            "Options:\n"
            "  -h        affiche cette aide\n"
            "  -d        active les messages de debug\n"
            "  serveur   adresse du serveur\n"
            "  port      port d'écoute du serveur\n",
            basename(progname));
    exit(EXIT_SUCCESS);
}

/* Fait la résolution de nom : nom de domaine -> adresse IP.
 */
struct in_addr resolve_name_to_addr(const char *name)
{
    struct addrinfo hints = {
        .ai_flags = 0,
        .ai_family = AF_INET,
        .ai_socktype = 0,
        .ai_protocol = 0,
    };
    struct addrinfo *addr;
    int error = getaddrinfo(name, NULL, &hints, &addr);
    die_if(error, "getaddrinfo(\"%s\"): %s", name, gai_strerror(error));
    struct in_addr res = ((struct sockaddr_in *)addr->ai_addr)->sin_addr;
    freeaddrinfo(addr);
    return res;
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
    die_if(argc < optind + 2, "serveur et/ou numéro de port manquant");
    const char *server = argv[optind];
    const int port = atoi(argv[optind + 1]);

    /* Encodage de l'adresse du serveur. */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = resolve_name_to_addr(server);

    msg("Serveur: %s (%s), port %d", server,
        inet_ntoa(server_addr.sin_addr), (int)htons(server_addr.sin_port));

    /* Création de la socket et connexion au serveur. */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    die_if(sock < 0, "erreur de création de socket");
    int error;
    error = connect(sock, (struct sockaddr *)&server_addr, sizeof server_addr);
    die_if(error != 0, "ne peut se connecter au serveur");

    /* Boucle principale : lecture d'une ligne sur stdin / envoi via la
     * socket. */
    char *buf = NULL;
    size_t bufsize = 0;
    ssize_t nread;
    while ((nread = getline(&buf, &bufsize, stdin)) != -1) {
        if (debug) {
            if (buf[nread - 1] == '\n')
                msg("Lu %zd car.: \"%.*s\\n\"", nread, nread - 1, buf);
            else
                msg("Lu %zd car.: \"%.*s\"", nread, nread, buf);
        }
        ssize_t nwrit = write(sock, buf, nread);
        if (nwrit != nread) {
            err("problème d'écriture sur la socket");
            break;
        }
    }
    if (nread == -1 && !feof(stdin))
        err("erreur de lecture");
    msg_if(debug, "Fin de la lecture");
    error = close(sock);
    die_if(error == -1, "erreur à la fermeture de la socket");

    return EXIT_SUCCESS;
}
