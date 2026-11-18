/* amostra.c: amostra dos registros lidos, usada so pelo P1. */

#include "amostra.h"
#include <stdio.h>

/* Indice do registro do meio mostrado na amostra. */
#define INDICE_AMOSTRA_MEIO 299

/* Mostra tres alimentos do vetor para conferir a leitura. */
/* Recebe o vetor lido de volta do dados.bin e quantos registros ele tem. */
void exibir_amostras(const Alimento_t *vetor, int quantidade)
{
    printf("\n--- AMOSTRA DOS REGISTROS ---\n");
    printf("Primeiro registro:\n");
    alimento_exibir(&vetor[0]);

    /* So mostra o do meio se o vetor tiver registros suficientes. */
    if (quantidade > INDICE_AMOSTRA_MEIO) {
        printf("\nRegistro intermediario (indice %d):\n", INDICE_AMOSTRA_MEIO);
        alimento_exibir(&vetor[INDICE_AMOSTRA_MEIO]);
    }

    printf("\nUltimo registro:\n");
    alimento_exibir(&vetor[quantidade - 1]);
    printf("-----------------------------\n\n");
}
