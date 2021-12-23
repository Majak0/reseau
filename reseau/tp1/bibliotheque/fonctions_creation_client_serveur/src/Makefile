.PHONY : all clean cleanall ../lib/client_serveur.a ../lib/libclient_serveur.so.1.0

all : serveur_tcp client_tcp serveur_udp client_udp \
      ../lib/client_serveur.a ../lib/libclient_serveur.so.1.0

serveur_tcp : serveur_tcp.o ../lib/client_serveur.a
	cc -o serveur_tcp serveur_tcp.o ../lib/client_serveur.a

client_tcp : client_tcp.o ../lib/client_serveur.a ../lib/client_serveur.a
	cc -o client_tcp client_tcp.o  ../lib/client_serveur.a


serveur_udp : serveur_udp.o ../lib/client_serveur.a
	cc -o serveur_udp serveur_udp.o ../lib/client_serveur.a

client_udp : client_udp.o ../lib/client_serveur.a
	cc -o client_udp client_udp.o  ../lib/client_serveur.a


../lib/client_serveur.a ../lib/libclient_serveur.so.1.0 : ../lib/client_serveur.h
	make -C ../lib

clean :
	@rm -f *.o *~ serveur_tcp client_tcp serveur_udp client_udp
	@make -C ../lib clean

cleanall : clean
	make -C ../lib cleanall
