/* arvore.c: arvore binaria de busca usada como indice de energia e proteina. */
/* Todas as funcoes sao recursivas: cada chamada cuida de um no e chama a si mesma nos filhos. */

#include "arvore.h"
#include "lista.h"
#include <stdlib.h>

/* Insere a chave na arvore de forma recursiva e devolve a raiz da subarvore. */
/* Recebe a raiz (pode ser NULL), a chave e o no do alimento dono da chave. */
NoArvore_t *arvore_inserir(NoArvore_t *raiz, double chave,
                           struct NoAlimento *alimento)
{
    NoArvore_t *resultado = raiz;

    if (!raiz) {
        /* Chegou num lugar vazio: e aqui que o novo no fica. */
        /* malloc basta porque todos os campos sao preenchidos logo abaixo. */
        NoArvore_t *novo = malloc(sizeof(NoArvore_t));
        if (!novo) {
            encerrar_sem_memoria("o no da arvore");
        }
        novo->chave = chave;
        novo->alimento = alimento;
        novo->esquerda = NULL;
        novo->direita = NULL;
        resultado = novo;
    } else if (chave < raiz->chave) {
        /* Chave menor desce pela esquerda. */
        raiz->esquerda = arvore_inserir(raiz->esquerda, chave, alimento);
    } else {
        /* chave igual vai para a direita (>=) */
        raiz->direita = arvore_inserir(raiz->direita, chave, alimento);
    }

    /* Devolve a raiz para quem chamou religar o filho (so muda quando a subarvore era vazia). */
    return resultado;
}

/* Percorre direita, raiz e esquerda para listar da maior chave para a menor. */
/* Soma em *contador quantos alimentos mostrou. */
void arvore_listar_decrescente(const NoArvore_t *raiz, int *contador)
{
    if (raiz) {
        /* Primeiro os maiores (direita), depois o proprio no, depois os menores (esquerda). */
        arvore_listar_decrescente(raiz->direita, contador);
        (*contador)++;
        alimento_exibir(&(raiz->alimento->dado));
        arvore_listar_decrescente(raiz->esquerda, contador);
    }
}

/* Lista em ordem decrescente so as chaves da faixa, sem descer onde nao ha nenhuma. */
/* A faixa e fechada [min, max] e *contador soma quantos alimentos entraram nela. */
void arvore_listar_faixa(const NoArvore_t *raiz, double min, double max,
                         int *contador)
{
    if (raiz) {
        /* A direita so tem chaves >= raiz. Se a raiz ja passou de max, */
        /* nada a direita serve e ela nem e visitada (poda). */
        if (raiz->chave <= max) {
            arvore_listar_faixa(raiz->direita, min, max, contador);
        }

        /* O proprio no so aparece se a chave estiver dentro da faixa. */
        if (raiz->chave >= min && raiz->chave <= max) {
            (*contador)++;
            alimento_exibir(&(raiz->alimento->dado));
        }

        /* A esquerda so tem chaves < raiz. Se a raiz ja esta abaixo de min, */
        /* nada a esquerda serve e ela tambem nao e visitada. */
        if (raiz->chave >= min) {
            arvore_listar_faixa(raiz->esquerda, min, max, contador);
        }
    }
}

/* Libera os filhos primeiro e depois o proprio no. */
/* Se liberasse o pai antes, perderia os ponteiros para os filhos e eles vazariam. */
void arvore_liberar(NoArvore_t *raiz)
{
    if (raiz) {
        arvore_liberar(raiz->esquerda);
        arvore_liberar(raiz->direita);
        free(raiz);
    }
}
