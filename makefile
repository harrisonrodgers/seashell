###
# @file makefile
# @brief Compiles the application.
# @author Harrison Rodgers
# @version 0.1
# @date 2015-04-23
##

CC=clang
CFLAGS=-std=gnu11 -g -Weverything

.PHONY: clean

all: seashell

shell: seashell.c seashell.h signals.c builtins.c
	$(CC) $(CFLAGS) $< -o my$@

clean:
	rm -f seashell
