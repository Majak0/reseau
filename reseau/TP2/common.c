#include "common.h"
#include "utils.h"
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int debug = 0;
int server_port = DEFAULT_PORT;

char *read_path(int sock)
{
    int pathlen;
    ssize_t nread = read(sock, &pathlen, sizeof pathlen);
    die_if(nread != sizeof pathlen,
           "échec de la lecture de la longueur du nom de fichier (%zd/%zu)",
           nread, sizeof pathlen);

    msg("Reçu la longueur du nom de fichier: %d", pathlen);
    char *path = malloc(pathlen + 1);
    die_if(path == NULL, "échec de malloc(%d)", pathlen + 1);

    nread = read(sock, path, pathlen);
    if (nread != pathlen) {
        free(path);
        die("échec de la lecture du nom de fichier (%zd/%d)", nread, pathlen);
    }
    path[pathlen] = '\0';       /* par sécurité */

    msg("nom de fichier reçu: %s", path);
    return path;
}

void write_path(int sock, const char *path)
{
    int pathlen = strlen(path) + 1; /* +1 pour le '\0' terminal */
    msg("envoi de la longueur du nom de fichier: %d", pathlen);
    msg("envoi du nom de fichier: %s", path);
    die_if(write(sock, &pathlen, sizeof pathlen) != sizeof pathlen ||
           write(sock, path, pathlen) != pathlen,
           "échec de l'écriture sur la socket");
}

void get_options(int *argc, char *argv[], const char *usage)
{
    int opt;
    while ((opt = getopt(*argc, argv, "hdp:")) != -1) {
        switch (opt) {
        case 'h':
            fprintf(stderr,
                    "Usage: %s [options]%s%s\n"
                    "Options:\n"
                    "  -h        affiche cette aide\n"
                    "  -d        active les messages de debug de la bilbiothèque\n"
                    "  -p port   numéro de port du serveur [%d]\n",
                    basename(argv[0]), (" " + !*usage), usage, DEFAULT_PORT);
            exit(EXIT_SUCCESS);
        case 'd':
            ++debug;
            break;
        case 'p':
            server_port = atoi(optarg);
            break;
        case '?':
            exit(EXIT_FAILURE);
        }
    }
    *argc -= optind - 1;
    memmove(argv + 1, argv + optind, *argc * sizeof *argv);
}
