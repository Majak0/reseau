#ifndef UTILS_H
#define UTILS_H

#include <stdnoreturn.h>

/* Affiche "# <message>... \n" sur la sortie d'erreur.  Le message est formaté
 * commme pour printf(3).
 */
void msg(const char *format, ...);

/* Affiche "ATTENTION: <message>.\n" sur la sortie d'erreur.  Le message est
 * formaté commme pour printf(3).
 */
void warn(const char *format, ...);

/* Affiche "ERREUR: <message>.\n" sur la sortie d'erreur.  Le message est
 * formaté commme pour printf(3).  Si errno est différent de 0, le message
 * d'erreur associé est également affiché.  NB: format peut être NULL.
 */
void err(const char *format, ...);

/* Comme err() ci-dessus, mais le programme est terminé par un appel à
 * exit(EXIT_FAILURE).
 */
noreturn void die(const char *format, ...);

/* Appelle respectivement les fonctions msg(...), warn(...), err(...) et
 * die(...) lorsque 'expr' est vrai (différent de 0).
 */
#define msg_if(expr, ...) if (expr) msg(__VA_ARGS__); else (void)0
#define warn_if(expr, ...) if (expr) warn(__VA_ARGS__); else (void)0
#define err_if(expr, ...) if (expr) err(__VA_ARGS__); else (void)0
#define die_if(expr, ...) if (expr) die(__VA_ARGS__); else (void)0

#endif /* !UTILS_H */
