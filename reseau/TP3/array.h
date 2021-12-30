#ifndef ARRAY_H
#define ARRAY_H

#include <unistd.h>

/* Active les messages de debug si différent de 0.
 */
extern int debug;

/* Préfixe à ajouter au début des messages.
 */
extern char msg_prefix[];

/* Écrit sur la sortie standard les valeurs des éléments du tableau 'tab' de
 * taille 'tsize'.  Écrit au plus 'tmax' valeurs.  L'affichage est précédé de la
 * chaîne 'prefix'.
 */
void imprimer_tableau(const char *prefix, int *tab, size_t tsize, ssize_t tmax);

/* Tasse le tableau 'tab' de taille 'tsize' (supprime les zéros).  Le tassement
 * est fait sur place.  Retourne la nouvelle taille du tableau (nombre
 * d'éléments non nuls).
 */
size_t tasser_tableau(int *tab, size_t tsize);

/* Remplit le tableau 'tab' avec 'tsize' éléments choisis aléatoirement, dont
 * exatement 'zeros' valeurs zéro (0).
 */
void remplir_tableau(int *tab, size_t tsize, size_t zeros);

/* Envoie le tableau 'tab' de taille 'tsize' via la socket 'sock'.  Retourne 0,
 * ou -1 en cas d'erreur.
 */
int envoyer_tableau(int sock, const int *tab, size_t tsize);

/* Attend la réception d'un tableau via la socket 'sock'.  Les éléments sont
 * sockés dans le tableau '*ptab' qui est alloué au préalable si 'ptab' est
 * NULL. Si 'tmax' est différent de -1, reçoit au maximum 'tmax' éléments.'
 * Retourne le nombre d'éléments du tableau, ou -1 en cas d'erreur.
 */
ssize_t recevoir_tableau(int sock, int **ptab, ssize_t tmax);

#endif /* ARRAY_H */
