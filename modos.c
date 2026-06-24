/* modos.c - Modos de jogo: interativo (usuario vs usuario) e arquivo
 * Jogo de Damas - Fundamentos de Programacao CK0211 - 2026.1
 * Prof. Miguel Franklin - Universidade Federal do Ceara
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tabuleiro.h"
#include "jogada.h"
#include "modos.h"

#define TAM_LINHA 64

/* -------------------------------------------------------------------------
 * Helpers
 * ---------------------------------------------------------------------- */

static Jogador proximo(Jogador atual)
{
    return (atual == JOGADOR_CIMA) ? JOGADOR_BAIXO : JOGADOR_CIMA;
}

static const char *nome_jogador(Jogador j)
{
    return (j == JOGADOR_CIMA) ? "Jogador de CIMA (o/O)" : "Jogador de BAIXO (@/&)";
}

static void imprimir_placar(const Tabuleiro *t)
{
    printf("Cima = %d / Baixo = %d\n", t->cap_cima, t->cap_baixo);
}

/*
 * Retorna 1 se a peca em (lin_at, col_at) ainda pode capturar
 * (ignora as demais pecas do mesmo jogador para checar so esta).
 */
static int peca_pode_capturar_encadeado(const Tabuleiro *t,
                                         int lin_at, int col_at,
                                         Jogador j)
{
    Tabuleiro tmp = *t;
    int l, c;

    /* Temporariamente remove as outras pecas do mesmo jogador */
    for (l = 0; l < LINHAS; l++) {
        for (c = 0; c < COLUNAS; c++) {
            if ((l != lin_at || c != col_at) && peca_do_jogador(&tmp, l, c, j))
                tmp.grid[l][c] = VAZIO;
        }
    }

    return tem_captura_disponivel(&tmp, j);
}

/* -------------------------------------------------------------------------
 * Modo 1: interativo
 * ---------------------------------------------------------------------- */

void modo_interativo(void)
{
    Tabuleiro t;
    Jogador   atual;
    char      linha[TAM_LINHA];
    char      resp;
    int       jogo_acabou;

reiniciar:
    tabuleiro_inicializar(&t);
    jogo_acabou = 0;

    /* U.2: perguntar quem joga primeiro */
    printf("\n=== DAMAS 10x10 ===\n");
    printf("Quem faz a primeira jogada? [C = cima / B = baixo]: ");
    fflush(stdout);

    while (1) {
        if (!fgets(linha, TAM_LINHA, stdin)) return;
        resp = toupper((unsigned char)linha[0]);
        if (resp == 'C') { atual = JOGADOR_CIMA;  break; }
        if (resp == 'B') { atual = JOGADOR_BAIXO; break; }
        printf("Opcao invalida. Digite C ou B: ");
        fflush(stdout);
    }

    printf("\nFormato das jogadas: ColLin--ColLin  (ex: B0--C1)\n");
    printf("Digite 'desistir' para encerrar a partida.\n\n");

    tabuleiro_imprimir(&t);

    while (!jogo_acabou) {
        int capturou = 0;
        int encadeando = 1;
        int lin_at = -1, col_at = -1; /* posicao da peca em captura encadeada */

        /* Loop de captura encadeada para o turno atual */
        while (encadeando) {
            imprimir_placar(&t);

            if (tem_captura_disponivel(&t, atual))
                printf("[!] Captura obrigatoria disponivel!\n");

            printf("%s > ", nome_jogador(atual));
            fflush(stdout);

            if (!fgets(linha, TAM_LINHA, stdin)) goto fim;
            linha[strcspn(linha, "\r\n")] = '\0';

            if (strcmp(linha, "desistir") == 0) {
                printf("\n%s desistiu. %s venceu!\n",
                       nome_jogador(atual), nome_jogador(proximo(atual)));
                goto fim;
            }

            Jogada j;
            if (!parsear_jogada(linha, &j)) {
                printf("Jogada inv\xc3\xa1lida.\n");
                continue;
            }

            /* Se estamos em encadeamento, a peca a mover deve ser a mesma */
            if (lin_at >= 0) {
                if (j.lin_ini != lin_at || j.col_ini != col_at) {
                    printf("Jogada inv\xc3\xa1lida.\n");
                    continue;
                }
            }

            ResultadoJogada res = executar_jogada(&t, j, atual, &capturou);

            if (res == JOGADA_INVALIDA) {
                printf("Jogada inv\xc3\xa1lida.\n");
                continue;
            }

            tabuleiro_imprimir(&t);

            if (capturou) {
                lin_at = j.lin_fim;
                col_at = j.col_fim;

                /* Verifica se pode continuar capturando com esta peca */
                if (peca_pode_capturar_encadeado(&t, lin_at, col_at, atual)) {
                    printf("[!] %s deve continuar capturando!\n",
                           nome_jogador(atual));
                    /* Continua o loop: mesmo jogador, mesma peca */
                } else {
                    encadeando = 0; /* Fim do turno */
                }
            } else {
                encadeando = 0; /* Movimento sem captura: fim do turno */
            }
        }

        /* Verifica condicao de vitoria */
        if (jogador_sem_pecas(&t, proximo(atual)) ||
            !tem_jogada_disponivel(&t, proximo(atual))) {
            imprimir_placar(&t);
            printf("\n*** %s venceu! ***\n\n", nome_jogador(atual));
            jogo_acabou = 1;
        } else {
            atual = proximo(atual);
        }
    }

fim:
    /* U.16: perguntar nova partida */
    printf("Deseja iniciar uma nova partida? [S/N]: ");
    fflush(stdout);
    if (fgets(linha, TAM_LINHA, stdin)) {
        if (toupper((unsigned char)linha[0]) == 'S')
            goto reiniciar;
    }
}

