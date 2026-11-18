/* interface.c: menu, leitura do teclado e acoes de cada opcao (P2). */
/* Toda leitura do teclado passa por ler_linha, que nunca escreve fora do buffer. */

#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Tamanho maximo de uma linha digitada pelo usuario. */
#define TAM_ENTRADA 64

/* Valor lido quando a linha nao tem um numero. */
#define LEITURA_INVALIDA 0

static int ler_inteiro(int valor_fim_entrada);
static int ler_double(double *valor);
static int ler_linha(char *buffer);
static int ler_faixa(const char *grandeza, double *min, double *max);
static NoCategoria_t *selecionar_categoria(NoCategoria_t *cabeca);
static void exibir_rodape(const char *texto, int total);

/* Mostra o titulo do P2 e o arquivo que sera carregado. */
void exibir_abertura(const char *caminho_bin)
{
    exibir_titulo("   Programa P2: Estruturas Dinamicas (Etapa C)       ");
    printf("Carregando alimentos de '%s' para a memoria...\n", caminho_bin);
}

/* Mostra as opcoes de 1 a 9. */
/* Os numeros vem do enum, entao o menu e o switch do P2 nunca ficam diferentes. */
void exibir_menu_principal(void)
{
    printf("\n==================== MENU PRINCIPAL (P2) ====================\n");
    printf("%d. Listar todas as categorias de alimentos (ordem alfabetica)\n",
           OPCAO_LISTAR_CATEGORIAS);
    printf("%d. Listar alimentos de uma categoria (ordem alfabetica)\n",
           OPCAO_LISTAR_ALIMENTOS);
    printf("%d. Listar alimentos de uma categoria por energia decrescente "
           "(arvore)\n", OPCAO_LISTAR_POR_ENERGIA);
    printf("%d. Listar alimentos de uma categoria por proteina decrescente "
           "(arvore)\n", OPCAO_LISTAR_POR_PROTEINA);
    printf("%d. Filtrar alimentos por faixa de energia [min, max] "
           "(arvore)\n", OPCAO_FAIXA_ENERGIA);
    printf("%d. Filtrar alimentos por faixa de proteina [min, max] "
           "(arvore)\n", OPCAO_FAIXA_PROTEINA);
    printf("%d. Remover uma categoria de alimentos\n",
           OPCAO_REMOVER_CATEGORIA);
    printf("%d. Remover um alimento especifico\n", OPCAO_REMOVER_ALIMENTO);
    printf("%d. Encerrar o programa (salvando alteracoes se houver)\n",
           OPCAO_ENCERRAR);
    printf("=============================================================\n");
    printf("Escolha uma opcao (%d-%d): ", OPCAO_LISTAR_CATEGORIAS,
           OPCAO_ENCERRAR);
}

/* Le um inteiro e so converte para o enum se estiver entre as opcoes. */
/* O valor e lido como int, conferido entre o minimo e o maximo e so entao convertido com cast. */
OpcaoMenu_e ler_opcao_menu(void)
{
    OpcaoMenu_e opcao = OPCAO_INVALIDA;
    /* No fim da entrada (Ctrl+D) vem OPCAO_ENCERRAR, e o programa sai em vez de repetir o menu. */
    int valor = ler_inteiro(OPCAO_ENCERRAR);

    if (valor >= OPCAO_LISTAR_CATEGORIAS && valor <= OPCAO_ENCERRAR) {
        opcao = (OpcaoMenu_e)valor;
    }

    return opcao;
}

/* Le uma linha e tira o inteiro com sscanf; no fim da entrada devolve valor_fim_entrada. */
/* Se a linha nao tiver numero (ex.: "abc"), devolve LEITURA_INVALIDA. */
static int ler_inteiro(int valor_fim_entrada)
{
    char buffer[TAM_ENTRADA];
    int valor = valor_fim_entrada;

    if (ler_linha(buffer)) {
        valor = LEITURA_INVALIDA;
        sscanf(buffer, "%d", &valor);
    }

    return valor;
}

/* Opcao 2: mostra os alimentos da categoria escolhida. */
void acao_listar_alimentos(NoCategoria_t *cabeca)
{
    NoCategoria_t *no_categoria = selecionar_categoria(cabeca);

    if (no_categoria) {
        lista_exibir_alimentos_categoria(no_categoria);
    }
}

