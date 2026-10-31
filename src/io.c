/**
 * @file io.c
 * @brief Implementação das rotinas de validação segura de entradas e I/O (Requisito R01 e Seção 9).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "io.h"

const char* io_mensagem_erro(StatusIO status) {
    switch (status) {
        case IO_SUCESSO:
            return "Entrada valida.";
        case IO_ERRO_ENTRADA_NULA:
            return "Erro interno: ponteiro nulo fornecido.";
        case IO_ERRO_BUFFER_EXCEDIDO:
            return "Linha digitada excedeu o limite maximo permitido.";
        case IO_ERRO_TEXTO_VAZIO:
            return "O texto nao pode ser vazio.";
        case IO_ERRO_APENAS_ESPACOS:
            return "O texto nao pode conter apenas espacos em branco.";
        case IO_ERRO_TAMANHO_EXCEDIDO:
            return "O texto excedeu o limite maximo de caracteres uteis permitidos.";
        case IO_ERRO_NUMERO_INVALIDO:
            return "Formato numerico invalido (nenhum digito encontrado).";
        case IO_ERRO_CARACTERES_EXTRAS:
            return "Caracteres adicionais invalidos encontrados apos o numero.";
        case IO_ERRO_OVERFLOW:
            return "Valor numerico fora dos limites suportados (overflow/underflow).";
        case IO_ERRO_ID_INVALIDO:
            return "ID invalido: deve ser um numero inteiro positivo (> 0).";
        case IO_ERRO_ANO_INVALIDO:
            return "Ano de publicacao invalido: deve estar entre 1 e 2026.";
        case IO_ERRO_EOF:
            return "Fim de entrada alcancado ou erro de leitura no stream.";
        default:
            return "Erro desconhecido na entrada de dados.";
    }
}

StatusIO io_validar_texto(const char *str, int max_caracteres_uteis) {
    size_t i;
    bool tem_caractere_visivel = false;
    size_t len;

    if (str == NULL) {
        return IO_ERRO_ENTRADA_NULA;
    }

    if (str[0] == '\0') {
        return IO_ERRO_TEXTO_VAZIO;
    }

    len = strlen(str);
    if (len > (size_t)max_caracteres_uteis) {
        return IO_ERRO_TAMANHO_EXCEDIDO;
    }

    for (i = 0; i < len; i++) {
        if (!isspace((unsigned char)str[i])) {
            tem_caractere_visivel = true;
            break;
        }
    }

    if (!tem_caractere_visivel) {
        return IO_ERRO_APENAS_ESPACOS;
    }

    return IO_SUCESSO;
}

StatusIO io_validar_inteiro(const char *str, long *valor_saida) {
    const char *p;
    char *endptr = NULL;
    long val;

    if (str == NULL) {
        return IO_ERRO_ENTRADA_NULA;
    }

    p = str;
    while (*p != '\0' && isspace((unsigned char)*p)) {
        p++;
    }

    if (*p == '\0') {
        return IO_ERRO_TEXTO_VAZIO;
    }

    errno = 0;
    val = strtol(p, &endptr, 10);

    if (endptr == p) {
        return IO_ERRO_NUMERO_INVALIDO;
    }

    if (errno == ERANGE || val > LONG_MAX || val < LONG_MIN) {
        return IO_ERRO_OVERFLOW;
    }

    /* Verifica caracteres residuais no fim da string */
    while (*endptr != '\0') {
        if (!isspace((unsigned char)*endptr)) {
            return IO_ERRO_CARACTERES_EXTRAS;
        }
        endptr++;
    }

    if (valor_saida != NULL) {
        *valor_saida = val;
    }

    return IO_SUCESSO;
}

StatusIO io_validar_id(const char *str, int *id_saida) {
    long val = 0;
    StatusIO st = io_validar_inteiro(str, &val);

    if (st != IO_SUCESSO) {
        return st;
    }

    if (val <= 0 || val > INT_MAX) {
        return IO_ERRO_ID_INVALIDO;
    }

    if (id_saida != NULL) {
        *id_saida = (int)val;
    }

    return IO_SUCESSO;
}

StatusIO io_validar_id_valor(int id) {
    if (id <= 0) {
        return IO_ERRO_ID_INVALIDO;
    }
    return IO_SUCESSO;
}

StatusIO io_validar_ano(const char *str, int *ano_saida) {
    long val = 0;
    StatusIO st = io_validar_inteiro(str, &val);

    if (st != IO_SUCESSO) {
        return st;
    }

    if (val < ANO_MINIMO || val > ANO_MAXIMO) {
        return IO_ERRO_ANO_INVALIDO;
    }

    if (ano_saida != NULL) {
        *ano_saida = (int)val;
    }

    return IO_SUCESSO;
}

StatusIO io_validar_ano_valor(int ano) {
    if (ano < ANO_MINIMO || ano > ANO_MAXIMO) {
        return IO_ERRO_ANO_INVALIDO;
    }
    return IO_SUCESSO;
}

void io_limpar_stream(FILE *stream) {
    int c;
    if (stream == NULL) {
        return;
    }
    while ((c = fgetc(stream)) != '\n' && c != EOF) {
        /* descarta caracteres */
    }
}

StatusIO io_ler_linha(char *destino, size_t tam_destino, FILE *stream) {
    char *nl;
    int c;

    if (destino == NULL || tam_destino == 0 || stream == NULL) {
        return IO_ERRO_ENTRADA_NULA;
    }

    if (fgets(destino, (int)tam_destino, stream) == NULL) {
        return IO_ERRO_EOF;
    }

    nl = strchr(destino, '\n');
    if (nl != NULL) {
        *nl = '\0';
        if (nl > destino && *(nl - 1) == '\r') {
            *(nl - 1) = '\0';
        }
        return IO_SUCESSO;
    }

    /* Linha nao continha '\n': checar se alcancou EOF ou se extrapolou o buffer */
    c = fgetc(stream);
    if (c == EOF) {
        return IO_SUCESSO;
    }

    /* A linha extrapolou a capacidade: descartar o restante para nao contaminar entradas futuras */
    ungetc(c, stream);
    io_limpar_stream(stream);
    return IO_ERRO_BUFFER_EXCEDIDO;
}

StatusIO io_ler_texto(char *destino, size_t tam_destino, FILE *stream) {
    StatusIO st;

    if (destino == NULL || tam_destino == 0 || stream == NULL) {
        return IO_ERRO_ENTRADA_NULA;
    }

    st = io_ler_linha(destino, tam_destino, stream);
    if (st != IO_SUCESSO) {
        return st;
    }

    return io_validar_texto(destino, (int)tam_destino - 1);
}

StatusIO io_ler_id(int *id_saida, FILE *stream) {
    char buffer[64];
    StatusIO st;

    st = io_ler_linha(buffer, sizeof(buffer), stream);
    if (st != IO_SUCESSO) {
        return st;
    }

    return io_validar_id(buffer, id_saida);
}

StatusIO io_ler_ano(int *ano_saida, FILE *stream) {
    char buffer[64];
    StatusIO st;

    st = io_ler_linha(buffer, sizeof(buffer), stream);
    if (st != IO_SUCESSO) {
        return st;
    }

    return io_validar_ano(buffer, ano_saida);
}

