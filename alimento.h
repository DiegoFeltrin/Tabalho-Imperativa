/* alimento.h: tipo Alimento_t, enum das categorias e funcoes basicas. */
/* Usado pelo P1 e pelo P2: os dois programas enxergam o alimento do mesmo jeito. */

#ifndef ALIMENTO_H
#define ALIMENTO_H

/* Tamanho maximo da descricao de um alimento e do nome de uma categoria. */
/* Cabem 127 letras e o '\0' do final. */
#define TAM_MAX_DESCRICAO 128

/* Numero de categorias da tabela TACO. */
#define TOTAL_CATEGORIAS 15

/* Categorias em ordem alfabetica; CAT_INVALIDA marca um nome nao reconhecido. */
/* Os valores sao os padrao do compilador (0 a 14), e CAT_INVALIDA vale 15. */
typedef enum {
    CAT_ALIMENTOS_PREPARADOS,
    CAT_BEBIDAS,
    CAT_CARNES_E_DERIVADOS,
    CAT_CEREAIS_E_DERIVADOS,
    CAT_FRUTAS_E_DERIVADOS,
    CAT_GORDURAS_E_OLEOS,
    CAT_LEGUMINOSAS_E_DERIVADOS,
    CAT_LEITE_E_DERIVADOS,
    CAT_MISCELANEAS,
    CAT_NOZES_E_SEMENTES,
    CAT_OUTROS_ALIMENTOS_INDUSTRIALIZADOS,
    CAT_OVOS_E_DERIVADOS,
    CAT_PESCADOS_E_FRUTOS_DO_MAR,
    CAT_PRODUTOS_ACUCARADOS,
    CAT_VERDURAS_HORTALICAS_E_DERIVADOS,
    CAT_INVALIDA
} Categoria_e;

/* Um alimento da tabela; a descricao tem tamanho fixo para o registro binario. */
/* Assim todo registro tem o mesmo tamanho (152 bytes) e da para gravar o struct inteiro. */
typedef struct {
    int numero;                         /* numero do alimento na tabela TACO */
    char descricao[TAM_MAX_DESCRICAO];  /* nome do alimento, com acento, em UTF-8 */
    Categoria_e categoria;              /* categoria guardada como enum, nunca como texto */
    double energia_kcal;                /* energia em kcal (NA, Tr, '*' e branco viram 0) */
    double proteina_g;                  /* proteina em gramas (mesma regra do 0) */
} Alimento_t;

/* Converte o nome da categoria do CSV para o enum. */
/* Devolve CAT_INVALIDA se o nome nao estiver na tabela. */
Categoria_e categoria_converter(const char *texto);

/* Devolve o nome da categoria. */
/* O texto e constante: quem chama so le, nao pode alterar nem liberar. */
const char *categoria_para_texto(Categoria_e categoria);

/* Mostra os dados de um alimento em uma linha. */
void alimento_exibir(const Alimento_t *alimento);

/* Mostra um texto e completa com espacos ate a largura em letras. */
/* Se o texto for maior que a largura, corta e termina com "...". */
void imprimir_alinhado(const char *texto, int largura);

/* Mostra "ERRO FATAL: Memoria insuficiente para <motivo>." e encerra o programa. */
/* Usada sempre que um malloc, calloc ou realloc devolve NULL. */
void encerrar_sem_memoria(const char *motivo);

/* Mostra o titulo do trabalho com a linha do programa (P1 ou P2) logo abaixo. */
/* Fica aqui porque os dois programas usam este modulo. */
void exibir_titulo(const char *linha_programa);

#endif /* ALIMENTO_H */
