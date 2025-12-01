/*
 * asdr.c - Implementação do Analisador Sintático Descendente Recursivo
 * ETAPA 2: Expressões, Comandos e Validações Semânticas
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "asdr.h"
#include "analex.h"
#include "tabsimb.h"
#include "gerador.h"

// Variáveis globais
TInfoAtomo lookahead;
int linha_atual = 1;
int erro_sintatico = 0;

// Função para obter nome legível do token
const char* nome_token(TAtomo token) {
    switch(token) {
        case sPRG: return "prg";
        case sVAR: return "var";
        case sSUBROT: return "subrot";
        case sRETURN: return "return";
        case sINT: return "int";
        case sFLOAT: return "float";
        case sBOOL: return "bool";
        case sCHAR: return "char";
        case sBEGIN: return "begin";
        case sEND: return "end";
        case sWRITE: return "write";
        case sREAD: return "read";
        case sIF: return "if";
        case sTHEN: return "then";
        case sELSE: return "else";
        case sWHILE: return "while";
        case sDO: return "do";
        case sREPEAT: return "repeat";
        case sUNTIL: return "until";
        case sFOR: return "for";
        case sTO: return "to";
        case sIDENT: return "identificador";
        case sNUM_INT: return "número inteiro";
        case sNUM_FLOAT: return "número real";
        case sMENOR: return "<";
        case sMENOR_IG: return "<=";
        case sIGUAL: return "=";
        case sDIFERENTE: return "!=";
        case sMAIOR: return ">";
        case sMAIOR_IG: return ">=";
        case sSOMA: return "+";
        case sSUBT: return "-";
        case sOU: return "ou";
        case sMULT: return "*";
        case sDIV: return "/";
        case sE: return "e";
        case sNAO: return "nao";
        case sABRE_PARENT: return "(";
        case sFECHA_PARENT: return ")";
        case sABRE_COLCH: return "[";
        case sFECHA_COLCH: return "]";
        case sPONTO: return ".";
        case sVIRG: return ",";
        case sPONTO_VIRG: return ";";
        case sATRIB: return "<-";
        case sEOF: return "fim de arquivo";
        default: return "token desconhecido";
    }
}

// Função para reportar erro sintático
void erro_sintatico_msg(const char *esperado, TAtomo encontrado) {
    erro_sintatico = 1;
    printf("Erro (%d): Esperado %s, encontrado '%s'\n", 
           linha_atual, esperado, nome_token(encontrado));
}

// Inicializa o analisador léxico
void inicializar_analex(FILE *arquivo) {
    lookahead = obter_atomo();
    linha_atual = lookahead.linha;
}

// Função de verificação de token
void verifica(TAtomo token_esperado) {
    if (lookahead.atomo == token_esperado) {
        lookahead = obter_atomo();
        linha_atual = lookahead.linha;
    } else {
        char msg[100];
        snprintf(msg, sizeof(msg), "token '%s'", nome_token(token_esperado));
        erro_sintatico_msg(msg, lookahead.atomo);
        exit(1); // Modo pânico: encerra compilação
    }
}

// Função principal do parser
int parse_programa() {
    erro_sintatico = 0;
    parse_ini();
    return !erro_sintatico;
}

// <ini> ::= sPRG <id> ; [<dcl>] [<sub>] <bco> .
void parse_ini() {
    verifica(sPRG);
    
    TInfoAtomo id = lookahead;
    parse_id();
    
    // Inserir programa na tabela de símbolos
    ts_inserir(id.lexema, CAT_PROGRAMA, TIPO_VOID, -1);
    
    verifica(sPONTO_VIRG);
    
    // Gerar instrução inicial
    gera_instr_mepa(NULL, "INPP", NULL, NULL);
    
    int qtde_vars = 0;
    
    // Declarações de variáveis (opcional)
    if (lookahead.atomo == sVAR) {
        qtde_vars = parse_dcl();
        if (qtde_vars > 0) {
            char buffer[20];
            snprintf(buffer, sizeof(buffer), "%d", qtde_vars);
            gera_instr_mepa(NULL, "AMEM", buffer, NULL);
        }
    }
    
    // Sub-rotinas (opcional) - ignoramos para simplificar
    if (lookahead.atomo == sSUBROT) {
        printf("Aviso: sub-rotinas ainda não implementadas\n");
    }
    
    // Bloco principal
    parse_bco();
    
    verifica(sPONTO);
    
    // Finalizar programa
    if (qtde_vars > 0) {
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%d", qtde_vars);
        gera_instr_mepa(NULL, "DMEM", buffer, NULL);
    }
    gera_instr_mepa(NULL, "PARA", NULL, NULL);
}

// <id> ::= sIDENT
void parse_id() {
    verifica(sIDENT);
}

// <dcl> ::= sVAR <dcl_var> ; { <dcl_var> ; }
int parse_dcl() {
    int total_vars = 0;
    
    verifica(sVAR);
    
    total_vars += parse_dcl_var();
    verifica(sPONTO_VIRG);
    
    while (lookahead.atomo == sINT || lookahead.atomo == sFLOAT || 
           lookahead.atomo == sBOOL || lookahead.atomo == sCHAR) {
        total_vars += parse_dcl_var();
        verifica(sPONTO_VIRG);
    }
    
    return total_vars;
}

// <dcl_var> ::= <tipo> <id> <mais_var>
int parse_dcl_var() {
    int count = 1;
    TAtomo tipo = parse_tipo();
    
    TInfoAtomo id = lookahead;
    parse_id();
    
    // Inserir variável na tabela de símbolos
    int endereco = obter_proximo_endereco();
    ts_inserir(id.lexema, CAT_VARIAVEL, tipo, endereco);
    
    count += parse_mais_var(tipo);
    
    return count;
}

// <tipo> ::= sINT | sFLOAT | sBOOL | sCHAR
TAtomo parse_tipo() {
    TAtomo tipo = lookahead.atomo;
    
    if (tipo == sINT) {
        verifica(sINT);
    } else if (tipo == sFLOAT) {
        verifica(sFLOAT);
    } else if (tipo == sBOOL) {
        verifica(sBOOL);
    } else if (tipo == sCHAR) {
        verifica(sCHAR);
    } else {
        erro_sintatico_msg("tipo (int, float, bool ou char)", tipo);
        exit(1);
    }
    
    return tipo;
}

// <mais_var> ::= , <id> <mais_var> | ε
int parse_mais_var(TAtomo tipo) {
    int count = 0;
    
    if (lookahead.atomo == sVIRG) {
        verifica(sVIRG);
        
        TInfoAtomo id = lookahead;
        parse_id();
        
        // Inserir variável na tabela de símbolos
        int endereco = obter_proximo_endereco();
        ts_inserir(id.lexema, CAT_VARIAVEL, tipo, endereco);
        
        count = 1 + parse_mais_var(tipo);
    }
    
    return count;
}

// <bco> ::= sBEGIN { <cmd> ; } sEND
void parse_bco() {
    verifica(sBEGIN);
    
    while (lookahead.atomo != sEND && lookahead.atomo != sEOF) {
        parse_cmd();
        verifica(sPONTO_VIRG);
    }
    
    verifica(sEND);
}

// <cmd> ::= <atrib> | <leitura> | <escrita> | <selecao> | <repeticao> | <ret> | <bco>
void parse_cmd() {
    switch (lookahead.atomo) {
        case sIDENT:
            parse_atrib();
            break;
        case sREAD:
            parse_leitura();
            break;
        case sWRITE:
            parse_escrita();
            break;
        case sIF:
            parse_selecao();
            break;
        case sWHILE:
        case sREPEAT:
        case sFOR:
            parse_repeticao();
            break;
        case sRETURN:
            parse_ret();
            break;
        case sBEGIN:
            parse_bco();
            break;
        default:
            erro_sintatico_msg("comando", lookahead.atomo);
            exit(1);
    }
}

// <atrib> ::= <id> <- <exp>
void parse_atrib() {
    TInfoAtomo id = lookahead;
    verifica(sIDENT);
    
    // Validação semântica: verificar se variável foi declarada
    RegistroTS *registro = ts_buscar(id.lexema);
    if (registro == NULL) {
        printf("Erro semântico (%d): variável '%s' não declarada\n", 
               linha_atual, id.lexema);
        exit(1);
    }
    
    verifica(sATRIB);
    
    // Avaliar expressão (resultado fica no topo da pilha)
    parse_exp();
    
    // Gerar instrução de armazenamento
    char nivel[20], endereco[20];
    snprintf(nivel, sizeof(nivel), "0");
    snprintf(endereco, sizeof(endereco), "%d", registro->endereco);
    gera_instr_mepa(NULL, "ARMZ", nivel, endereco);
}

// <leitura> ::= sREAD ( <id> )
void parse_leitura() {
    verifica(sREAD);
    verifica(sABRE_PARENT);
    
    TInfoAtomo id = lookahead;
    verifica(sIDENT);
    
    // Validação semântica: verificar se variável foi declarada
    RegistroTS *registro = ts_buscar(id.lexema);
    if (registro == NULL) {
        printf("Erro semântico (%d): variável '%s' não declarada\n", 
               linha_atual, id.lexema);
        exit(1);
    }
    
    verifica(sFECHA_PARENT);
    
    // Gerar instruções: ler e armazenar
    gera_instr_mepa(NULL, "LEIT", NULL, NULL);
    
    char nivel[20], endereco[20];
    snprintf(nivel, sizeof(nivel), "0");
    snprintf(endereco, sizeof(endereco), "%d", registro->endereco);
    gera_instr_mepa(NULL, "ARMZ", nivel, endereco);
}

// <escrita> ::= sWRITE ( <exp> )
void parse_escrita() {
    verifica(sWRITE);
    verifica(sABRE_PARENT);
    
    // Avaliar expressão (resultado fica no topo da pilha)
    parse_exp();
    
    verifica(sFECHA_PARENT);
    
    // Gerar instrução de impressão
    gera_instr_mepa(NULL, "IMPR", NULL, NULL);
}

// <ret> ::= sRETURN <exp>
void parse_ret() {
    verifica(sRETURN);
    parse_exp();
    // Instrução de retorno será implementada com sub-rotinas
}

// <selecao> ::= sIF <exp> sTHEN <cmd> [sELSE <cmd>]
void parse_selecao() {
    verifica(sIF);
    
    // Avaliar condição
    parse_exp();
    
    verifica(sTHEN);
    
    // Gerar rótulo para desvio falso
    char *rotulo_falso = novo_rotulo();
    char rotulo_falso_str[20];
    strcpy(rotulo_falso_str, rotulo_falso);
    
    gera_instr_mepa(NULL, "DSVF", rotulo_falso_str, NULL);
    
    // Comando do THEN
    parse_cmd();
    
    if (lookahead.atomo == sELSE) {
        // Gerar rótulo para fim do IF
        char *rotulo_fim = novo_rotulo();
        char rotulo_fim_str[20];
        strcpy(rotulo_fim_str, rotulo_fim);
        
        gera_instr_mepa(NULL, "DSVS", rotulo_fim_str, NULL);
        gera_instr_mepa(rotulo_falso_str, "NADA", NULL, NULL);
        
        verifica(sELSE);
        parse_cmd();
        
        gera_instr_mepa(rotulo_fim_str, "NADA", NULL, NULL);
    } else {
        // Sem ELSE
        gera_instr_mepa(rotulo_falso_str, "NADA", NULL, NULL);
    }
}

// <repeticao> ::= <while> | <repeat> | <for>
void parse_repeticao() {
    if (lookahead.atomo == sWHILE) {
        parse_while();
    } else if (lookahead.atomo == sREPEAT) {
        parse_repeat();
    } else if (lookahead.atomo == sFOR) {
        parse_for();
    }
}

// <while> ::= sWHILE <exp> sDO <cmd>
void parse_while() {
    verifica(sWHILE);
    
    // Rótulo de início do loop
    char *rotulo_inicio = novo_rotulo();
    char rotulo_inicio_str[20];
    strcpy(rotulo_inicio_str, rotulo_inicio);
    
    gera_instr_mepa(rotulo_inicio_str, "NADA", NULL, NULL);
    
    // Avaliar condição
    parse_exp();
    
    verifica(sDO);
    
    // Rótulo de saída
    char *rotulo_fim = novo_rotulo();
    char rotulo_fim_str[20];
    strcpy(rotulo_fim_str, rotulo_fim);
    
    gera_instr_mepa(NULL, "DSVF", rotulo_fim_str, NULL);
    
    // Corpo do loop
    parse_cmd();
    
    // Voltar ao início
    gera_instr_mepa(NULL, "DSVS", rotulo_inicio_str, NULL);
    gera_instr_mepa(rotulo_fim_str, "NADA", NULL, NULL);
}

// <repeat> ::= sREPEAT { <cmd> ; } sUNTIL <exp>
void parse_repeat() {
    verifica(sREPEAT);
    
    // Rótulo de início
    char *rotulo_inicio = novo_rotulo();
    char rotulo_inicio_str[20];
    strcpy(rotulo_inicio_str, rotulo_inicio);
    
    gera_instr_mepa(rotulo_inicio_str, "NADA", NULL, NULL);
    
    // Comandos do corpo
    while (lookahead.atomo != sUNTIL && lookahead.atomo != sEOF) {
        parse_cmd();
        verifica(sPONTO_VIRG);
    }
    
    verifica(sUNTIL);
    
    // Avaliar condição
    parse_exp();
    
    // Se falso, volta ao início
    gera_instr_mepa(NULL, "DSVF", rotulo_inicio_str, NULL);
}

// <for> ::= sFOR ( <atrib> ; <exp> ; <atrib> ) <cmd>
void parse_for() {
    verifica(sFOR);
    verifica(sABRE_PARENT);
    
    // Inicialização
    parse_atrib();
    verifica(sPONTO_VIRG);
    
    // Rótulo de início
    char *rotulo_inicio = novo_rotulo();
    char rotulo_inicio_str[20];
    strcpy(rotulo_inicio_str, rotulo_inicio);
    
    gera_instr_mepa(rotulo_inicio_str, "NADA", NULL, NULL);
    
    // Condição
    parse_exp();
    
    verifica(sPONTO_VIRG);
    
    // Rótulo de saída e corpo
    char *rotulo_fim = novo_rotulo();
    char rotulo_fim_str[20];
    strcpy(rotulo_fim_str, rotulo_fim);
    
    gera_instr_mepa(NULL, "DSVF", rotulo_fim_str, NULL);
    
    // Pular para o corpo (evitar incremento na primeira iteração)
    char *rotulo_corpo = novo_rotulo();
    char rotulo_corpo_str[20];
    strcpy(rotulo_corpo_str, rotulo_corpo);
    
    gera_instr_mepa(NULL, "DSVS", rotulo_corpo_str, NULL);
    
    // Rótulo do incremento
    char *rotulo_incr = novo_rotulo();
    char rotulo_incr_str[20];
    strcpy(rotulo_incr_str, rotulo_incr);
    
    gera_instr_mepa(rotulo_incr_str, "NADA", NULL, NULL);
    
    // Incremento
    parse_atrib();
    
    verifica(sFECHA_PARENT);
    
    // Voltar para testar condição
    gera_instr_mepa(NULL, "DSVS", rotulo_inicio_str, NULL);
    
    // Corpo do loop
    gera_instr_mepa(rotulo_corpo_str, "NADA", NULL, NULL);
    parse_cmd();
    
    // Após corpo, executar incremento
    gera_instr_mepa(NULL, "DSVS", rotulo_incr_str, NULL);
    
    // Fim do for
    gera_instr_mepa(rotulo_fim_str, "NADA", NULL, NULL);
}

// <exp> ::= <exp_simples> [<op_rel> <exp_simples>]
void parse_exp() {
    parse_exp_simples();
    
    // Operadores relacionais
    if (lookahead.atomo == sMENOR || lookahead.atomo == sMENOR_IG ||
        lookahead.atomo == sIGUAL || lookahead.atomo == sDIFERENTE ||
        lookahead.atomo == sMAIOR || lookahead.atomo == sMAIOR_IG) {
        
        TAtomo op = lookahead.atomo;
        lookahead = obter_atomo();
        linha_atual = lookahead.linha;
        
        parse_exp_simples();
        
        // Gerar instrução de comparação
        switch(op) {
            case sMENOR:
                gera_instr_mepa(NULL, "CMME", NULL, NULL);
                break;
            case sMENOR_IG:
                gera_instr_mepa(NULL, "CMEG", NULL, NULL);
                break;
            case sIGUAL:
                gera_instr_mepa(NULL, "CMIG", NULL, NULL);
                break;
            case sDIFERENTE:
                gera_instr_mepa(NULL, "CMDG", NULL, NULL);
                break;
            case sMAIOR:
                gera_instr_mepa(NULL, "CMMA", NULL, NULL);
                break;
            case sMAIOR_IG:
                gera_instr_mepa(NULL, "CMAG", NULL, NULL);
                break;
            default:
                break;
        }
    }
}

// <exp_simples> ::= [+|-] <termo> { (+|-|ou) <termo> }
void parse_exp_simples() {
    int sinal_negativo = 0;
    
    // Sinal unário opcional
    if (lookahead.atomo == sSOMA) {
        verifica(sSOMA);
    } else if (lookahead.atomo == sSUBT) {
        verifica(sSUBT);
        sinal_negativo = 1;
    }
    
    parse_termo();
    
    // Aplicar sinal negativo se necessário
    if (sinal_negativo) {
        gera_instr_mepa(NULL, "INVR", NULL, NULL);
    }
    
    // Operadores aditivos
    while (lookahead.atomo == sSOMA || lookahead.atomo == sSUBT || 
           lookahead.atomo == sOU) {
        TAtomo op = lookahead.atomo;
        lookahead = obter_atomo();
        linha_atual = lookahead.linha;
        
        parse_termo();
        
        // Gerar instrução de operação
        switch(op) {
            case sSOMA:
                gera_instr_mepa(NULL, "SOMA", NULL, NULL);
                break;
            case sSUBT:
                gera_instr_mepa(NULL, "SUBT", NULL, NULL);
                break;
            case sOU:
                gera_instr_mepa(NULL, "DISJ", NULL, NULL);
                break;
            default:
                break;
        }
    }
}

// <termo> ::= <fator> { (*|/|e) <fator> }
void parse_termo() {
    parse_fator();
    
    // Operadores multiplicativos
    while (lookahead.atomo == sMULT || lookahead.atomo == sDIV || 
           lookahead.atomo == sE) {
        TAtomo op = lookahead.atomo;
        lookahead = obter_atomo();
        linha_atual = lookahead.linha;
        
        parse_fator();
        
        // Gerar instrução de operação
        switch(op) {
            case sMULT:
                gera_instr_mepa(NULL, "MULT", NULL, NULL);
                break;
            case sDIV:
                gera_instr_mepa(NULL, "DIVI", NULL, NULL);
                break;
            case sE:
                gera_instr_mepa(NULL, "CONJ", NULL, NULL);
                break;
            default:
                break;
        }
    }
}

// <fator> ::= <id> | <num> | ( <exp> ) | nao <fator>
void parse_fator() {
    if (lookahead.atomo == sIDENT) {
        TInfoAtomo id = lookahead;
        verifica(sIDENT);
        
        // Validação semântica: verificar se variável foi declarada
        RegistroTS *registro = ts_buscar(id.lexema);
        if (registro == NULL) {
            printf("Erro semântico (%d): variável '%s' não declarada\n", 
                   linha_atual, id.lexema);
            exit(1);
        }
        
        // Gerar instrução para carregar valor
        char nivel[20], endereco[20];
        snprintf(nivel, sizeof(nivel), "0");
        snprintf(endereco, sizeof(endereco), "%d", registro->endereco);
        gera_instr_mepa(NULL, "CRVL", nivel, endereco);
        
    } else if (lookahead.atomo == sNUM_INT) {
        TInfoAtomo num = lookahead;
        verifica(sNUM_INT);
        
        // Gerar instrução para carregar constante
        gera_instr_mepa(NULL, "CRCT", num.lexema, NULL);
        
    } else if (lookahead.atomo == sNUM_FLOAT) {
        TInfoAtomo num = lookahead;
        verifica(sNUM_FLOAT);
        
        // Gerar instrução para carregar constante
        gera_instr_mepa(NULL, "CRCT", num.lexema, NULL);
        
    } else if (lookahead.atomo == sABRE_PARENT) {
        verifica(sABRE_PARENT);
        parse_exp();
        verifica(sFECHA_PARENT);
        
    } else if (lookahead.atomo == sNAO) {
        verifica(sNAO);
        parse_fator();
        gera_instr_mepa(NULL, "NEGA", NULL, NULL);
        
    } else {
        erro_sintatico_msg("fator (identificador, número ou expressão)", 
                          lookahead.atomo);
        exit(1);
    }
}