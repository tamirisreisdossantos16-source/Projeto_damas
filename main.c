/* main.c - Ponto de entrada do jogo de Damas 10x10
 * Jogo de Damas - Fundamentos de Programacao CK0211 - 2026.1
 * Prof. Miguel Franklin - Universidade Federal do Ceara
 */

#include <stdio.h>
#include <string.h>
#include "modos.h"

static void exibir_ajuda(const char *nome_programa)
{
    printf("Uso:\n");
    printf("  %s                        -> modo interativo (2 jogadores)\n",
           nome_programa);
    printf("  %s <arquivo_de_jogadas>   -> modo arquivo\n\n",
           nome_programa);
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        /* Sem argumentos: modo interativo */
        modo_interativo();

    } else if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            exibir_ajuda(argv[0]);
        } else {
            /* Argumento e o caminho do arquivo de jogadas */
            modo_arquivo(argv[1]);
        }

    } else {
        fprintf(stderr, "Argumentos invalidos.\n");
        exibir_ajuda(argv[0]);
        return 1;
    }

    return 0;
}
