/*
 * LPDC - Compilador para LPD com Tradução Dirigida a Sintaxe
 * Projeto 2 de Compiladores
 * 
 * main.c - Ponto de entrada e orquestração do compilador
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analex.h"
#include "asdr.h"
#include "tabsimb.h"
#include "gerador.h"

// Variáveis globais
FILE *arquivo_fonte = NULL;
FILE *arquivo_mepa = NULL;
FILE *arquivo_ts = NULL;
char nome_arquivo[256];

// Função auxiliar para extrair nome base do arquivo
void extrair_nome_base(const char *caminho, char *base) {
    const char *ultimo_barra = strrchr(caminho, '/');
    const char *nome = ultimo_barra ? ultimo_barra + 1 : caminho;
    
    strcpy(base, nome);
    char *ponto = strrchr(base, '.');
    if (ponto) *ponto = '\0';
}

// Função para criar arquivos de saída
int criar_arquivos_saida(const char *nome_base) {
    char caminho[300];
    
    // Criar arquivo .mepa
    snprintf(caminho, sizeof(caminho), "%s.mepa", nome_base);
    arquivo_mepa = fopen(caminho, "w");
    if (!arquivo_mepa) {
        fprintf(stderr, "Erro: não foi possível criar arquivo %s\n", caminho);
        return 0;
    }
    
    // Criar arquivo .ts
    snprintf(caminho, sizeof(caminho), "%s.ts", nome_base);
    arquivo_ts = fopen(caminho, "w");
    if (!arquivo_ts) {
        fprintf(stderr, "Erro: não foi possível criar arquivo %s\n", caminho);
        fclose(arquivo_mepa);
        return 0;
    }
    
    return 1;
}

// Função para fechar todos os arquivos
void fechar_arquivos() {
    if (arquivo_fonte) fclose(arquivo_fonte);
    if (arquivo_mepa) fclose(arquivo_mepa);
    if (arquivo_ts) fclose(arquivo_ts);
}

int main(int argc, char *argv[]) {
    // Verificar argumentos
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.lpd>\n", argv[0]);
        return 1;
    }
    
    // Abrir arquivo fonte
    arquivo_fonte = fopen(argv[1], "r");
    if (!arquivo_fonte) {
        fprintf(stderr, "Erro: não foi possível abrir arquivo '%s'\n", argv[1]);
        return 1;
    }
    
    // Extrair nome base para arquivos de saída
    extrair_nome_base(argv[1], nome_arquivo);
    
    // Criar arquivos de saída
    if (!criar_arquivos_saida(nome_arquivo)) {
        fechar_arquivos();
        return 1;
    }
    
    // Inicializar módulos
    inicializar_analex(arquivo_fonte);
    inicializar_tabela_simbolos();
    inicializar_gerador(arquivo_mepa);
    
    printf("Compilando '%s'...\n", argv[1]);
    
    // Executar análise sintática
    if (parse_programa()) {
        // Sucesso na compilação
        printf("\nCódigo compilado com sucesso!\n");
        
        // Salvar tabela de símbolos
        salvar_tabela_simbolos(arquivo_ts);
        
        // Finalizar geração de código
        finalizar_gerador();
        
        fechar_arquivos();
        return 0;
    } else {
        // Erro na compilação
        printf("\nCompilação finalizada com erros.\n");
        fechar_arquivos();
        return 1;
    }
}