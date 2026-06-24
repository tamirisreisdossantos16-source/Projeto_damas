/* jogada.c - Parsing, validacao e execucao de jogadas
 * Jogo de Damas - Fundamentos de Programacao CK0211 - 2026.1
 * Prof. Miguel Franklin - Universidade Federal do Ceara
 */

#include <string.h>
#include <stdlib.h>
#include "jogada.h"

/* -------------------------------------------------------------------------
 * Parsing
 * ---------------------------------------------------------------------- */

int parsear_jogada(const char *str, Jogada *j)
{
    /* Formato esperado: "B0--C1"  (6 caracteres exatos) */
    if (!str || strlen(str) < 6)
        return 0;

    /* Coluna inicial: letra A-J */
    if (str[0] < 'A' || str[0] > 'J')
        return 0;

    /* Linha inicial: digito 0-9 */
    if (str[1] < '0' || str[1] > '9')
        return 0;

    /* Separador '--' */
    if (str[2] != '-' || str[3] != '-')
        return 0;

    /* Coluna final: letra A-J */
    if (str[4] < 'A' || str[4] > 'J')
        return 0;

    /* Linha final: digito 0-9 */
    if (str[5] < '0' || str[5] > '9')
        return 0;

    /* Nao deve haver caracteres extras alem do possivel '\n' ou '\r' */
    if (str[6] != '\0' && str[6] != '\n' && str[6] != '\r')
        return 0;

    j->col_ini = str[0] - 'A';
    j->lin_ini = str[1] - '0';
    j->col_fim = str[4] - 'A';
    j->lin_fim = str[5] - '0';

    return 1;
}

/* -------------------------------------------------------------------------
 * Helpers internos
 * ---------------------------------------------------------------------- */

static int eh_adversario(const Tabuleiro *t, int lin, int col, Jogador j)
{
    Casa c = t->grid[lin][col];
    if (c == VAZIO) return 0;
    if (j == JOGADOR_CIMA)
        return (c == PECA_BAIXO || c == DAMA_BAIXO);
    else
        return (c == PECA_CIMA || c == DAMA_CIMA);
}

/* Sinal de um inteiro: -1, 0 ou 1 */
static int sinal(int x)
{
    if (x > 0) return  1;
    if (x < 0) return -1;
    return 0;
}

/* -------------------------------------------------------------------------
 * Validacao e execucao de peca normal
 * ---------------------------------------------------------------------- */

/*
 * Tenta executar movimento de peca normal (nao-dama).
 * Retorna JOGADA_OK ou JOGADA_INVALIDA. Preenche *capturou.
 */
static ResultadoJogada mover_normal(Tabuleiro *t, Jogada j, Jogador atual,
                                     int *capturou)
{
    int dlin = j.lin_fim - j.lin_ini;
    int dcol = j.col_fim - j.col_ini;
    int abs_dlin = abs(dlin);
    int abs_dcol = abs(dcol);

    /* Movimento deve ser diagonal */
    if (abs_dlin != abs_dcol)
        return JOGADA_INVALIDA;

    if (abs_dlin == 1) {
        /* Movimento simples de uma casa */

        /* Peca normal so anda para frente */
        if (atual == JOGADOR_CIMA && dlin <= 0)
            return JOGADA_INVALIDA;
        if (atual == JOGADOR_BAIXO && dlin >= 0)
            return JOGADA_INVALIDA;

        /* Casa destino deve estar vazia */
        if (t->grid[j.lin_fim][j.col_fim] != VAZIO)
            return JOGADA_INVALIDA;

        /* Executa o movimento */
        t->grid[j.lin_fim][j.col_fim] = t->grid[j.lin_ini][j.col_ini];
        t->grid[j.lin_ini][j.col_ini] = VAZIO;
        *capturou = 0;
        return JOGADA_OK;

    } else if (abs_dlin == 2) {
        /* Captura: salta sobre peca adversaria */
        int lin_meio = j.lin_ini + sinal(dlin);
        int col_meio = j.col_ini + sinal(dcol);

        /* A casa do meio deve conter um adversario */
        if (!eh_adversario(t, lin_meio, col_meio, atual))
            return JOGADA_INVALIDA;

        /* A casa destino deve estar vazia */
        if (t->grid[j.lin_fim][j.col_fim] != VAZIO)
            return JOGADA_INVALIDA;

        /* Executa a captura */
        t->grid[j.lin_fim][j.col_fim] = t->grid[j.lin_ini][j.col_ini];
        t->grid[j.lin_ini][j.col_ini] = VAZIO;
        t->grid[lin_meio][col_meio]    = VAZIO;

        if (atual == JOGADOR_CIMA)
            t->cap_cima++;
        else
            t->cap_baixo++;

        *capturou = 1;
        return JOGADA_OK;

    } else {
        /* Deslocamento maior que 2 e invalido para peca normal */
        return JOGADA_INVALIDA;
    }
}

