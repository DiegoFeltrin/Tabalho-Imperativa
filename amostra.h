/* amostra.h: amostra dos registros lidos, usada so pelo P1. */

#ifndef AMOSTRA_H
#define AMOSTRA_H

#include "alimento.h"

/* Mostra o primeiro, o do meio (indice 299) e o ultimo alimento do vetor. */
/* Serve para conferir a olho que a leitura de volta do dados.bin deu certo. */
void exibir_amostras(const Alimento_t *vetor, int quantidade);

#endif /* AMOSTRA_H */
