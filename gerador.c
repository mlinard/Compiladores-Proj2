/*
 * gerador.c - Implementação do Gerador de Código MEPA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gerador.h"

// Variáveis globais do gerador
static FILE *arquivo_saida = NULL;
static int contador_rotulo = 1;
static char rotulo_buffer[20];

// Inicializa o gerador de código
void inicializar_gerador(FILE *arquivo) {
    arquivo_saida = arquivo;
    contador_rotulo = 1;
}

// Finaliza o gerador (adiciona marcador de fim)
void finalizar_gerador() {
    if (arquivo_saida != NULL) {
        fprintf(arquivo_saida, "FIM\n");
        fflush(arquivo_saida);
    }
}

// Gera uma instrução MEPA no arquivo de saída
void gera_instr_mepa(const char *rotulo, const char *mnemonico, 
                     const char *parametro1, const char *parametro2) {
    if (arquivo_saida == NULL) return;
    
    // Escrever rótulo (se fornecido)
    if (rotulo != NULL && strlen(rotulo) > 0) {
        fprintf(arquivo_saida, "%s: ", rotulo);
    }
    
    // Escrever mnemônico (obrigatório)
    fprintf(arquivo_saida, "%s", mnemonico);
    
    // IMPORTANTE: Não colocar espaço entre os parâmetros, apenas vírgula
    // conforme especificado no documento do projeto
    
    // Escrever primeiro parâmetro (se fornecido)
    if (parametro1 != NULL && strlen(parametro1) > 0) {
        fprintf(arquivo_saida, " %s", parametro1);
        
        // Escrever segundo parâmetro (se fornecido)
        if (parametro2 != NULL && strlen(parametro2) > 0) {
            fprintf(arquivo_saida, ",%s", parametro2);
        }
    }
    
    fprintf(arquivo_saida, "\n");
}

// Gera um novo rótulo único
char* novo_rotulo() {
    snprintf(rotulo_buffer, sizeof(rotulo_buffer), "L%d", contador_rotulo);
    contador_rotulo++;
    return rotulo_buffer;
}

// Obtém o rótulo atual (último gerado) sem incrementar
char* obter_rotulo_atual() {
    snprintf(rotulo_buffer, sizeof(rotulo_buffer), "L%d", contador_rotulo - 1);
    return rotulo_buffer;
}

// Libera recursos do gerador
void liberar_gerador() {
    arquivo_saida = NULL;
    contador_rotulo = 1;
}