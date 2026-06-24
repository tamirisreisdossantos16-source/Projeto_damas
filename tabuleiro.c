/* tabuleiro.c - Representacao e impressao do tabuleiro de Damas 10x10
 * Jogo de Damas - Fundamentos de Programacao CK0211 - 2026.1
 * Prof. Miguel Franklin - Universidade Federal do Ceara
 */

#include <stdio.h>
#include "tabuleiro.h"

/* No tabuleiro 10x10, as casas jogaveis sao aquelas onde (lin + col) e impar */
int casa_jogavel(int lin, int col)
{
    return (lin + col) % 2 != 0;
}

void tabuleiro_inicializar(Tabuleiro *t)
{
    int lin, col;

    t->cap_cima  = 0;
    t->cap_baixo = 0;

    for (lin = 0; lin < LINHAS; lin++) {
        for (col = 0; col < COLUNAS; col++) {
            t->grid[lin][col] = VAZIO;

            if (!casa_jogavel(lin, col))
                continue;

            /* Linhas 0-2: pecas do jogador de cima ('o') */
            if (lin <= 2)
                t->grid[lin][col] = PECA_CIMA;

            /* Linhas 7-9: pecas do jogador de baixo ('@') */
            else if (lin >= 7)
                t->grid[lin][col] = PECA_BAIXO;
        }
    }
}

void tabuleiro_imprimir(const Tabuleiro *t)
{
    int lin, col;
    const char *separador = "+-+-+-+-+-+-+-+-+-+-+";

    /* Cabecalho com letras das colunas */
    printf(" A B C D E F G H I J\n");
    printf("%s\n", separador);

    for (lin = 0; lin < LINHAS; lin++) {
        printf("%d", lin);

        for (col = 0; col < COLUNAS; col++) {
            char c;

            if (!casa_jogavel(lin, col)) {
                c = '#';
            } else {
                switch (t->grid[lin][col]) {
                    case PECA_CIMA:  c = 'o'; break;
                    case DAMA_CIMA:  c = 'O'; break;
                    case PECA_BAIXO: c = '@'; break;
                    case DAMA_BAIXO: c = '&'; break;
                    default:         c = ' '; break;
                }
            }

            printf("|%c", c);
        }

        printf("|%d\n", lin);
        printf("%s\n", separador);
    }

    /* Rodape com letras das colunas */
    printf(" A B C D E F G H I J\n");
}

int peca_do_jogador(const Tabuleiro *t, int lin, int col, Jogador j)
{
    Casa c = t->grid[lin][col];

    if (j == JOGADOR_CIMA)
        return (c == PECA_CIMA || c == DAMA_CIMA);
    else
        return (c == PECA_BAIXO || c == DAMA_BAIXO);
}

int e_dama(const Tabuleiro *t, int lin, int col)
{
    Casa c = t->grid[lin][col];
    return (c == DAMA_CIMA || c == DAMA_BAIXO);
}

void tentar_promover(Tabuleiro *t, int lin, int col)
{
    /* Jogador de baixo chega a linha 0 e vira dama */
    if (lin == 0 && t->grid[lin][col] == PECA_BAIXO) {
        t->grid[lin][col] = DAMA_BAIXO;
        return;
    }

    /* Jogador de cima chega a linha 9 e vira dama */
    if (lin == 9 && t->grid[lin][col] == PECA_CIMA) {
        t->grid[lin][col] = DAMA_CIMA;
    }
}

int jogador_sem_pecas(const Tabuleiro *t, Jogador j)
{
    int lin, col;

    for (lin = 0; lin < LINHAS; lin++) {
        for (col = 0; col < COLUNAS; col++) {
            if (peca_do_jogador(t, lin, col, j))
                return 0;
        }
    }

    return 1;
}
