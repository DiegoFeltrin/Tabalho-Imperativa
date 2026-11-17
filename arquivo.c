/* arquivo.c: leitura do CSV e leitura/gravacao do arquivo binario. */

#include "arquivo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Tamanho maximo de uma linha do CSV. */
#define TAM_LINHA_BUFFER 512

/* Quantos alimentos cabem no vetor antes do primeiro realloc. */
#define CAPACIDADE_INICIAL 64

/* Quantos campos cada linha do CSV tem. */
#define CAMPOS_POR_LINHA 5

/* Tamanho do texto de formato que o sscanf usa para ler uma linha. */
#define TAM_FORMATO 64

static int linha_para_alimento(const char *linha, Alimento_t *alimento);
static Alimento_t *vetor_aumentar(Alimento_t *vetor, int *capacidade);

/* Le o CSV linha a linha num vetor que dobra de tamanho quando enche. */
Alimento_t *arquivo_ler_csv(const char *caminho, int *total)
{
    Alimento_t *vetor = NULL;
    int capacidade = CAPACIDADE_INICIAL;
    int total_lidos = -1;
    char buffer[TAM_LINHA_BUFFER];
    FILE *arquivo = fopen(caminho, "r");

    if (arquivo) {
        vetor = calloc((size_t)capacidade, sizeof(Alimento_t));
        if (!vetor) {
            fprintf(stderr, "ERRO FATAL: Memoria insuficiente para ler '%s'.\n", caminho);
            exit(EXIT_FAILURE);
        }
        total_lidos = 0;
        fgets(buffer, sizeof(buffer), arquivo);
        while (fgets(buffer, sizeof(buffer), arquivo)) {
            if (total_lidos == capacidade) {
                vetor = vetor_aumentar(vetor, &capacidade);
            }
            if (linha_para_alimento(buffer, &vetor[total_lidos])) {
                total_lidos++;
            }
        }
        fclose(arquivo);
    }

    *total = total_lidos;
    return vetor;
}

/* Le numero;descricao;categoria;energia;proteina direto para os campos do alimento. */
static int linha_para_alimento(const char *linha, Alimento_t *alimento)
{
    char categoria[TAM_MAX_DESCRICAO];
    char formato[TAM_FORMATO];
    int convertido = 0;

    snprintf(formato, sizeof(formato), "%%d;%%%d[^;];%%%d[^;];%%lf;%%lf",
             TAM_MAX_DESCRICAO - 1, TAM_MAX_DESCRICAO - 1);
    memset(alimento, 0, sizeof(Alimento_t));
    /* NA, Tr, '*' e branco da TACO gravados como 0 */
    if (sscanf(linha, formato, &alimento->numero,
               alimento->descricao, categoria, &alimento->energia_kcal,
               &alimento->proteina_g) == CAMPOS_POR_LINHA) {
        alimento->categoria = categoria_converter(categoria);
        convertido = 1;
    }

    return convertido;
}

/* Dobra a capacidade do vetor com realloc. */
static Alimento_t *vetor_aumentar(Alimento_t *vetor, int *capacidade)
{
    Alimento_t *novo = realloc(vetor, (size_t)(*capacidade * 2) * sizeof(Alimento_t));

    if (!novo) {
        free(vetor);
        fprintf(stderr, "ERRO FATAL: Memoria insuficiente para aumentar o vetor.\n");
        exit(EXIT_FAILURE);
    }
    *capacidade = *capacidade * 2;

    return novo;
}

/* Grava o vetor inteiro com fwrite; fclose com falha tambem conta como erro. */
int arquivo_gravar_binario(const char *caminho, const Alimento_t *vetor,
                           int quantidade)
{
    int total_gravados = -1;
    FILE *arquivo = fopen(caminho, "wb");

    if (arquivo) {
        total_gravados = (int)fwrite(vetor, sizeof(Alimento_t), (size_t)quantidade, arquivo);
        if (fclose(arquivo) == EOF) {
            total_gravados = -1;
        }
    }

    return total_gravados;
}

/* Le os registros com fread, no maximo capacidade. */
int arquivo_ler_binario(const char *caminho, Alimento_t *vetor,
                        int capacidade)
{
    int total_lidos = -1;
    FILE *arquivo = fopen(caminho, "rb");

    if (arquivo) {
        total_lidos = (int)fread(vetor, sizeof(Alimento_t), (size_t)capacidade, arquivo);
        fclose(arquivo);
    }

    return total_lidos;
}

/* Vai ao fim do arquivo e usa ftell para saber o tamanho. */
long arquivo_obter_tamanho(const char *caminho)
{
    long tamanho = -1;
    FILE *arquivo = fopen(caminho, "rb");

    if (arquivo) {
        if (fseek(arquivo, 0, SEEK_END) == 0) {
            tamanho = ftell(arquivo);
        }
        fclose(arquivo);
    }

    return tamanho;
}
