#ifndef TABULEIRO_H
#define TABULEIRO_H

/* Jogo de Damas 10x10 - Fundamentos de Programacao CK0211 - 2026.1
 * Prof. Miguel Franklin - Universidade Federal do Ceara
 */

#define LINHAS  10
#define COLUNAS 10

typedef enum {
    VAZIO      = 0,
    PECA_CIMA  = 1,   /* 'o' */
    DAMA_CIMA  = 2,   /* 'O' */
    PECA_BAIXO = 3,   /* '@' */
    DAMA_BAIXO = 4    /* '&' */
} Casa;

typedef enum {
    JOGADOR_CIMA  = 0,
    JOGADOR_BAIXO = 1
} Jogador;

typedef struct {
    Casa grid[LINHAS][COLUNAS];
    int  cap_cima;   /* pecas capturadas pelo jogador de cima  */
    int  cap_baixo;  /* pecas capturadas pelo jogador de baixo */
} Tabuleiro;

typedef struct {
    int col_ini;
    int lin_ini;
    int col_fim;
    int lin_fim;
} Jogada;

/* Inicializa o tabuleiro com as pecas na posicao inicial */
void tabuleiro_inicializar(Tabuleiro *t);

/* Imprime o tabuleiro no terminal */
void tabuleiro_imprimir(const Tabuleiro *t);

/* Retorna 1 se a casa (lin, col) e jogavel (casa escura), 0 caso contrario */
int casa_jogavel(int lin, int col);

/* Retorna 1 se a peca em (lin, col) pertence ao jogador, 0 caso contrario */
int peca_do_jogador(const Tabuleiro *t, int lin, int col, Jogador j);

/* Retorna 1 se a peca em (lin, col) e uma dama */
int e_dama(const Tabuleiro *t, int lin, int col);

/* Promove peca a dama se ela atingiu o extremo oposto */
void tentar_promover(Tabuleiro *t, int lin, int col);

/* Retorna 1 se o jogador nao tem mais pecas no tabuleiro */
int jogador_sem_pecas(const Tabuleiro *t, Jogador j);

#endif /* TABULEIRO_H */
