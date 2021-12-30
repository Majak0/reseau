#ifndef COMMON_H
#define COMMON_H

#define DEFAULT_PORT 4021
#define BACKLOG 16
#define RETRY 3
#define DELAY 1

extern int debug;
extern int server_port;

extern char *read_path(int sock);
extern void write_path(int sock, const char *path);
extern void get_options(int *argc, char *argv[], const char *usage);

#endif /* COMMON_H */
