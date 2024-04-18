#Copyright by Ana Stanciulescu 312CA 25.12.2023

CC= gcc
CFLAGS= -g -Wall -Wextra -std=c99 -pedantic -ggdb3

TARGETS = image_editor

build: $(TARGETS)

image_editor: image_editor.c
	$(CC) $(CFLAGS) image_editor.c -o image_editor -lm
pack:
	zip -FSr 312CA_StanciulescuAna_Tema3.zip README Makefile *.c *.h

clean: 
	rm -f $(TARGETS)

.PHONY: pack clean