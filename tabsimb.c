/*
 * tabsimb.c - Implementação da Tabela de Símbolos
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabsimb.h"

// Lista encadeada simples para a tabela de símbolos
static RegistroTS *cabeca = NULL;
static int proximo_endereco = 0;

// Inicializa a tabela de símbolos
void inicializar_tabela_simbolos() {
    cabeca = NULL;
    proximo_endereco = 0;
}

// Converte TAtomo para TipoDado
static TipoDado atomo_para_tipo(TAtomo tipo_atomo) {
    switch(tipo_atomo) {
        case sINT: return TIPO_INT;
        case sFLOAT: return TIPO_FLOAT;
        case sBOOL: return TIPO_BOOL;
        case sCHAR: return TIPO_CHAR;
        default: return TIPO_VOID;
    }
}

// Busca um identificador na tabela
RegistroTS* ts_buscar(const char *lexema) {
    RegistroTS *atual = cabeca;
    
    while (atual != NULL) {
        if (strcmp(atual->lexema, lexema) == 0) {
            return atual;
        }
        atual = atual->proximo;
    }
    
    return NULL;
}

// Insere um identificador na tabela
RegistroTS* ts_inserir(const char *lexema, Categoria cat, TAtomo tipo_atomo, int endereco) {
    // Verificar se já existe (regra de unicidade)
    RegistroTS *existente = ts_buscar(lexema);
    if (existente != NULL && cat != CAT_PROGRAMA) {
        printf("Erro semântico: identificador '%s' já declarado\n", lexema);
        exit(1);
    }
    
    // Criar novo registro
    RegistroTS *novo = (RegistroTS*)malloc(sizeof(RegistroTS));
    if (novo == NULL) {
        printf("Erro: falha ao alocar memória para tabela de símbolos\n");
        exit(1);
    }
    
    // Preencher campos
    strncpy(novo->lexema, lexema, sizeof(novo->lexema) - 1);
    novo->lexema[sizeof(novo->lexema) - 1] = '\0';
    novo->categoria = cat;
    novo->tipo = atomo_para_tipo(tipo_atomo);
    novo->endereco = endereco;
    novo->proximo = cabeca;
    
    // Inserir no início da lista
    cabeca = novo;
    
    // Atualizar contador de endereços para variáveis
    if (cat == CAT_VARIAVEL && endereco >= 0) {
        proximo_endereco = endereco + 1;
    }
    
    return novo;
}

// Obtém o próximo endereço disponível para alocação
int obter_proximo_endereco() {
    return proximo_endereco;
}

// Converte categoria para string
const char* categoria_para_string(Categoria cat) {
    switch(cat) {
        case CAT_PROGRAMA: return "programa";
        case CAT_VARIAVEL: return "variável";
        case CAT_CONSTANTE: return "constante";
        case CAT_FUNCAO: return "função";
        case CAT_PARAMETRO: return "parâmetro";
        default: return "desconhecida";
    }
}

// Converte tipo para string
const char* tipo_para_string(TipoDado tipo) {
    switch(tipo) {
        case TIPO_INT: return "int";
        case TIPO_FLOAT: return "float";
        case TIPO_BOOL: return "bool";
        case TIPO_CHAR: return "char";
        case TIPO_VOID: return "void";
        default: return "desconhecido";
    }
}

// Salva a tabela de símbolos em arquivo
void salvar_tabela_simbolos(FILE *arquivo) {
    if (arquivo == NULL) return;
    
    // Percorrer lista e salvar cada registro
    RegistroTS *atual = cabeca;
    
    while (atual != NULL) {
        fprintf(arquivo, "TS[ lex: %s | cat: %s | tip: %s | end: %d ]\n",
                atual->lexema,
                categoria_para_string(atual->categoria),
                tipo_para_string(atual->tipo),
                atual->endereco);
        atual = atual->proximo;
    }
}

// Libera memória da tabela de símbolos
void liberar_tabela_simbolos() {
    RegistroTS *atual = cabeca;
    RegistroTS *proximo;
    
    while (atual != NULL) {
        proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    
    cabeca = NULL;
    proximo_endereco = 0;
}