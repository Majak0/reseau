.PHONY : clean cleanall

all : client_serveur.a libclient_serveur.so

client_serveur.a : fonction_serveur_tcp.o fonction_client_tcp.o \
                   fonction_serveur_udp.o fonction_client_udp.o \
                   lire_n.o ecrire_n.o adresse.o
	@echo; echo; echo "Création de la bibliothèque statique"
	@ar r client_serveur.a fonction_serveur_tcp.o fonction_client_tcp.o \
                              fonction_serveur_udp.o fonction_client_udp.o \
                              adresse.o lire_n.o ecrire_n.o 1>/dev/null 2>&1
	@echo; echo "Contenu de la bibliotheque :"
	@ar t client_serveur.a
	@ranlib	client_serveur.a
	@echo; echo

fonction_serveur_tcp.o fonction_client_tcp.o fonction_serveur_udp.o fonction_client_udp.o lire_n.o ecrire_n.o adresse.o : client__serveur.h

libclient_serveur.so: libclient_serveur.so.1.0 
	@ln -s libclient_serveur.so.1.0 libclient_serveur.so 2>/dev/null

libclient_serveur.so.1.0 : s_fonction_serveur_tcp.o s_fonction_client_tcp.o \
                   s_fonction_serveur_udp.o s_fonction_client_udp.o \
                   s_lire_n.o s_ecrire_n.o s_adresse.o  client__serveur.h
	@echo; echo "Création de la bibliothèque dynamique"
	@rm -f *.so
	@cc -fPIC -shared -Wl,-soname=libclient_serveur.so.1 -o libclient_serveur.so.1.0 s_*.o
	@rm -f s_*.o
	@echo; echo "Terminé"; echo

s_fonction_serveur_tcp.o s_fonction_client_tcp.o s_fonction_serveur_udp.o s_fonction_client_udp.o s_lire_n.o s_ecrire_n.o s_adresse.o : client__serveur.h
	@cc -c -fPIC  $*.c

clean :
	@rm -f *.o *~

cleanall : clean
	@rm -f client_serveur.a *.so*
