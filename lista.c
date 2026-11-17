/* lista.c: listas ligadas de categorias e de alimentos. */

#include "lista.h"
#include "arquivo.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/* Largura, em letras, da coluna do nome da categoria. */
#define LARGURA_NOME_CATEGORIA 40

/* Letra acentuada em UTF-8: byte 0xC3 e depois um byte de 0x80 a 0xBF. */
#define UTF8_PRIMEIRO_BYTE 0xC3
#define UTF8_SEGUNDO_MIN 0x80
#define UTF8_SEGUNDO_MAX 0xBF
#define UTF8_MINUSCULA_INICIO 0xA0
#define UTF8_DIF_MAIUSCULA 0x20

/* Letra base das minusculas 0xC3 0xA0 a 0xC3 0xBF; ponto = nao tratada. */
static const char * const BASE_ACENTOS = "aaaaa..ceeeeiiii..ooooo..uuuu...";

static void lista_inserir_alimento(NoCategoria_t **cabeca_categorias,
                                   const Alimento_t *alimento);
static NoCategoria_t *lista_obter_ou_criar_categoria(NoCategoria_t **cabeca,
                                                     Categoria_e categoria);
static void categoria_indexar(NoCategoria_t *no_categoria, NoAlimento_t *alimento);
static void categoria_reconstruir_arvores(NoCategoria_t *no_categoria);
static void categoria_liberar(NoCategoria_t *no_categoria);
static int comparar_texto(const char *texto_a, const char *texto_b);
static int letra_base(const char **texto);

/* Le dados.bin inteiro para um vetor e insere alimento por alimento. */
NoCategoria_t *lista_carregar_de_binario(const char *caminho_bin,
                                         int *total_lidos)
{
    NoCategoria_t *cabeca = NULL;
    int contador = 0;
    long tamanho = arquivo_obter_tamanho(caminho_bin);

    if (tamanho >= (long)sizeof(Alimento_t)) {
        int capacidade = (int)(tamanho / (long)sizeof(Alimento_t));
        Alimento_t *vetor = malloc((size_t)capacidade * sizeof(Alimento_t));
        int lidos;
        int i;

        if (!vetor) {
            fprintf(stderr, "ERRO FATAL: Memoria insuficiente para ler '%s'.\n",
                    caminho_bin);
            exit(EXIT_FAILURE);
        }

        lidos = arquivo_ler_binario(caminho_bin, vetor, capacidade);
        for (i = 0; i < lidos; i++) {
            lista_inserir_alimento(&cabeca, &vetor[i]);
            contador++;
        }

        free(vetor);
    }

    if (total_lidos) {
        *total_lidos = contador;
    }

    return cabeca;
}

/* Coloca o alimento na sua categoria em ordem alfabetica e nas duas arvores. */
static void lista_inserir_alimento(NoCategoria_t **cabeca_categorias,
                                   const Alimento_t *alimento)
{
    NoCategoria_t *no_categoria = lista_obter_ou_criar_categoria(cabeca_categorias,
                                                              alimento->categoria);
    NoAlimento_t *novo = calloc(1, sizeof(NoAlimento_t));
    NoAlimento_t *anterior = NULL;
    NoAlimento_t *atual = no_categoria->alimentos;

    if (!novo) {
        fprintf(stderr, "ERRO FATAL: Memoria insuficiente para o no do alimento.\n");
        exit(EXIT_FAILURE);
    }
    novo->dado = *alimento;

    while (atual && comparar_texto(novo->dado.descricao, atual->dado.descricao) >= 0) {
        anterior = atual;
        atual = atual->proximo;
    }

    novo->proximo = atual;
    if (!anterior) {
        no_categoria->alimentos = novo;
    } else {
        anterior->proximo = novo;
    }

    no_categoria->total_alimentos++;
    categoria_indexar(no_categoria, novo);
}

