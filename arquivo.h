/* arquivo.h: leitura do CSV e leitura/gravacao do arquivo binario. */

#ifndef ARQUIVO_H
#define ARQUIVO_H

#include "alimento.h"

/* Nome do arquivo binario que o P1 grava e o P2 le. */
#define ARQUIVO_DADOS_BIN "dados.bin"

/* Le o CSV num vetor dinamico e coloca em *total quantos leu (-1 se nao abriu). */
/* Quem chama recebe o vetor e precisa liberar com free. */
Alimento_t *arquivo_ler_csv(const char *caminho, int *total);

/* Grava o vetor no arquivo binario e devolve quantos gravou (-1 se deu erro). */
int arquivo_gravar_binario(const char *caminho, const Alimento_t *vetor, int quantidade);

/* Le ate capacidade registros do binario e devolve quantos leu (-1 se deu erro). */
/* O vetor tem que ter espaco para pelo menos capacidade registros. */
int arquivo_ler_binario(const char *caminho, Alimento_t *vetor, int capacidade);

/* Devolve o tamanho do arquivo em bytes (-1 se deu erro). */
/* O P1 usa para conferir se o arquivo tem total x sizeof(Alimento_t) bytes. */
long arquivo_obter_tamanho(const char *caminho);

#endif /* ARQUIVO_H */
