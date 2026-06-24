#ifndef MODOS_H
#define MODOS_H

/* modos.h - Modos de jogo: interativo e leitura de arquivo
 * Jogo de Damas - Fundamentos de Programacao CK0211 - 2026.1
 * Prof. Miguel Franklin - Universidade Federal do Ceara
 */

/*
 * Modo 1: dois jogadores no terminal, jogadas digitadas manualmente.
 */
void modo_interativo(void);

/*
 * Modo 2: lê jogadas de um arquivo txt e executa a partida.
 * arquivo_entrada: caminho para o arquivo com as jogadas.
 */
void modo_arquivo(const char *arquivo_entrada);

#endif /* MODOS_H */
