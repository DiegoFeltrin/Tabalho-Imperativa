/* arquivo.c: leitura do CSV e leitura/gravacao do arquivo binario. */
/* O P1 usa todas as funcoes daqui; o P2 usa a leitura, a gravacao e o tamanho do arquivo binario. */

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
/* Recebe o caminho do CSV; devolve o vetor e poe em *total quantos alimentos leu (-1 se nao abriu). */
Alimento_t *arquivo_ler_csv(const char *caminho, int *total)
{
    Alimento_t *vetor = NULL;
    int capacidade = CAPACIDADE_INICIAL;
    int total_lidos = -1;
    char buffer[TAM_LINHA_BUFFER];
    FILE *arquivo = fopen(caminho, "r");

    if (arquivo) {
        /* Comeca com espaco para CAPACIDADE_INICIAL alimentos; calloc ja zera tudo. */
        vetor = calloc((size_t)capacidade, sizeof(Alimento_t));
        if (!vetor) {
            encerrar_sem_memoria("ler o arquivo CSV");
        }
        total_lidos = 0;
        /* A primeira linha e o cabecalho (nomes das colunas) e e descartada. */
        fgets(buffer, sizeof(buffer), arquivo);
        /* fgets le uma linha inteira sem passar do tamanho do buffer. */
        while (fgets(buffer, sizeof(buffer), arquivo)) {
            /* Vetor cheio: dobra a capacidade antes de ler o proximo alimento. */
            if (total_lidos == capacidade) {
                vetor = vetor_aumentar(vetor, &capacidade);
            }
            /* So conta a linha se ela tiver os 5 campos; linha ruim e ignorada. */
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
/* Devolve 1 se a linha tinha os 5 campos e 0 se nao tinha. */
static int linha_para_alimento(const char *linha, Alimento_t *alimento)
{
    char categoria[TAM_MAX_DESCRICAO];
    char formato[TAM_FORMATO];
    int convertido = 0;

    /* Monta o formato "%d;%127[^;];%127[^;];%lf;%lf" a partir de TAM_MAX_DESCRICAO. */
    /* O 127 limita o que o sscanf copia, assim o texto nunca passa do tamanho do vetor. */
    snprintf(formato, sizeof(formato), "%%d;%%%d[^;];%%%d[^;];%%lf;%%lf",
             TAM_MAX_DESCRICAO - 1, TAM_MAX_DESCRICAO - 1);
    /* Zera o registro antes, para os bytes que sobram na descricao ficarem sempre 0. */
    memset(alimento, 0, sizeof(Alimento_t));
    /* NA, Tr, '*' e branco da TACO gravados como 0 */
    /* (a TACO nao traz o valor medido nesses casos; 0 marca o dado ausente) */
    if (sscanf(linha, formato, &alimento->numero,
               alimento->descricao, categoria, &alimento->energia_kcal,
               &alimento->proteina_g) == CAMPOS_POR_LINHA) {
        /* A categoria chega como texto e e guardada como enum. */
        alimento->categoria = categoria_converter(categoria);
        convertido = 1;
    }

    return convertido;
}

/* Dobra a capacidade do vetor com realloc. */
/* Recebe o vetor e a capacidade atual; devolve o vetor maior e atualiza *capacidade. */
static Alimento_t *vetor_aumentar(Alimento_t *vetor, int *capacidade)
{
    /* Dobrar (64, 128, 256...) faz poucos realloc mesmo com muitos alimentos. */
    /* O resultado vai para outro ponteiro: se o realloc falhar, o vetor antigo ainda existe. */
    Alimento_t *novo = realloc(vetor, (size_t)(*capacidade * 2) * sizeof(Alimento_t));

    /* Se falhou, libera o vetor antigo, mostra o erro e encerra. */
    if (!novo) {
        free(vetor);
        encerrar_sem_memoria("aumentar o vetor");
    }
    *capacidade = *capacidade * 2;

    return novo;
}

/* Grava o vetor inteiro com fwrite; fclose com falha tambem conta como erro. */
/* Cada registro e o struct Alimento_t inteiro, com tamanho fixo de sizeof(Alimento_t). */
int arquivo_gravar_binario(const char *caminho, const Alimento_t *vetor,
                           int quantidade)
{
    int total_gravados = -1;
    FILE *arquivo = fopen(caminho, "wb");

    if (arquivo) {
        /* Um fwrite so grava todos os registros; ele devolve quantos conseguiu gravar. */
        total_gravados = (int)fwrite(vetor, sizeof(Alimento_t), (size_t)quantidade, arquivo);
        /* O fclose termina de escrever no disco; se ele falhar o arquivo pode estar incompleto. */
        if (fclose(arquivo) == EOF) {
            total_gravados = -1;
        }
    }

    return total_gravados;
}

/* Le os registros com fread, no maximo capacidade. */
/* Como os registros tem tamanho fixo, o fread le o struct inteiro direto no vetor. */
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
/* Devolve o tamanho em bytes, ou -1 se nao conseguiu abrir. */
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
