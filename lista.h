/* lista.h: lista ligada de categorias, cada uma com seus alimentos e duas arvores. */
/* Estrutura em dois niveis: uma lista de categorias, e cada categoria tem a sua lista de alimentos. */

#ifndef LISTA_H
#define LISTA_H

#include "alimento.h"
#include "arvore.h"

/* No da lista de alimentos de uma categoria. */
typedef struct NoAlimento {
    Alimento_t dado;               /* copia do alimento lido do dados.bin */
    struct NoAlimento *proximo;    /* proximo alimento em ordem alfabetica (NULL no ultimo) */
} NoAlimento_t;

/* No da lista de categorias: alimentos em ordem alfabetica e as arvores de indice. */
typedef struct NoCategoria {
    Categoria_e categoria;         /* qual categoria e este no (o nome sai de categoria_para_texto) */
    NoAlimento_t *alimentos;       /* primeiro alimento da categoria, ja em ordem alfabetica */
    NoArvore_t *arvore_energia;    /* indice dos alimentos por energia */
    NoArvore_t *arvore_proteina;   /* indice dos alimentos por proteina */
    int total_alimentos;           /* quantos alimentos a categoria tem agora */
    struct NoCategoria *proximo;   /* proxima categoria em ordem alfabetica (NULL na ultima) */
} NoCategoria_t;

/* Le dados.bin, monta as listas e as arvores e devolve a primeira categoria. */
/* Coloca em *total_lidos quantos alimentos foram carregados. */
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
/* Recebe ponteiro para a cabeca porque a primeira categoria pode ser a removida. */
int lista_remover_categoria(NoCategoria_t **cabeca_categorias,
                            Categoria_e categoria);

/* Grava os alimentos que restaram e devolve quantos gravou (-1 se deu erro). */
int lista_salvar_binario(const NoCategoria_t *cabeca,
                         const char *caminho_bin);

/* Libera todas as categorias, alimentos e arvores e zera a cabeca. */
/* Depois dela a lista fica vazia (cabeca = NULL). */
void lista_liberar_tudo(NoCategoria_t **cabeca);

#endif /* LISTA_H */
