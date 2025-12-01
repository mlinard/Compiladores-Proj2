/*
 * tabsimb.h - Interface da Tabela de Símbolos
 */

#ifndef TABSIMB_H
#define TABSIMB_H

#include <stdio.h>
#include "analex.h"

// Categorias de identificadores
typedef enum {
    CAT_PROGRAMA,
    CAT_VARIAVEL,
    CAT_CONSTANTE,
    CAT_FUNCAO,
    CAT_PARAMETRO
} Categoria;

// Tipos de dados (mapeados dos tokens)
typedef enum {
    TIPO_VOID = 0,
    TIPO_INT = 4,      // sINT
    TIPO_FLOAT = 5,    // sFLOAT
    TIPO_BOOL = 6,     // sBOOL
    TIPO_CHAR = 7      // sCHAR
} TipoDado;

// Registro da Tabela de Símbolos
typedef struct RegistroTS {
    char lexema[50];
    Categoria categoria;
    TipoDado tipo;
    int endereco;
    struct RegistroTS *proximo;
} RegistroTS;

// Funções da Tabela de Símbolos
void inicializar_tabela_simbolos();
RegistroTS* ts_inserir(const char *lexema, Categoria cat, TAtomo tipo, int endereco);
RegistroTS* ts_buscar(const char *lexema);
void salvar_tabela_simbolos(FILE *arquivo);
void liberar_tabela_simbolos();

// Funções auxiliares
int obter_proximo_endereco();
const char* categoria_para_string(Categoria cat);
const char* tipo_para_string(TipoDado tipo);

#endif