
CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -O -g

ARCHIVE = TP4.tar.gz

BIN =	client \
	serveur
TOOLS = utils

CSRC = 	$(BIN:%=%.c) \
	$(TOOLS:%=%.c)

HSRC = 	$(TOOLS:%=%.h) \

OBJ = $(CSRC:%.c=%.o)
OBJ2 = $(TOOLS:%=%.o)

.PHONY: all clean tar

all: $(BIN)

tar: $(ARCHIVE)

clean:
	-$(RM) .depend *~
	-$(RM) $(OBJ) $(BIN) $(ARCHIVE)

$(BIN): $(OBJ2)

$(ARCHIVE): Makefile $(CSRC) $(HSRC)
	$(MAKE)
	tar zcf $@ $^

ifneq ($(MAKECMDGOALS),clean)

.depend: $(CSRC) $(HSRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MM $^ > $@

-include .depend

endif
