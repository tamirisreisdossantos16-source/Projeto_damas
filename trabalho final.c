#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 10

#define VAZIA ' '
#define BLOQUEADA '#'
#define PECA_CIMA 'O'
#define DAMA_CIMA '&'
#define PECA_BAIXO '@'
#define DAMA_BAIXO '0'

enum Jogador {
    JOGADOR_CIMA = 0,
    JOGADOR_BAIXO = 1
};

struct Jogada {
    enum Jogador jogador;
    int li;
    int ci;
    int lf;
    int cf;
};

struct ResultadoJogada {
    int valida;
    int captura;
    int linha_capturada;
    int coluna_capturada;
};

struct Tabuleiro {
    char casas[TAM][TAM];
    int capturas_cima;
    int capturas_baixo;
};

int dentro_tabuleiro(int linha, int coluna) {
    return linha >= 0 && linha < TAM && coluna >= 0 && coluna < TAM;
}

int casa_jogavel(int linha, int coluna) {
    return dentro_tabuleiro(linha, coluna) && ((linha + coluna) % 2 == 1);
}