/* Mostra a lista de categorias e le o numero escolhido. */
/* Devolve o no da categoria, ou NULL (com aviso) se o numero nao existir. */
static NoCategoria_t *selecionar_categoria(NoCategoria_t *cabeca)
{
    NoCategoria_t *selecionada = NULL;
    int indice;

    lista_exibir_categorias(cabeca);
    printf("Digite o numero da categoria desejada: ");
    indice = ler_inteiro(LEITURA_INVALIDA);

    selecionada = lista_obter_categoria_por_indice(cabeca, indice);
    if (!selecionada) {
        printf("AVISO: Categoria invalida!\n");
    }

    return selecionada;
}

/* Lista uma categoria pela arvore de energia ou de proteina, da maior para a menor. */
/* por_energia = 1 escolhe a arvore de energia; 0 escolhe a de proteina. */
void listar_decrescente(NoCategoria_t *cabeca, int por_energia)
{
    NoCategoria_t *no_categoria = selecionar_categoria(cabeca);

    if (no_categoria) {
        /* Comeca com proteina e troca para energia se for o caso: uma funcao serve as duas opcoes. */
        const NoArvore_t *arvore = no_categoria->arvore_proteina;
        const char *grandeza = "PROTEINA";
        int contador = 0;

        if (por_energia) {
            arvore = no_categoria->arvore_energia;
            grandeza = "ENERGIA";
        }
        printf("\n--- ALIMENTOS DE '%s' POR %s DECRESCENTE (ARVORE) ---\n",
               categoria_para_texto(no_categoria->categoria), grandeza);
        arvore_listar_decrescente(arvore, &contador);
        exibir_rodape("alimentos exibidos", contador);
    }
}

/* Le a faixa e lista a categoria pela arvore de energia ou de proteina. */
/* Se o minimo ou o maximo nao for numero, avisa e nao lista nada. */
void listar_faixa(NoCategoria_t *cabeca, int por_energia)
{
    NoCategoria_t *no_categoria = selecionar_categoria(cabeca);

    if (no_categoria) {
        /* Mesma ideia de listar_decrescente: comeca com proteina e troca se for energia. */
        const NoArvore_t *arvore = no_categoria->arvore_proteina;
        const char *pergunta = "proteina (g)";
        const char *grandeza = "PROTEINA";
        const char *unidade = "G";
        double min;
        double max;
        int contador = 0;

        if (por_energia) {
            arvore = no_categoria->arvore_energia;
            pergunta = "energia (kcal)";
            grandeza = "ENERGIA";
            unidade = "KCAL";
        }
        if (ler_faixa(pergunta, &min, &max)) {
            printf("\n--- ALIMENTOS DE '%s' COM %s ENTRE %.1f E %.1f %s ---\n",
                   categoria_para_texto(no_categoria->categoria), grandeza, min, max, unidade);
            arvore_listar_faixa(arvore, min, max, &contador);
            exibir_rodape("alimentos encontrados na faixa", contador);
        }
    }
}

/* Pede minimo e maximo; devolve 0 se algum valor nao for numero. */
/* Se vierem invertidos (minimo maior que maximo), troca os dois e avisa. */
static int ler_faixa(const char *grandeza, double *min, double *max)
{
    int valida;
    double troca;

    /* So pede o maximo se o minimo ja foi um numero valido. */
    printf("Informe o valor MINIMO de %s: ", grandeza);
    valida = ler_double(min);
    if (valida) {
        printf("Informe o valor MAXIMO de %s: ", grandeza);
        valida = ler_double(max);
    }

    if (!valida) {
        printf("AVISO: Valor invalido! Digite um numero.\n");
    } else if (*min > *max) {
        printf("AVISO: O minimo era maior que o maximo; os valores foram trocados.\n");
        troca = *min;
        *min = *max;
        *max = troca;
    }

    return valida;
}

/* Le um numero real da linha; devolve 1 se a linha tinha um numero. */
static int ler_double(double *valor)
{
    char buffer[TAM_ENTRADA];
    int valido = 0;

    *valor = 0.0;
    if (ler_linha(buffer)) {
        valido = (sscanf(buffer, "%lf", valor) == 1);
    }

    return valido;
}

/* Le uma linha com fgets e joga fora o que passar do buffer; devolve 0 no fim da entrada. */
static int ler_linha(char *buffer)
{
    /* fgets nunca escreve mais que TAM_ENTRADA bytes, mesmo se a linha for gigante. */
    int leu = (fgets(buffer, TAM_ENTRADA, stdin) != NULL);
    int caractere = 0;

    /* Sem '\n' no buffer quer dizer que a linha nao coube: descarta o resto ate o fim da linha, */
    /* senao o resto seria lido como se fosse a proxima resposta. */
    if (leu && strchr(buffer, '\n') == NULL) {
        caractere = getchar();
        while (caractere != '\n' && caractere != EOF) {
            caractere = getchar();
        }
    }

    return leu;
}