/* Devolve a categoria se ja existe; senao cria e coloca em ordem alfabetica do nome. */
static NoCategoria_t *lista_obter_ou_criar_categoria(NoCategoria_t **cabeca,
                                                     Categoria_e categoria)
{
    const char *nome = categoria_para_texto(categoria);
    NoCategoria_t *anterior = NULL;
    NoCategoria_t *atual = *cabeca;
    NoCategoria_t *resultado = NULL;

    while (atual && atual->categoria != categoria &&
           comparar_texto(nome, categoria_para_texto(atual->categoria)) >= 0) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual && atual->categoria == categoria) {
        resultado = atual;
    } else {
        resultado = calloc(1, sizeof(NoCategoria_t));
        if (!resultado) {
            fprintf(stderr, "ERRO FATAL: Memoria insuficiente para o no da categoria.\n");
            exit(EXIT_FAILURE);
        }
        resultado->categoria = categoria;
        resultado->proximo = atual;
        if (!anterior) {
            *cabeca = resultado;
        } else {
            anterior->proximo = resultado;
        }
    }

    return resultado;
}

/* Insere o alimento na arvore de energia e na de proteina da categoria. */
static void categoria_indexar(NoCategoria_t *no_categoria, NoAlimento_t *alimento)
{
    no_categoria->arvore_energia = arvore_inserir(no_categoria->arvore_energia,
                                               alimento->dado.energia_kcal,
                                               alimento);
    no_categoria->arvore_proteina = arvore_inserir(no_categoria->arvore_proteina,
                                                alimento->dado.proteina_g,
                                                alimento);
}

/* Mostra cada categoria com sua posicao e quantos alimentos tem. */
void lista_exibir_categorias(const NoCategoria_t *cabeca)
{
    const NoCategoria_t *atual = cabeca;
    int indice = 1;

    printf("\n=== LISTA DE CATEGORIAS (Ordem Alfabetica) ===\n");
    while (atual) {
        printf("  [%2d] ", indice);
        imprimir_alinhado(categoria_para_texto(atual->categoria), LARGURA_NOME_CATEGORIA);
        printf(" (%d %s)\n", atual->total_alimentos,
               atual->total_alimentos == 1 ? "alimento" : "alimentos");
        indice++;
        atual = atual->proximo;
    }
    printf("==============================================\n\n");
}

/* Anda pela lista ate a posicao pedida. */
NoCategoria_t *lista_obter_categoria_por_indice(NoCategoria_t *cabeca,
                                                int indice)
{
    NoCategoria_t *resultado = NULL;
    NoCategoria_t *atual = cabeca;
    int posicao = 1;

    while (atual && posicao < indice) {
        atual = atual->proximo;
        posicao++;
    }

    if (posicao == indice) {
        resultado = atual;
    }

    return resultado;
}

/* Mostra os alimentos na ordem da lista, que ja e alfabetica. */
void lista_exibir_alimentos_categoria(const NoCategoria_t *no_categoria)
{
    const NoAlimento_t *atual = no_categoria->alimentos;
    int contador = 0;

    printf("\n--- ALIMENTOS DA CATEGORIA: %s (Ordem Alfabetica) ---\n",
           categoria_para_texto(no_categoria->categoria));
    while (atual) {
        contador++;
        alimento_exibir(&(atual->dado));
        atual = atual->proximo;
    }
    printf("Total de alimentos exibidos: %d.\n"
           "----------------------------------------------------\n\n", contador);
}

/* Tira o alimento da lista, libera o no e monta as arvores de novo. */
int lista_remover_alimento(NoCategoria_t *no_categoria, int numero_alimento)
{
    int removido = 0;
    NoAlimento_t *anterior = NULL;
    NoAlimento_t *atual = no_categoria->alimentos;

    while (atual && atual->dado.numero != numero_alimento) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual) {
        if (!anterior) {
            no_categoria->alimentos = atual->proximo;
        } else {
            anterior->proximo = atual->proximo;
        }
        free(atual);
        no_categoria->total_alimentos--;
        categoria_reconstruir_arvores(no_categoria);
        removido = 1;
    }

    return removido;
}

/* Apaga as duas arvores e insere de novo os alimentos que sobraram. */
static void categoria_reconstruir_arvores(NoCategoria_t *no_categoria)
{
    NoAlimento_t *atual = no_categoria->alimentos;

    arvore_liberar(no_categoria->arvore_energia);
    arvore_liberar(no_categoria->arvore_proteina);
    no_categoria->arvore_energia = NULL;
    no_categoria->arvore_proteina = NULL;

    while (atual) {
        categoria_indexar(no_categoria, atual);
        atual = atual->proximo;
    }
}

