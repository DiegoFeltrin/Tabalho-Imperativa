/* arvore.h: arvore binaria de busca que indexa os alimentos por energia ou proteina. */
/* Cada categoria tem duas arvores: uma com chave energia e outra com chave proteina. */

#ifndef ARVORE_H
#define ARVORE_H

/* O no da arvore aponta para o no do alimento na lista (definido em lista.h). */
/* So a declaracao basta aqui, porque a arvore guarda apenas o ponteiro. */
struct NoAlimento;

/* Um no da arvore: a chave (energia ou proteina) e o alimento dono dela. */
/* O alimento nao e copiado: o no guarda so a chave e um ponteiro para o no da lista. */
typedef struct NoArvore {
    double chave;                  /* energia ou proteina do alimento */
    struct NoAlimento *alimento;   /* no da lista onde estao os dados do alimento */
    struct NoArvore *esquerda;     /* subarvore com chaves menores */
    struct NoArvore *direita;      /* subarvore com chaves maiores ou iguais */
} NoArvore_t;

/* Insere a chave na arvore e devolve a raiz. */
/* Na arvore vazia (raiz NULL) o novo no vira a raiz. */
NoArvore_t *arvore_inserir(NoArvore_t *raiz, double chave,
                           struct NoAlimento *alimento);

/* Mostra os alimentos da maior chave para a menor e conta quantos mostrou. */
void arvore_listar_decrescente(const NoArvore_t *raiz, int *contador);

/* Mostra, da maior chave para a menor, so os alimentos com chave entre min e max. */
/* A faixa e fechada: min e max tambem entram. */
void arvore_listar_faixa(const NoArvore_t *raiz, double min, double max,
                         int *contador);

/* Libera todos os nos da arvore. */
/* Os alimentos nao sao liberados aqui: eles pertencem a lista. */
void arvore_liberar(NoArvore_t *raiz);

#endif /* ARVORE_H */