/* Mostra a linha de total e a linha de tracos que fecham uma listagem. */
static void exibir_rodape(const char *texto, int total)
{
    printf("Total de %s: %d.\n", texto, total);
    printf("------------------------------------------------------------\n");
}

/* Opcao 7: remove a categoria escolhida com todos os seus alimentos e avisa. */
/* Recebe ponteiro para a cabeca porque a primeira categoria pode ser removida; devolve 1 se removeu. */
int acao_remover_categoria(NoCategoria_t **cabeca)
{
    int removeu = 0;
    const char *nome;
    NoCategoria_t *no_categoria = selecionar_categoria(*cabeca);

    if (no_categoria) {
        /* O nome vem de uma tabela constante, entao o ponteiro continua valido */
        /* mesmo depois que o no da categoria e liberado. */
        nome = categoria_para_texto(no_categoria->categoria);
        removeu = lista_remover_categoria(cabeca, no_categoria->categoria);
        if (removeu) {
            printf("SUCESSO: Categoria '%s' e todos os seus alimentos foram removidos!\n", nome);
        }
    }

    return removeu;
}

/* Opcao 8: mostra os alimentos da categoria e remove o numero digitado. */
/* Devolve 1 se removeu e 0 se o numero nao estava na categoria. */
/* Recebe ponteiro para a cabeca porque uma categoria que fica vazia e removida. */
int acao_remover_alimento(NoCategoria_t **cabeca)
{
    int removeu = 0;
    NoCategoria_t *no_categoria = selecionar_categoria(*cabeca);

    if (no_categoria) {
        int num_alimento;

        lista_exibir_alimentos_categoria(no_categoria);
        printf("Digite o numero do alimento que deseja remover: ");
        num_alimento = ler_inteiro(LEITURA_INVALIDA);
        removeu = lista_remover_alimento(no_categoria, num_alimento);
        if (removeu) {
            printf("SUCESSO: Alimento %d removido e arvores de "
                   "'%s' reconstruidas!\n", num_alimento, categoria_para_texto(no_categoria->categoria));
            /* Sem alimentos a categoria sairia do arquivo na gravacao; */
            /* removendo agora, o menu fica igual ao que o P2 carrega na proxima vez. */
            if (no_categoria->total_alimentos == 0) {
                const char *nome = categoria_para_texto(no_categoria->categoria);

                lista_remover_categoria(cabeca, no_categoria->categoria);
                printf("AVISO: Categoria '%s' ficou sem alimentos e foi removida.\n", nome);
            }
        } else {
            printf("AVISO: Alimento com numero %d nao encontrado "
                   "nesta categoria.\n", num_alimento);
        }
    }

    return removeu;
}

/* Opcao 9: regrava o arquivo so se algo foi removido e libera toda a memoria. */
/* Devolve EXIT_FAILURE se a gravacao falhou; nesse caso as remocoes nao ficam salvas. */
int acao_encerrar(NoCategoria_t **cabeca, int dados_modificados,
                  const char *caminho_bin)
{
    int codigo_retorno = EXIT_SUCCESS;

    /* Sem remocao o arquivo continua igual, entao nem e regravado. */
    if (dados_modificados) {
        int gravados;

        printf("\nAlteracoes detectadas. Regravando '%s' "
               "atualizado...\n", caminho_bin);
        gravados = lista_salvar_binario(*cabeca, caminho_bin);
        if (gravados < 0) {
            fprintf(stderr, "ERRO: Nao foi possivel gravar '%s'. "
                    "As remocoes feitas nao foram salvas.\n", caminho_bin);
            codigo_retorno = EXIT_FAILURE;
        } else {
            printf("SUCESSO: %d registros gravados na versao "
                   "atualizada de '%s'.\n", gravados, caminho_bin);
        }
    } else {
        printf("\nNenhuma alteracao nos dados. O arquivo '%s' "
               "foi mantido intacto.\n", caminho_bin);
    }

    /* Em qualquer caso, libera as listas e as arvores antes de terminar. */
    printf("Liberando toda a memoria alocada (listas e arvores)...\n");
    lista_liberar_tudo(cabeca);
    printf("Memoria liberada com sucesso. Programa finalizado.\n");

    return codigo_retorno;
}
