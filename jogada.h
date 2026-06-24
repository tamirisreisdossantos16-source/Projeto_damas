#ifndef JOGADA_H
#define JOGADA_H

/* jogada.h - Parsing e validacao de jogadas
 * Jogo de Damas - Fundamentos de Programacao CK0211 - 2026.1
 * Prof. Miguel Franklin - Universidade Federal do Ceara
 */

#include "tabuleiro.h"

/* Resultado da validacao de uma jogada */
typedef enum {
    JOGADA_OK         = 0,
    JOGADA_INVALIDA   = 1,
    JOGADA_FIM_JOGO   = 2   /* usado internamente para encerrar partida */
} ResultadoJogada;

/*
 * Faz o parsing de uma string no formato "B0--C1" para uma struct Jogada.
 * Retorna 1 em caso de sucesso, 0 se o formato for invalido.
 */
int parsear_jogada(const char *str, Jogada *j);

/*
 * Valida e executa a jogada no tabuleiro.
 * Retorna JOGADA_OK se a jogada foi executada com sucesso.
 * Retorna JOGADA_INVALIDA se a jogada nao e permitida.
 * Preenche *capturou com 1 se houve captura, 0 caso contrario.
 */
ResultadoJogada executar_jogada(Tabuleiro *t, Jogada j, Jogador atual,
                                 int *capturou);

/*
 * Verifica se existe pelo menos uma captura possivel para o jogador.
 * Retorna 1 se houver, 0 caso contrario.
 */
int tem_captura_disponivel(const Tabuleiro *t, Jogador j);

/*
 * Verifica se o jogador possui alguma jogada legal (captura ou movimento).
 * Retorna 1 se houver alguma jogada, 0 caso contrario.
 */
int tem_jogada_disponivel(const Tabuleiro *t, Jogador j);

#endif /* JOGADA_H */