/* -------------------------------------------------------------------------
 * Validacao e execucao de dama
 * ---------------------------------------------------------------------- */

/*
 * Tenta executar movimento de dama.
 * A dama pode andar multiplas casas na diagonal, mas nao pode saltar
 * pecas proprias e captura apenas uma peca adversaria por jogada.
 */
static ResultadoJogada mover_dama(Tabuleiro *t, Jogada j, Jogador atual,
                                   int *capturou)
{
    int dlin = j.lin_fim - j.lin_ini;
    int dcol = j.col_fim - j.col_ini;
    int abs_dlin = abs(dlin);
    int abs_dcol = abs(dcol);

    int slin, scol;
    int lin, col;
    int lin_cap = -1, col_cap = -1; /* posicao da peca a capturar */
    int n_adversarios = 0;

    /* Deve ser diagonal */
    if (abs_dlin != abs_dcol || abs_dlin == 0)
        return JOGADA_INVALIDA;

    slin = sinal(dlin);
    scol = sinal(dcol);

    /* Percorre o caminho entre origem e destino (exclusive) */
    lin = j.lin_ini + slin;
    col = j.col_ini + scol;

    while (lin != j.lin_fim || col != j.col_fim) {
        Casa c = t->grid[lin][col];

        if (c != VAZIO) {
            if (eh_adversario(t, lin, col, atual)) {
                n_adversarios++;
                lin_cap = lin;
                col_cap = col;

                if (n_adversarios > 1)
                    return JOGADA_INVALIDA; /* mais de um adversario no caminho */
            } else {
                /* Peca propria bloqueia */
                return JOGADA_INVALIDA;
            }
        } else if (n_adversarios == 1) {
            /* Casa vazia apos adversario: posicao valida de parada */
            /* Continuamos para ver se o destino e valido */
        }

        lin += slin;
        col += scol;
    }

    /* Verifica a casa destino */
    if (t->grid[j.lin_fim][j.col_fim] != VAZIO)
        return JOGADA_INVALIDA;

    /* Se havia adversario no caminho, o destino deve estar apos ele */
    if (n_adversarios == 1) {
        /* Certifica que o destino esta APOS a peca capturada na mesma diagonal */
        int lin_apos = lin_cap + slin;
        int col_apos = col_cap + scol;
        int valido = 0;

        while (lin_apos != j.lin_fim + slin || col_apos != j.col_fim + scol) {
            if (lin_apos == j.lin_fim && col_apos == j.col_fim) {
                valido = 1;
                break;
            }
            /* Se encontrar outra peca apos a capturada, para */
            if (t->grid[lin_apos][col_apos] != VAZIO)
                break;
            lin_apos += slin;
            col_apos += scol;
        }

        if (!valido)
            return JOGADA_INVALIDA;

        /* Executa captura */
        t->grid[j.lin_fim][j.col_fim] = t->grid[j.lin_ini][j.col_ini];
        t->grid[j.lin_ini][j.col_ini] = VAZIO;
        t->grid[lin_cap][col_cap]      = VAZIO;

        if (atual == JOGADOR_CIMA)
            t->cap_cima++;
        else
            t->cap_baixo++;

        *capturou = 1;
        return JOGADA_OK;
    }

    /* Movimento sem captura */
    t->grid[j.lin_fim][j.col_fim] = t->grid[j.lin_ini][j.col_ini];
    t->grid[j.lin_ini][j.col_ini] = VAZIO;
    *capturou = 0;
    return JOGADA_OK;
}

/* -------------------------------------------------------------------------
 * Interface publica: executar_jogada
 * ---------------------------------------------------------------------- */

ResultadoJogada executar_jogada(Tabuleiro *t, Jogada j, Jogador atual,
                                 int *capturou)
{
    int lin_ini = j.lin_ini;
    int col_ini = j.col_ini;
    int lin_fim = j.lin_fim;
    int col_fim = j.col_fim;

    /* Coordenadas dentro dos limites */
    if (lin_ini < 0 || lin_ini >= LINHAS || col_ini < 0 || col_ini >= COLUNAS)
        return JOGADA_INVALIDA;
    if (lin_fim < 0 || lin_fim >= LINHAS || col_fim < 0 || col_fim >= COLUNAS)
        return JOGADA_INVALIDA;

    /* Origem e destino nao podem ser iguais */
    if (lin_ini == lin_fim && col_ini == col_fim)
        return JOGADA_INVALIDA;

    /* Casas devem ser jogaveis */
    if (!casa_jogavel(lin_ini, col_ini) || !casa_jogavel(lin_fim, col_fim))
        return JOGADA_INVALIDA;

    /* A origem deve ter uma peca do jogador atual */
    if (!peca_do_jogador(t, lin_ini, col_ini, atual))
        return JOGADA_INVALIDA;

    /* Captura obrigatoria: se ha captura disponivel, o movimento deve ser captura */
    if (tem_captura_disponivel(t, atual)) {
        /* O movimento precisa ter distancia 2 (peca normal) ou verificar captura de dama */
        int dlin = abs(lin_fim - lin_ini);

        if (e_dama(t, lin_ini, col_ini)) {
            /* Verificamos apos tentar o movimento se houve captura */
            Tabuleiro copia = *t;
            int cap_temp = 0;
            ResultadoJogada r = mover_dama(&copia, j, atual, &cap_temp);
            if (r != JOGADA_OK || !cap_temp)
                return JOGADA_INVALIDA;
            /* Movimento valido com captura: aplica no tabuleiro real */
            *t = copia;
            *capturou = 1;
            tentar_promover(t, lin_fim, col_fim);
            return JOGADA_OK;
        } else {
            if (dlin != 2)
                return JOGADA_INVALIDA;
        }
    }

    /* Executa o movimento adequado ao tipo de peca */
    ResultadoJogada resultado;

    if (e_dama(t, lin_ini, col_ini))
        resultado = mover_dama(t, j, atual, capturou);
    else
        resultado = mover_normal(t, j, atual, capturou);

    if (resultado == JOGADA_OK)
        tentar_promover(t, lin_fim, col_fim);

    return resultado;
}

