/* P1.c: le alimentos.csv, grava dados.bin e confere o arquivo gravado. */

#include "alimento.h"
#include "arquivo.h"
#include "amostra.h"
#include <stdio.h>
#include <stdlib.h>

/* Quantos alimentos a tabela TACO tem; outro total so gera aviso. */
#define ESPERADO_REGISTROS 597

#define ARQUIVO_CSV "alimentos.csv"
#define ARQUIVO_BIN "dados.bin"

/* Converte o CSV em binario e confere o arquivo gravado (Etapa B). */
int main(void)
{
    Alimento_t *vetor_lido = NULL;
    Alimento_t *vetor_conferencia = NULL;
    int codigo_retorno = EXIT_SUCCESS;
    int total_lidos = 0;
    int total_gravados = 0;
    int total_conferidos = 0;
    long tamanho_bytes = 0;
    long tamanho_esperado = 0;

    printf("=====================================================\n");
    printf("   PUCPR - Programacao Imperativa - Trabalho RA2     \n");
    printf("   Programa P1: Conversao CSV -> Binario (Etapa B)   \n");
    printf("=====================================================\n\n");

    printf("[1/3] Lendo arquivo CSV: '%s'...\n", ARQUIVO_CSV);
    vetor_lido = arquivo_ler_csv(ARQUIVO_CSV, &total_lidos);
    if (total_lidos <= 0) {
        fprintf(stderr, "ERRO: Leitura do CSV falhou! Registros lidos: %d\n",
                total_lidos);
        codigo_retorno = EXIT_FAILURE;
    } else {
        printf("SUCESSO: %d registros lidos com sucesso do CSV!\n",
               total_lidos);
        if (total_lidos != ESPERADO_REGISTROS) {
            printf("AVISO: a tabela TACO tem %d alimentos e o CSV tem %d.\n",
                   ESPERADO_REGISTROS, total_lidos);
        }
    }

    if (codigo_retorno == EXIT_SUCCESS) {
        printf("\n[2/3] Gravando em formato binario: '%s'...\n", ARQUIVO_BIN);
        total_gravados = arquivo_gravar_binario(ARQUIVO_BIN, vetor_lido,
                                                total_lidos);
        if (total_gravados != total_lidos) {
            fprintf(stderr, "ERRO: Falha ao gravar no arquivo binario! "
                    "Registros gravados: %d\n", total_gravados);
            codigo_retorno = EXIT_FAILURE;
        } else {
            printf("SUCESSO: %d registros gravados em '%s'!\n",
                   total_gravados, ARQUIVO_BIN);
        }
    }

    if (codigo_retorno == EXIT_SUCCESS) {
        printf("\n[3/3] Conferindo integridade de '%s'...\n", ARQUIVO_BIN);
        tamanho_bytes = arquivo_obter_tamanho(ARQUIVO_BIN);
        tamanho_esperado = (long)((size_t)total_lidos * sizeof(Alimento_t));
        printf("  - sizeof(Alimento): %lu bytes\n", (unsigned long)sizeof(Alimento_t));
        printf("  - Tamanho do arquivo no disco: %ld bytes\n", tamanho_bytes);
        printf("  - Calculo (%d x %lu):        %ld bytes\n",
               total_lidos, (unsigned long)sizeof(Alimento_t), tamanho_esperado);
        if (tamanho_bytes != tamanho_esperado) {
            fprintf(stderr, "ERRO: Tamanho do arquivo binario diverge "
                    "do esperado!\n");
            codigo_retorno = EXIT_FAILURE;
        }
    }

    if (codigo_retorno == EXIT_SUCCESS) {
        vetor_conferencia = calloc((size_t)total_lidos, sizeof(Alimento_t));
        if (!vetor_conferencia) {
            free(vetor_lido);
            fprintf(stderr, "ERRO FATAL: Memoria insuficiente para conferir "
                    "'%s'.\n", ARQUIVO_BIN);
            exit(EXIT_FAILURE);
        }
        total_conferidos = arquivo_ler_binario(ARQUIVO_BIN, vetor_conferencia,
                                               total_lidos);
        if (total_conferidos != total_lidos) {
            fprintf(stderr, "ERRO: Leitura de verificacao do binario "
                    "falhou! Lidos: %d\n", total_conferidos);
            codigo_retorno = EXIT_FAILURE;
        } else {
            printf("SUCESSO: Conferencia confirmada! "
                   "%d registros validos.\n", total_conferidos);
            exibir_amostras(vetor_conferencia, total_conferidos);
            printf("Status: Etapa B concluida com sucesso absoluto!\n");
        }
    }

    free(vetor_conferencia);
    free(vetor_lido);

    return codigo_retorno;
}