/* -------------------------------------------------------------------------
 * Modo 2: arquivo (OFFLINE)
 * ---------------------------------------------------------------------- */

void modo_arquivo(const char *arquivo_entrada)
{
    Tabuleiro t;
    Jogador   atual;
    FILE     *fp;
    char      linha[TAM_LINHA];
    int       num_linha = 0;
    int       jogo_acabou = 0;

    fp = fopen(arquivo_entrada, "r");
    if (!fp) {
        printf("Erro: nao foi possivel abrir o arquivo '%s'.\n",
               arquivo_entrada);
        return;
    }

    tabuleiro_inicializar(&t);

    /* O.2: primeira linha define o jogador inicial */
    if (!fgets(linha, TAM_LINHA, fp)) {
        printf("Arquivo vazio.\n");
        fclose(fp);
        return;
    }
    num_linha = 1;
    linha[strcspn(linha, "\r\n")] = '\0';

    if (strcmp(linha, "C") == 0) {
        atual = JOGADOR_CIMA;
    } else if (strcmp(linha, "B") == 0) {
        atual = JOGADOR_BAIXO;
    } else {
        printf("Jogador inicial inv\xc3\xa1lido.\n");
        fclose(fp);
        return;
    }

    printf("\n=== DAMAS 10x10 - Modo Offline: %s ===\n\n", arquivo_entrada);
    printf("Jogador inicial: %s\n\n", nome_jogador(atual));
    tabuleiro_imprimir(&t);

    /* Variaveis de controle de encadeamento */
    int encadeando = 0;
    int lin_at = -1, col_at = -1;

    while (!jogo_acabou && fgets(linha, TAM_LINHA, fp)) {
        num_linha++;
        linha[strcspn(linha, "\r\n")] = '\0';

        /* Ignora linhas vazias */
        if (linha[0] == '\0')
            continue;

        Jogada j;

        /* O.5: jogada invalida de formato */
        if (!parsear_jogada(linha, &j)) {
            printf("Jogada inv\xc3\xa1lida na linha %d do arquivo de entrada.\n",
                   num_linha);
            continue;
        }

        /* Restringe peca em encadeamento */
        if (encadeando && (j.lin_ini != lin_at || j.col_ini != col_at)) {
            printf("Jogada inv\xc3\xa1lida na linha %d do arquivo de entrada.\n",
                   num_linha);
            continue;
        }

        int capturou = 0;
        ResultadoJogada res = executar_jogada(&t, j, atual, &capturou);

        if (res == JOGADA_INVALIDA) {
            printf("Jogada inv\xc3\xa1lida na linha %d do arquivo de entrada.\n",
                   num_linha);
            continue;
        }

        printf("Linha %d: %s jogou %s\n", num_linha, nome_jogador(atual), linha);
        tabuleiro_imprimir(&t);
        imprimir_placar(&t);
        printf("\n");

        if (capturou) {
            lin_at = j.lin_fim;
            col_at = j.col_fim;

            if (peca_pode_capturar_encadeado(&t, lin_at, col_at, atual)) {
                encadeando = 1;
                /* Nao troca de jogador: aguarda proxima linha */
            } else {
                encadeando = 0;
                lin_at = col_at = -1;

                /* Verifica vitoria */
                if (jogador_sem_pecas(&t, proximo(atual)) ||
                    !tem_jogada_disponivel(&t, proximo(atual))) {
                    jogo_acabou = 1;
                } else {
                    atual = proximo(atual);
                }
            }
        } else {
            encadeando = 0;
            lin_at = col_at = -1;

            if (jogador_sem_pecas(&t, proximo(atual)) ||
                !tem_jogada_disponivel(&t, proximo(atual))) {
                jogo_acabou = 1;
            } else {
                atual = proximo(atual);
            }
        }
    }

    fclose(fp);

    /* O.7: situacao final */
    printf("=== Situacao final ===\n");
    tabuleiro_imprimir(&t);
    imprimir_placar(&t);
    printf("\n");

    /* O.8: mensagem de vitoria */
    if (jogador_sem_pecas(&t, JOGADOR_BAIXO) ||
        (!tem_jogada_disponivel(&t, JOGADOR_BAIXO) && !jogador_sem_pecas(&t, JOGADOR_CIMA)))
        printf("O vencedor \xc3\xa9 o usu\xc3\xa1rio de CIMA.\n");
    else if (jogador_sem_pecas(&t, JOGADOR_CIMA) ||
             (!tem_jogada_disponivel(&t, JOGADOR_CIMA) && !jogador_sem_pecas(&t, JOGADOR_BAIXO)))
        printf("O vencedor \xc3\xa9 o usu\xc3\xa1rio de BAIXO.\n");
}
