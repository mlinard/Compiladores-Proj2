/*
 * gerador.h - Interface do Gerador de Código MEPA
 */

#ifndef GERADOR_H
#define GERADOR_H

#include <stdio.h>

// Funções de inicialização e finalização
void inicializar_gerador(FILE *arquivo);
void finalizar_gerador();

// Função principal de geração de instrução MEPA
void gera_instr_mepa(const char *rotulo, const char *mnemonico, 
                     const char *parametro1, const char *parametro2);

// Funções para gerenciamento de rótulos
char* novo_rotulo();
char* obter_rotulo_atual();

// Funções auxiliares
void liberar_gerador();

#endif