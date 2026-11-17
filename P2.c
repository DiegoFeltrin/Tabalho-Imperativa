/* P2.c: carrega dados.bin em listas e arvores e mostra o menu (Etapa C). */

#include "arquivo.h"
#include "interface.h"
#include "lista.h"
#include <stdio.h>
#include <stdlib.h>

#define ARQUIVO_DADOS_BIN "dados.bin"

/* Carrega os dados e repete o menu ate o usuario escolher sair. */
int main(void)
{
    int codigo_retorno = EXIT_SUCCESS;
    int total_carregados = 0;
    int dados_modificados = 0;
    OpcaoMenu_e opcao = OPCAO_INVALIDA;
    NoCategoria_t *lista_categorias;
    long tamanho_bin;

    exibir_abertura(ARQUIVO_DADOS_BIN);
    tamanho_bin = arquivo_obter_tamanho(ARQUIVO_DADOS_BIN);
    lista_categorias = lista_carregar_de_binario(ARQUIVO_DADOS_BIN,
                                                 &total_carregados);

    if (tamanho_bin != 0 && (!lista_categorias || total_carregados == 0)) {
        fprintf(stderr, "ERRO FATAL: Nao foi possivel carregar '%s'. "
                "Execute o P1 primeiro!\n", ARQUIVO_DADOS_BIN);
        codigo_retorno = EXIT_FAILURE;
    } else {
        printf("SUCESSO: %d registros carregados e organizados em listas "
               "e arvores!\n", total_carregados);
        if (tamanho_bin % (long)sizeof(Alimento_t) != 0) {
            printf("AVISO: '%s' tem um registro incompleto no fim, que foi "
                   "ignorado.\n", ARQUIVO_DADOS_BIN);
        }
    }

    while (codigo_retorno == EXIT_SUCCESS && opcao != OPCAO_ENCERRAR) {
        exibir_menu_principal();
        opcao = ler_opcao_menu();

        switch (opcao) {
        case OPCAO_LISTAR_CATEGORIAS:
            lista_exibir_categorias(lista_categorias);
            break;
        case OPCAO_LISTAR_ALIMENTOS:
            acao_listar_alimentos(lista_categorias);
            break;
        case OPCAO_LISTAR_POR_ENERGIA:
        case OPCAO_LISTAR_POR_PROTEINA:
            listar_decrescente(lista_categorias, opcao == OPCAO_LISTAR_POR_ENERGIA);
            break;
        case OPCAO_FAIXA_ENERGIA:
        case OPCAO_FAIXA_PROTEINA:
            listar_faixa(lista_categorias, opcao == OPCAO_FAIXA_ENERGIA);
            break;
        case OPCAO_REMOVER_CATEGORIA:
            if (acao_remover_categoria(&lista_categorias)) {
                dados_modificados = 1;
            }
            break;
        case OPCAO_REMOVER_ALIMENTO:
            if (acao_remover_alimento(lista_categorias)) {
                dados_modificados = 1;
            }
            break;
        case OPCAO_ENCERRAR:
            codigo_retorno = acao_encerrar(&lista_categorias, dados_modificados,
                                           ARQUIVO_DADOS_BIN);
            break;
        default:
            printf("Opcao invalida! Digite um valor entre %d e %d.\n",
                   OPCAO_LISTAR_CATEGORIAS, OPCAO_ENCERRAR);
            break;
        }
    }

    return codigo_retorno;
}
