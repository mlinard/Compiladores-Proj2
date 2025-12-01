/*
 * asdr.h - Interface do Analisador Sintático Descendente Recursivo
 */

#ifndef ASDR_H
#define ASDR_H

#include "analex.h"

// Variáveis globais do parser
extern TInfoAtomo lookahead;
extern int linha_atual;
extern int erro_sintatico;

// Funções de inicialização e controle
void inicializar_analex(FILE *arquivo);
int parse_programa();

// Função auxiliar de verificação de tokens
void verifica(TAtomo token_esperado);

// Funções do ASDR (uma para cada não-terminal da gramática)
void parse_ini();
void parse_id();
void parse_dcl();
int parse_dcl_var();
TAtomo parse_tipo();
int parse_mais_var(TAtomo tipo);
void parse_bco();
void parse_cmd();
void parse_atrib();
void parse_leitura();
void parse_escrita();
void parse_ret();
void parse_selecao();
void parse_repeticao();
void parse_while();
void parse_repeat();
void parse_for();
void parse_exp();
void parse_exp_simples();
void parse_termo();
void parse_fator();

// Funções auxiliares
const char* nome_token(TAtomo token);
void erro_sintatico_msg(const char *esperado, TAtomo encontrado);

#endif