/* Tira a categoria da lista e libera tudo o que ela tem. */
int lista_remover_categoria(NoCategoria_t **cabeca_categorias,
                            Categoria_e categoria)
{
    int removido = 0;
    NoCategoria_t *anterior = NULL;
    NoCategoria_t *atual = *cabeca_categorias;

    while (atual && atual->categoria != categoria) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual) {
        if (!anterior) {
            *cabeca_categorias = atual->proximo;
        } else {
            anterior->proximo = atual->proximo;
        }
        categoria_liberar(atual);
        removido = 1;
    }

    return removido;
}

/* Copia os alimentos de todas as categorias para um vetor e grava o vetor. */
int lista_salvar_binario(const NoCategoria_t *cabeca, const char *caminho_bin)
{
    int quantidade = 0;
    int total_gravados;
    int i = 0;
    const NoCategoria_t *no_categoria = cabeca;
    const NoAlimento_t *no_alimento;
    Alimento_t *vetor = NULL;

    while (no_categoria) {
        quantidade += no_categoria->total_alimentos;
        no_categoria = no_categoria->proximo;
    }

    if (quantidade > 0) {
        vetor = calloc((size_t)quantidade, sizeof(Alimento_t));
        if (!vetor) {
            fprintf(stderr,
                    "ERRO FATAL: Memoria insuficiente para gravar '%s'.\n",
                    caminho_bin);
            exit(EXIT_FAILURE);
        }

        no_categoria = cabeca;
        while (no_categoria) {
            no_alimento = no_categoria->alimentos;
            while (no_alimento) {
                vetor[i] = no_alimento->dado;
                i++;
                no_alimento = no_alimento->proximo;
            }
            no_categoria = no_categoria->proximo;
        }
    }

    total_gravados = arquivo_gravar_binario(caminho_bin, vetor, quantidade);
    if (total_gravados != quantidade) {
        total_gravados = -1;
    }
    free(vetor);

    return total_gravados;
}

/* Libera categoria por categoria, guardando a proxima antes do free. */
void lista_liberar_tudo(NoCategoria_t **cabeca)
{
    NoCategoria_t *categoria_atual = *cabeca;
    NoCategoria_t *proxima_categoria;

    while (categoria_atual) {
        proxima_categoria = categoria_atual->proximo;
        categoria_liberar(categoria_atual);
        categoria_atual = proxima_categoria;
    }

    *cabeca = NULL;
}

/* Libera as duas arvores, a lista de alimentos e o no da categoria. */
static void categoria_liberar(NoCategoria_t *no_categoria)
{
    NoAlimento_t *alimento_atual = no_categoria->alimentos;
    NoAlimento_t *proximo_alimento;

    arvore_liberar(no_categoria->arvore_energia);
    arvore_liberar(no_categoria->arvore_proteina);

    while (alimento_atual) {
        proximo_alimento = alimento_atual->proximo;
        free(alimento_atual);
        alimento_atual = proximo_alimento;
    }

    free(no_categoria);
}

/* Compara como o strcmp, mas ignora acento e maiuscula. */
static int comparar_texto(const char *texto_a, const char *texto_b)
{
    int letra_a = letra_base(&texto_a);
    int letra_b = letra_base(&texto_b);

    while (letra_a == letra_b && letra_a != '\0') {
        letra_a = letra_base(&texto_a);
        letra_b = letra_base(&texto_b);
    }

    return letra_a - letra_b;
}

/* Le uma letra do texto, avanca o ponteiro e devolve a letra minuscula sem acento. */
static int letra_base(const char **texto)
{
    const unsigned char *letra_atual = (const unsigned char *)*texto;
    int letra = tolower(letra_atual[0]);
    int tamanho = 1;

    if (letra_atual[0] == UTF8_PRIMEIRO_BYTE &&
        letra_atual[1] >= UTF8_SEGUNDO_MIN &&
        letra_atual[1] <= UTF8_SEGUNDO_MAX) {
        int indice = (letra_atual[1] | UTF8_DIF_MAIUSCULA) - UTF8_MINUSCULA_INICIO;

        if (BASE_ACENTOS[indice] != '.') {
            letra = BASE_ACENTOS[indice];
            tamanho = 2;
        }
    }

    *texto += tamanho;
    return letra;
}
