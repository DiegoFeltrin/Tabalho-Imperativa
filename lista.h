/* lista.h: lista ligada de categorias, cada uma com seus alimentos e duas arvores. */

#ifndef LISTA_H
#define LISTA_H

#include "alimento.h"
#include "arvore.h"

/* No da lista de alimentos de uma categoria. */
typedef struct NoAlimento {
    Alimento_t dado;
    struct NoAlimento *proximo;
} NoAlimento_t;

/* No da lista de categorias: alimentos em ordem alfabetica e as arvores de indice. */
typedef struct NoCategoria {
    Categoria_e categoria;
    NoAlimento_t *alimentos;
    NoArvore_t *arvore_energia;
    NoArvore_t *arvore_proteina;
    int total_alimentos;
    struct NoCategoria *proximo;
} NoCategoria_t;

/* Le dados.bin, monta as listas e as arvores e devolve a primeira categoria. */
NoCategoria_t *lista_carregar_de_binario(const char *caminho_bin,
                                         int *total_lidos);

/* Mostra as categorias numeradas, na ordem da lista. */
void lista_exibir_categorias(const NoCategoria_t *cabeca);

/* Devolve a categoria da posicao indice (comeca em 1), ou NULL. */
NoCategoria_t *lista_obter_categoria_por_indice(NoCategoria_t *cabeca,
                                                int indice);

/* Mostra os alimentos da categoria em ordem alfabetica. */
void lista_exibir_alimentos_categoria(const NoCategoria_t *no_categoria);

/* Remove o alimento com esse numero e refaz as arvores; devolve 1 se removeu. */
int lista_remover_alimento(NoCategoria_t *no_categoria, int numero_alimento);

/* Remove a categoria e libera tudo o que ela tem; devolve 1 se removeu. */
int lista_remover_categoria(NoCategoria_t **cabeca_categorias,
                            Categoria_e categoria);

/* Grava os alimentos que restaram e devolve quantos gravou (-1 se deu erro). */
int lista_salvar_binario(const NoCategoria_t *cabeca,
                         const char *caminho_bin);

/* Libera todas as categorias, alimentos e arvores e zera a cabeca. */
void lista_liberar_tudo(NoCategoria_t **cabeca);

#endif /* LISTA_H */
