
## Chemin vers la bibiothèque client_serveur. Modifier si besoin.
## Utiliser la valeur . pour le répertoire courant.
CS_DIR = ../../Client_Serveur

CC = gcc
CPPFLAGS = -I$(CS_DIR)
CFLAGS = -std=gnu11 -Wall -Wextra -O -g

ARCHIVE = TP2.tar.gz

BIN =	$(addprefix client, 1 2 2b 3) \
	$(addprefix serveur, 1 2 2b 3)
TOOLS = common utils

CSRC = 	$(BIN:%=%.c) \
	$(TOOLS:%=%.c)

HSRC = 	$(TOOLS:%=%.h)

OBJ = $(CSRC:%.c=%.o)
OBJ2 = $(TOOLS:%=%.o)

CS_BASE = $(CS_DIR)/client_serveur
CS_ARCHIVE = $(CS_BASE).a
CS_HEADER  = $(CS_BASE).h

.PHONY: all clean tar

all: $(BIN)

tar: $(ARCHIVE)

clean:
	-$(RM) .depend *~
	-$(RM) $(OBJ) $(BIN) $(ARCHIVE)

$(BIN): $(CS_ARCHIVE) $(OBJ2)

$(ARCHIVE): README Makefile $(CSRC) $(HSRC)
	$(MAKE)
	tar zcf $@ $^

ifneq ($(MAKECMDGOALS),clean)

ERR=0
ifeq ($(wildcard $(CS_HEADER)),)
$(warning $(CS_HEADER) non trouvé)
ERR=1
endif
ifeq ($(wildcard $(CS_ARCHIVE)),)
$(warning $(CS_ARCHIVE) non trouvé)
ERR=1
endif
ifeq ($(ERR),1)
$(info ,----)
$(info | Veuillez éditer la variabe CS_DIR au début du fichier Makefile.)
$(info `----)
$(error )
endif

.depend: $(CSRC) $(HSRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MM $^ > $@

-include .depend

endif
