/**
 * @file io.h
 * @brief Módulo de validação segura de entradas de dados e I/O (Requisito R01 e Seção 9).
 *
 * Fornece validação estrita para texto (fgets sem buffer overflow, rejeição de strings vazias
 * ou apenas com espaços, até 79 caracteres úteis), inteiros com strtol (rejeição de caracteres extras,
 * letras e overflow), ID (> 0) e ano de publicação (1 a 2026).
 */

#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "bst.h"

/**
 * @brief Códigos de status e erro para validação e leitura de entradas.
 */
typedef enum {
    IO_SUCESSO = 0,                 /**< Entrada válida e processada com sucesso */
    IO_ERRO_ENTRADA_NULA,           /**< Ponteiro de entrada ou destino nulo (NULL) */
    IO_ERRO_BUFFER_EXCEDIDO,        /**< A entrada excedeu a capacidade do buffer */
    IO_ERRO_TEXTO_VAZIO,            /**< Texto vazio (comprimento 0) */
    IO_ERRO_APENAS_ESPACOS,         /**< Texto composto exclusivamente por espaços em branco */
    IO_ERRO_TAMANHO_EXCEDIDO,       /**< Quantidade de caracteres úteis superior ao limite permitido */
    IO_ERRO_NUMERO_INVALIDO,        /**< Nenhum dígito numérico válido encontrado */
    IO_ERRO_CARACTERES_EXTRAS,      /**< Caracteres adicionais não numéricos no final do número */
    IO_ERRO_OVERFLOW,               /**< Valor numérico ultrapassou limites do tipo (overflow/underflow) */
    IO_ERRO_ID_INVALIDO,            /**< ID inválido (deve ser inteiro positivo > 0) */
    IO_ERRO_ANO_INVALIDO,           /**< Ano inválido (deve estar entre 1 e 2026) */
    IO_ERRO_EOF                     /**< Fim de arquivo inesperado ou falha na leitura do stream */
} StatusIO;

/**
 * @brief Retorna uma mensagem amigável em português para o código de erro de I/O.
 * @param status Código StatusIO.
 * @return String constante descritiva.
 */
const char* io_mensagem_erro(StatusIO status);

/* ==========================================================================
 * Funções puras de validação (testáveis sem necessidade de stream)
 * ========================================================================== */

/**
 * @brief Valida um texto segundo as regras do edital:
 *        - Não nulo
 *        - Não vazio
 *        - Não composto apenas de espaços em branco (isspace)
 *        - Comprimento útil <= max_caracteres_uteis (ex: 79)
 * @param str String terminada em '\0' a validar.
 * @param max_caracteres_uteis Limite máximo de caracteres úteis (79 para título e autor).
 * @return IO_SUCESSO se válido, ou código de erro correspondente.
 */
StatusIO io_validar_texto(const char *str, int max_caracteres_uteis);

/**
 * @brief Converte e valida uma string numérica em um número inteiro longo usando strtol.
 *        Rejeita letras, lixo no final da string, strings vazias e overflow.
 * @param str String contendo o número.
 * @param valor_saida Ponteiro para armazenar o valor convertido.
 * @return IO_SUCESSO se a conversão for estritamente válida, ou código de erro.
 */
StatusIO io_validar_inteiro(const char *str, long *valor_saida);

/**
 * @brief Valida uma string contendo um ID de livro (deve ser inteiro positivo > 0 e caber em int).
 * @param str String contendo o ID.
 * @param id_saida Ponteiro para armazenar o ID validado (pode ser NULL).
 * @return IO_SUCESSO se válido, ou código de erro.
 */
StatusIO io_validar_id(const char *str, int *id_saida);

/**
 * @brief Valida o valor numérico direto de um ID de livro.
 * @param id Valor do ID a verificar.
 * @return IO_SUCESSO se id > 0, IO_ERRO_ID_INVALIDO caso contrário.
 */
StatusIO io_validar_id_valor(int id);

/**
 * @brief Valida uma string contendo o ano de publicação (intervalo [1, 2026]).
 * @param str String contendo o ano.
 * @param ano_saida Ponteiro para armazenar o ano validado (pode ser NULL).
 * @return IO_SUCESSO se válido, ou código de erro.
 */
StatusIO io_validar_ano(const char *str, int *ano_saida);

/**
 * @brief Valida o valor numérico direto de um ano de publicação.
 * @param ano Valor do ano a verificar.
 * @return IO_SUCESSO se 1 <= ano <= 2026, IO_ERRO_ANO_INVALIDO caso contrário.
 */
StatusIO io_validar_ano_valor(int ano);

/* ==========================================================================
 * Funções seguras de leitura a partir de stream (stdin ou arquivos)
 * ========================================================================== */

/**
 * @brief Descarta todo o restante da linha atual do stream até '\n' ou EOF.
 * @param stream Fluxo de entrada (normalmente stdin).
 */
void io_limpar_stream(FILE *stream);

/**
 * @brief Lê uma linha completa de forma segura usando fgets.
 *        Se a linha exceder o buffer (não contiver '\n'), rejeita a entrada
 *        e descarta automaticamente o restante do stream antes de retornar.
 * @param destino Buffer onde o texto lido será gravado.
 * @param tam_destino Tamanho total do buffer de destino.
 * @param stream Fluxo de entrada.
 * @return IO_SUCESSO se lido com sucesso, IO_ERRO_BUFFER_EXCEDIDO se ultrapassou o buffer.
 */
StatusIO io_ler_linha(char *destino, size_t tam_destino, FILE *stream);

/**
 * @brief Lê e valida um texto com espaços para o modelo Livro (título/autor).
 * @param destino Buffer de destino (mínimo TAM_TEXTO bytes).
 * @param tam_destino Tamanho total do buffer.
 * @param stream Fluxo de entrada.
 * @return IO_SUCESSO se válido, ou código de erro detalhado.
 */
StatusIO io_ler_texto(char *destino, size_t tam_destino, FILE *stream);

/**
 * @brief Lê e valida interativamente um ID de livro a partir do stream.
 * @param id_saida Ponteiro onde será guardado o ID lido.
 * @param stream Fluxo de entrada.
 * @return IO_SUCESSO se lido e válido, ou código de erro.
 */
StatusIO io_ler_id(int *id_saida, FILE *stream);

/**
 * @brief Lê e valida interativamente um ano de publicação (1 a 2026) a partir do stream.
 * @param ano_saida Ponteiro onde será guardado o ano lido.
 * @param stream Fluxo de entrada.
 * @return IO_SUCESSO se lido e válido, ou código de erro.
 */
StatusIO io_ler_ano(int *ano_saida, FILE *stream);

#endif /* IO_H */

