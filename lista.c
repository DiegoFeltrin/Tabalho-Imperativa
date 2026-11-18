/* lista.c: listas ligadas de categorias e de alimentos. */
/* Aqui ficam a insercao ordenada, as remocoes e a liberacao da memoria do P2. */

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
/* Exemplo: o segundo byte de "a com til" aponta para a posicao 3, que guarda 'a'. */
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
/* Recebe o caminho do arquivo; devolve a primeira categoria e poe em *total_lidos quantos alimentos leu. */
NoCategoria_t *lista_carregar_de_binario(const char *caminho_bin,
                                         int *total_lidos)
{
    NoCategoria_t *cabeca = NULL;
    int contador = 0;
    long tamanho = arquivo_obter_tamanho(caminho_bin);

    /* So le se o arquivo tiver pelo menos um registro inteiro. */
    if (tamanho >= (long)sizeof(Alimento_t)) {
        /* Quantos registros cabem no arquivo: tamanho dividido pelo tamanho de um registro. */
        int capacidade = (int)(tamanho / (long)sizeof(Alimento_t));
        Alimento_t *vetor = malloc((size_t)capacidade * sizeof(Alimento_t));
        int lidos;
        int i;

        /* Sem memoria nao da para continuar: mostra o erro e encerra. */
        if (!vetor) {
            encerrar_sem_memoria("ler o arquivo binario");
        }

        /* Le tudo de uma vez e depois coloca cada alimento na lista da sua categoria. */
        lidos = arquivo_ler_binario(caminho_bin, vetor, capacidade);
        for (i = 0; i < lidos; i++) {
            lista_inserir_alimento(&cabeca, &vetor[i]);
            contador++;
        }

        /* Os alimentos foram copiados para os nos, entao o vetor ja pode ser liberado. */
        free(vetor);
    }

    if (total_lidos) {
        *total_lidos = contador;
    }

    return cabeca;
}

/* Coloca o alimento na sua categoria em ordem alfabetica e nas duas arvores. */
/* Recebe o ponteiro para a cabeca da lista de categorias e o alimento lido. */
static void lista_inserir_alimento(NoCategoria_t **cabeca_categorias,
                                   const Alimento_t *alimento)
{
    NoCategoria_t *no_categoria = lista_obter_ou_criar_categoria(cabeca_categorias,
                                                              alimento->categoria);
    /* calloc em vez de malloc: o no ja nasce zerado, com proximo = NULL. */
    NoAlimento_t *novo = calloc(1, sizeof(NoAlimento_t));
    /* anterior e o no que fica antes do novo; atual e o no que fica depois dele. */
    NoAlimento_t *anterior = NULL;
    NoAlimento_t *atual = no_categoria->alimentos;

    if (!novo) {
        encerrar_sem_memoria("o no do alimento");
    }
    novo->dado = *alimento;

    /* Anda enquanto o nome do novo vem depois (ou e igual) ao do atual. */
    /* Com o >= um nome igual fica depois do que ja estava, na ordem em que chegou. */
    while (atual && comparar_texto(novo->dado.descricao, atual->dado.descricao) >= 0) {
        anterior = atual;
        atual = atual->proximo;
    }

    /* O novo entra entre anterior e atual. */
    /* Se anterior ficou NULL, o novo vem antes de todos e vira o primeiro da lista. */
    /* Senao ele entra no meio (ou no fim, quando atual e NULL). */
    /* Como cada alimento ja entra no lugar certo, a lista nunca precisa ser ordenada depois. */
    novo->proximo = atual;
    if (!anterior) {
        no_categoria->alimentos = novo;
    } else {
        anterior->proximo = novo;
    }

    /* Atualiza o contador e coloca o alimento tambem nas arvores de energia e proteina. */
    no_categoria->total_alimentos++;
    categoria_indexar(no_categoria, novo);
}

/* Devolve a categoria se ja existe; senao cria e coloca em ordem alfabetica do nome. */
/* Recebe o ponteiro para a cabeca (a cabeca muda se a nova categoria for a primeira). */
static NoCategoria_t *lista_obter_ou_criar_categoria(NoCategoria_t **cabeca,
                                                     Categoria_e categoria)
{
    const char *nome = categoria_para_texto(categoria);
    NoCategoria_t *anterior = NULL;
    NoCategoria_t *atual = *cabeca;
    NoCategoria_t *resultado = NULL;

    /* Uma passada so: para quando acha a categoria procurada ou quando chega */
    /* no primeiro nome que vem depois dela (ali e o lugar de uma categoria nova). */
    while (atual && atual->categoria != categoria &&
           comparar_texto(nome, categoria_para_texto(atual->categoria)) >= 0) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual && atual->categoria == categoria) {
        /* A categoria ja existia: so devolve o no. */
        resultado = atual;
    } else {
        /* Nao existia: cria o no zerado (sem alimentos, arvores vazias, total 0). */
        resultado = calloc(1, sizeof(NoCategoria_t));
        if (!resultado) {
            encerrar_sem_memoria("o no da categoria");
        }
        /* Liga a nova categoria entre anterior e atual, como na lista de alimentos. */
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
/* As duas arvores apontam para o mesmo no do alimento; nada e copiado. */
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
/* A posicao mostrada e o numero que o usuario digita para escolher a categoria. */
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
/* Devolve o no dessa posicao, ou NULL se o numero for menor que 1 ou maior que a lista. */
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

    /* So devolve o no se parou exatamente na posicao pedida. */
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
/* Recebe a categoria e o numero do alimento; devolve 1 se removeu e 0 se nao achou. */
int lista_remover_alimento(NoCategoria_t *no_categoria, int numero_alimento)
{
    int removido = 0;
    NoAlimento_t *anterior = NULL;
    NoAlimento_t *atual = no_categoria->alimentos;

    /* Procura o alimento guardando quem vem antes dele. */
    while (atual && atual->dado.numero != numero_alimento) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual) {
        /* Religa os vizinhos: quem vinha antes passa a apontar para quem vinha depois. */
        /* Se era o primeiro, a lista passa a comecar no proximo. */
        if (!anterior) {
            no_categoria->alimentos = atual->proximo;
        } else {
            anterior->proximo = atual->proximo;
        }
        free(atual);
        no_categoria->total_alimentos--;
        /* Os nos das arvores apontavam para o no que foi liberado, */
        /* entao as duas arvores sao refeitas com os alimentos que sobraram. */
        categoria_reconstruir_arvores(no_categoria);
        removido = 1;
    }

    return removido;
}

