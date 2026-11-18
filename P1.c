/* P1.c: le alimentos.csv, grava dados.bin e confere o arquivo gravado. */
/* Etapa B do trabalho. Cada passo so roda se o anterior deu certo. */

#include "alimento.h"
#include "arquivo.h"
#include "amostra.h"
#include <stdio.h>
#include <stdlib.h>

/* Quantos alimentos a tabela TACO tem; outro total so gera aviso. */
#define ESPERADO_REGISTROS 597

#define ARQUIVO_CSV "alimentos.csv"

/* Converte o CSV em binario e confere o arquivo gravado (Etapa B). */
/* Devolve EXIT_SUCCESS se os 3 passos deram certo, senao EXIT_FAILURE. */
int main(void)
{
    Alimento_t *alimentos_lidos = NULL;
    Alimento_t *alimentos_conferidos = NULL;
    int codigo_retorno = EXIT_SUCCESS;
    int total_lidos = 0;
    int total_gravados = 0;
    int total_conferidos = 0;
    long tamanho_bytes = 0;
    long tamanho_esperado = 0;

    exibir_titulo("   Programa P1: Conversao CSV -> Binario (Etapa B)   ");

    /* Passo 1: le o CSV para um vetor dinamico (calloc no inicio, realloc quando enche). */
    printf("[1/3] Lendo arquivo CSV: '%s'...\n", ARQUIVO_CSV);
    alimentos_lidos = arquivo_ler_csv(ARQUIVO_CSV, &total_lidos);
    if (total_lidos <= 0) {
        fprintf(stderr, "ERRO: Leitura do CSV falhou! Registros lidos: %d\n",
                total_lidos);
        codigo_retorno = EXIT_FAILURE;
    } else {
        printf("SUCESSO: %d registros lidos com sucesso do CSV!\n",
               total_lidos);
        /* Um total diferente de 597 nao impede o resto: so avisa. */
        if (total_lidos != ESPERADO_REGISTROS) {
            printf("AVISO: a tabela TACO tem %d alimentos e o CSV tem %d.\n",
                   ESPERADO_REGISTROS, total_lidos);
        }
    }

    /* Passo 2: grava o vetor inteiro no dados.bin. */
    if (codigo_retorno == EXIT_SUCCESS) {
        printf("\n[2/3] Gravando em formato binario: '%s'...\n", ARQUIVO_DADOS_BIN);
        total_gravados = arquivo_gravar_binario(ARQUIVO_DADOS_BIN, alimentos_lidos,
                                                total_lidos);
        if (total_gravados != total_lidos) {
            fprintf(stderr, "ERRO: Falha ao gravar no arquivo binario! "
                    "Registros gravados: %d\n", total_gravados);
            codigo_retorno = EXIT_FAILURE;
        } else {
            printf("SUCESSO: %d registros gravados em '%s'!\n",
                   total_gravados, ARQUIVO_DADOS_BIN);
        }
    }

    /* Passo 3a: confere o tamanho. Com registros de tamanho fixo, o arquivo tem que ter */
    /* exatamente total x sizeof(Alimento_t) bytes; outro tamanho quer dizer gravacao incompleta. */
    if (codigo_retorno == EXIT_SUCCESS) {
        printf("\n[3/3] Conferindo integridade de '%s'...\n", ARQUIVO_DADOS_BIN);
        tamanho_bytes = arquivo_obter_tamanho(ARQUIVO_DADOS_BIN);
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

    /* Passo 3b: le o binario de volta num segundo vetor e mostra uma amostra. */
    if (codigo_retorno == EXIT_SUCCESS) {
        alimentos_conferidos = calloc((size_t)total_lidos, sizeof(Alimento_t));
        if (!alimentos_conferidos) {
            free(alimentos_lidos);
            encerrar_sem_memoria("conferir o arquivo binario");
        }
        total_conferidos = arquivo_ler_binario(ARQUIVO_DADOS_BIN, alimentos_conferidos,
                                               total_lidos);
        if (total_conferidos != total_lidos) {
            fprintf(stderr, "ERRO: Leitura de verificacao do binario "
                    "falhou! Lidos: %d\n", total_conferidos);
            codigo_retorno = EXIT_FAILURE;
        } else {
            printf("SUCESSO: Conferencia confirmada! "
                   "%d registros validos.\n", total_conferidos);
            exibir_amostras(alimentos_conferidos, total_conferidos);
            printf("Status: Etapa B concluida com sucesso absoluto!\n");
        }
    }

    /* Libera os dois vetores em qualquer caso (free(NULL) nao faz nada). */
    free(alimentos_conferidos);
    free(alimentos_lidos);

    return codigo_retorno;
}
