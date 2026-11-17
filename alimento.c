/* alimento.c: conversao de categorias e exibicao de um alimento. */

#include "alimento.h"
#include <stdio.h>
#include <string.h>

/* Nome mostrado para uma categoria que nao esta na tabela. */
#define TEXTO_DESCONHECIDA "Desconhecida"

/* Larguras, em letras, das colunas da descricao e da categoria. */
#define LARGURA_DESCRICAO 45
#define LARGURA_CATEGORIA 37

/* Em UTF-8, os bytes de continuacao de uma letra tem a forma 10xxxxxx. */
#define UTF8_MASCARA_CONTINUACAO 0xC0
#define UTF8_BYTE_CONTINUACAO 0x80

/* Final de um texto cortado para caber na coluna. */
#define RETICENCIAS "..."
#define TAM_RETICENCIAS 3

/* Nome de cada categoria, na mesma ordem do enum Categoria_e. */
static const char * const NOMES[TOTAL_CATEGORIAS] = {
    "Alimentos preparados",
    "Bebidas (alcoólicas e não alcoólicas)",
    "Carnes e derivados",
    "Cereais e derivados",
    "Frutas e derivados",
    "Gorduras e óleos",
    "Leguminosas e derivados",
    "Leite e derivados",
    "Miscelâneas",
    "Nozes e sementes",
    "Outros alimentos industrializados",
    "Ovos e derivados",
    "Pescados e frutos do mar",
    "Produtos açucarados",
    "Verduras, hortaliças e derivados"
};

/* Procura o nome na tabela e devolve a categoria (CAT_INVALIDA se nao achar). */
Categoria_e categoria_converter(const char *texto)
{
    int i = 0;

    while (i < TOTAL_CATEGORIAS && strcmp(texto, NOMES[i]) != 0) {
        i++;
    }

    return (Categoria_e)i;
}

/* Devolve o nome da categoria, ou "Desconhecida" fora da tabela. */
const char *categoria_para_texto(Categoria_e categoria)
{
    const char *resultado = TEXTO_DESCONHECIDA;

    if ((int)categoria >= 0 && (int)categoria < TOTAL_CATEGORIAS) {
        resultado = NOMES[categoria];
    }

    return resultado;
}

/* Mostra numero, descricao, categoria, energia e proteina. */
void alimento_exibir(const Alimento_t *alimento)
{
    printf("[%3d] ", alimento->numero);
    imprimir_alinhado(alimento->descricao, LARGURA_DESCRICAO);
    printf(" | ");
    imprimir_alinhado(categoria_para_texto(alimento->categoria), LARGURA_CATEGORIA);
    printf(" | %6.1f kcal | %5.1f g prot\n", alimento->energia_kcal, alimento->proteina_g);
}

/* Mostra o texto com largura letras: completa com espacos ou corta e termina com "...". */
void imprimir_alinhado(const char *texto, int largura)
{
    int tamanho = (int)strlen(texto);
    int letras = 0;
    int bytes_corte = tamanho;
    int i;

    for (i = 0; i < tamanho; i++) {
        if (((unsigned char)texto[i] & UTF8_MASCARA_CONTINUACAO) != UTF8_BYTE_CONTINUACAO) {
            letras++;
            if (letras == largura - TAM_RETICENCIAS + 1) {
                bytes_corte = i;
            }
        }
    }

    if (letras > largura) {
        printf("%.*s%s", bytes_corte, texto, RETICENCIAS);
    } else {
        printf("%s", texto);
        for (i = 0; i < largura - letras; i++) {
            putchar(' ');
        }
    }
}