/* Apaga as duas arvores e insere de novo os alimentos que sobraram. */
static void categoria_reconstruir_arvores(NoCategoria_t *no_categoria)
{
    NoAlimento_t *atual = no_categoria->alimentos;

    /* Libera so os nos das arvores; os alimentos continuam na lista. */
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
/* Recebe o ponteiro para a cabeca e o enum da categoria; devolve 1 se removeu. */
int lista_remover_categoria(NoCategoria_t **cabeca_categorias,
                            Categoria_e categoria)
{
    int removido = 0;
    NoCategoria_t *anterior = NULL;
    NoCategoria_t *atual = *cabeca_categorias;

    /* Mesma busca com anterior e atual da remocao de alimento. */
    while (atual && atual->categoria != categoria) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual) {
        /* Tira o no da lista antes de liberar, para nenhum ponteiro ficar apontando para ele. */
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
/* Devolve quantos registros gravou, ou -1 se a gravacao falhou. */
int lista_salvar_binario(const NoCategoria_t *cabeca, const char *caminho_bin)
{
    int quantidade = 0;
    int total_gravados;
    int i = 0;
    const NoCategoria_t *no_categoria = cabeca;
    const NoAlimento_t *no_alimento;
    Alimento_t *vetor = NULL;

    /* Primeira passada: conta quantos alimentos restaram para saber o tamanho do vetor. */
    while (no_categoria) {
        quantidade += no_categoria->total_alimentos;
        no_categoria = no_categoria->proximo;
    }

    if (quantidade > 0) {
        vetor = calloc((size_t)quantidade, sizeof(Alimento_t));
        if (!vetor) {
            encerrar_sem_memoria("gravar o arquivo binario");
        }

        /* Segunda passada: copia os alimentos, categoria por categoria, para o vetor. */
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

    /* Com 0 alimentos o arquivo e criado vazio. Gravou menos do que devia = erro (-1). */
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

    /* Guarda o proximo antes de liberar: depois do free o no nao pode mais ser lido. */
    while (categoria_atual) {
        proxima_categoria = categoria_atual->proximo;
        categoria_liberar(categoria_atual);
        categoria_atual = proxima_categoria;
    }

    /* A lista fica vazia, e quem chamou nao fica com um ponteiro para memoria liberada. */
    *cabeca = NULL;
}

/* Libera as duas arvores, a lista de alimentos e o no da categoria. */
/* A ordem importa: primeiro o que esta dentro do no, por ultimo o proprio no. */
static void categoria_liberar(NoCategoria_t *no_categoria)
{
    NoAlimento_t *alimento_atual = no_categoria->alimentos;
    NoAlimento_t *proximo_alimento;

    /* 1) As arvores, que apontam para os alimentos. */
    arvore_liberar(no_categoria->arvore_energia);
    arvore_liberar(no_categoria->arvore_proteina);

    /* 2) Os alimentos, guardando o proximo antes de cada free. */
    while (alimento_atual) {
        proximo_alimento = alimento_atual->proximo;
        free(alimento_atual);
        alimento_atual = proximo_alimento;
    }

    /* 3) O no da categoria. */
    free(no_categoria);
}

/* Compara como o strcmp, mas ignora acento e maiuscula. */
/* Devolve negativo se texto_a vem antes, 0 se sao iguais e positivo se vem depois. */
static int comparar_texto(const char *texto_a, const char *texto_b)
{
    int letra_a = letra_base(&texto_a);
    int letra_b = letra_base(&texto_b);

    /* Compara letra por letra (ja sem acento e em minuscula) ate achar uma diferente ou o fim. */
    while (letra_a == letra_b && letra_a != '\0') {
        letra_a = letra_base(&texto_a);
        letra_b = letra_base(&texto_b);
    }

    return letra_a - letra_b;
}

/* Le uma letra do texto, avanca o ponteiro e devolve a letra minuscula sem acento. */
/* A letra acentuada ocupa 2 bytes em UTF-8, por isso o ponteiro pode andar 1 ou 2 bytes. */
static int letra_base(const char **texto)
{
    const unsigned char *letra_atual = (const unsigned char *)*texto;
    int letra = tolower(letra_atual[0]);
    int tamanho = 1;

    /* Se for uma letra acentuada (0xC3 e mais um byte), troca pela letra sem acento. */
    if (letra_atual[0] == UTF8_PRIMEIRO_BYTE &&
        letra_atual[1] >= UTF8_SEGUNDO_MIN &&
        letra_atual[1] <= UTF8_SEGUNDO_MAX) {
        /* O "| 0x20" junta maiuscula e minuscula; depois vira a posicao em BASE_ACENTOS. */
        int indice = (letra_atual[1] | UTF8_DIF_MAIUSCULA) - UTF8_MINUSCULA_INICIO;

        if (BASE_ACENTOS[indice] != '.') {
            letra = BASE_ACENTOS[indice];
            tamanho = 2;
        }
    }

    *texto += tamanho;
    return letra;
}
