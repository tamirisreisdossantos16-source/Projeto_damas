# Makefile - Jogo de Damas 10x10
# Fundamentos de Programacao CK0211 - 2026.1

CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c99 -g
TARGET  = damas
OBJS    = main.o tabuleiro.o jogada.o modos.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o:      main.c modos.h
	$(CC) $(CFLAGS) -c $<

tabuleiro.o: tabuleiro.c tabuleiro.h
	$(CC) $(CFLAGS) -c $<

jogada.o:    jogada.c jogada.h tabuleiro.h
	$(CC) $(CFLAGS) -c $<

modos.o:     modos.c modos.h tabuleiro.h jogada.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