/* -------------------------------------------------------------------------
 * Verificacao de capturas e jogadas disponiveis
 * ---------------------------------------------------------------------- */

/*
 * Verifica se uma peca normal em (lin, col) pode capturar em alguma direcao.
 */
static int normal_pode_capturar(const Tabuleiro *t, int lin, int col,
                                 Jogador j)
{
    int dlin, dcol;

    /* Peca normal pode capturar nas 4 diagonais */
    for (dlin = -1; dlin <= 1; dlin += 2) {
        for (dcol = -1; dcol <= 1; dcol += 2) {
            int lm = lin + dlin;
            int cm = col + dcol;
            int ld = lin + 2 * dlin;
            int cd = col + 2 * dcol;

            if (ld < 0 || ld >= LINHAS || cd < 0 || cd >= COLUNAS)
                continue;

            if (eh_adversario(t, lm, cm, j) &&
                t->grid[ld][cd] == VAZIO) {
                return 1;
            }
        }
    }

    return 0;
}

/*
 * Verifica se uma dama em (lin, col) pode capturar em alguma direcao.
 */
static int dama_pode_capturar(const Tabuleiro *t, int lin, int col, Jogador j)
{
    int slin, scol;

    for (slin = -1; slin <= 1; slin += 2) {
        for (scol = -1; scol <= 1; scol += 2) {
            int l = lin + slin;
            int c = col + scol;
            int encontrou_adversario = 0;

            while (l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS) {
                if (t->grid[l][c] != VAZIO) {
                    if (!encontrou_adversario && eh_adversario(t, l, c, j)) {
                        encontrou_adversario = 1;
                    } else {
                        break; /* propria peca ou segundo adversario */
                    }
                } else if (encontrou_adversario) {
                    return 1; /* casa vazia apos adversario */
                }

                l += slin;
                c += scol;
            }
        }
    }

    return 0;
}

int tem_captura_disponivel(const Tabuleiro *t, Jogador j)
{
    int lin, col;

    for (lin = 0; lin < LINHAS; lin++) {
        for (col = 0; col < COLUNAS; col++) {
            if (!peca_do_jogador(t, lin, col, j))
                continue;

            if (e_dama(t, lin, col)) {
                if (dama_pode_capturar(t, lin, col, j))
                    return 1;
            } else {
                if (normal_pode_capturar(t, lin, col, j))
                    return 1;
            }
        }
    }

    return 0;
}

int tem_jogada_disponivel(const Tabuleiro *t, Jogador j)
{
    int lin, col, dlin, dcol;

    if (tem_captura_disponivel(t, j))
        return 1;

    for (lin = 0; lin < LINHAS; lin++) {
        for (col = 0; col < COLUNAS; col++) {
            if (!peca_do_jogador(t, lin, col, j))
                continue;

            if (e_dama(t, lin, col)) {
                /* Dama: verifica se ha alguma casa vazia em qualquer diagonal */
                for (dlin = -1; dlin <= 1; dlin += 2) {
                    for (dcol = -1; dcol <= 1; dcol += 2) {
                        int l = lin + dlin;
                        int c = col + dcol;
                        if (l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS &&
                            t->grid[l][c] == VAZIO)
                            return 1;
                    }
                }
            } else {
                /* Peca normal: so anda para frente */
                dlin = (j == JOGADOR_CIMA) ? 1 : -1;
                for (dcol = -1; dcol <= 1; dcol += 2) {
                    int l = lin + dlin;
                    int c = col + dcol;
                    if (l >= 0 && l < LINHAS && c >= 0 && c < COLUNAS &&
                        t->grid[l][c] == VAZIO)
                        return 1;
                }
            }
        }
    }

    return 0;
}
