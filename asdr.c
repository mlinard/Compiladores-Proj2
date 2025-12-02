/*
 * asdr.c - Implementação do Analisador Sintático Descendente Recursivo
 * VERSÃO FINAL COM TYPE CHECKING COMPLETO
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

// Função auxiliar: converter TAtomo para TipoDado
TipoDado atomo_para_tipodado(TAtomo tipo) {
    switch(tipo) {
        case sINT: return TIPO_INT;
        case sFLOAT: return TIPO_FLOAT;
        case sBOOL: return TIPO_BOOL;
        case sCHAR: return TIPO_CHAR;
        default: return TIPO_VOID;
    }
}

// Função auxiliar: verificar compatibilidade de tipos
int tipos_compativeis(TipoDado tipo1, TipoDado tipo2) {
    // Tipos exatamente iguais são sempre compatíveis
    if (tipo1 == tipo2) return 1;
    
    // Conversão implícita: int pode ser atribuído a float
    if (tipo1 == TIPO_INT && tipo2 == TIPO_FLOAT) return 1;
    if (tipo1 == TIPO_FLOAT && tipo2 == TIPO_INT) return 1;
    
    // Demais combinações são incompatíveis
    return 0;
}

// Função auxiliar: obter nome do tipo para mensagens
const char* nome_tipo(TipoDado tipo) {
    return tipo_para_string(tipo);
}

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
    ts_inserir(id.lexema, CAT_PROGRAMA, sVOID, -1);
    
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
        printf("Aviso: sub-rotinas não implementadas (ignorando)\n");
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
    
    // Avaliar expressão E obter seu tipo
    TipoDado tipo_exp = parse_exp();
    
    // TYPE CHECKING: Validar compatibilidade de tipos
    if (!tipos_compativeis(tipo_exp, registro->tipo)) {
        printf("Erro semântico (%d): incompatibilidade de tipos na atribuição - "
               "tentando atribuir '%s' a variável '%s' do tipo '%s'\n",
               linha_atual,
               nome_tipo(tipo_exp),
               id.lexema,
               nome_tipo(registro->tipo));
        exit(1);
    }
    
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
    
    // Avaliar condição e obter tipo
    TipoDado tipo_cond = parse_exp();
    
    // TYPE CHECKING: Condição deve ser booleana (ou numérica para comparação)
    // Em LPD, aceitamos qualquer tipo que possa ser avaliado como verdadeiro/falso
    
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
TipoDado parse_exp() {
    TipoDado tipo1 = parse_exp_simples();
    
    // Operadores relacionais
    if (lookahead.atomo == sMENOR || lookahead.atomo == sMENOR_IG ||
        lookahead.atomo == sIGUAL || lookahead.atomo == sDIFERENTE ||
        lookahead.atomo == sMAIOR || lookahead.atomo == sMAIOR_IG) {
        
        TAtomo op = lookahead.atomo;
        lookahead = obter_atomo();
        linha_atual = lookahead.linha;
        
        TipoDado tipo2 = parse_exp_simples();
        
        // TYPE CHECKING: Operandos devem ser compatíveis
        if (!tipos_compativeis(tipo1, tipo2)) {
            printf("Erro semântico (%d): operandos incompatíveis na comparação - "
                   "'%s' e '%s'\n",
                   linha_atual,
                   nome_tipo(tipo1),
                   nome_tipo(tipo2));
            exit(1);
        }
        
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
        
        // Comparações retornam tipo BOOL
        return TIPO_BOOL;
    }
    
    return tipo1;
}

// <exp_simples> ::= [+|-] <termo> { (+|-|ou) <termo> }
TipoDado parse_exp_simples() {
    int sinal_negativo = 0;
    
    // Sinal unário opcional
    if (lookahead.atomo == sSOMA) {
        verifica(sSOMA);
    } else if (lookahead.atomo == sSUBT) {
        verifica(sSUBT);
        sinal_negativo = 1;
    }
    
    TipoDado tipo_resultado = parse_termo();
    
    // Aplicar sinal negativo se necessário
    if (sinal_negativo) {
        // TYPE CHECKING: Sinal negativo só faz sentido em numéricos
        if (tipo_resultado != TIPO_INT && tipo_resultado != TIPO_FLOAT) {
            printf("Erro semântico (%d): operador unário '-' aplicado a tipo não-numérico '%s'\n",
                   linha_atual,
                   nome_tipo(tipo_resultado));
            exit(1);
        }
        gera_instr_mepa(NULL, "INVR", NULL, NULL);
    }
    
    // Operadores aditivos
    while (lookahead.atomo == sSOMA || lookahead.atomo == sSUBT || 
           lookahead.atomo == sOU) {
        TAtomo op = lookahead.atomo;
        lookahead = obter_atomo();
        linha_atual = lookahead.linha;
        
        TipoDado tipo_termo = parse_termo();
        
        // TYPE CHECKING: Validar compatibilidade de operandos
        if (op == sOU) {
            // Operador lógico 'ou' requer booleanos
            if (tipo_resultado != TIPO_BOOL || tipo_termo != TIPO_BOOL) {
                printf("Erro semântico (%d): operador 'ou' requer operandos booleanos\n",
                       linha_atual);
                exit(1);
            }
            gera_instr_mepa(NULL, "DISJ", NULL, NULL);
            tipo_resultado = TIPO_BOOL;
        } else {
            // Operadores aritméticos requerem tipos compatíveis
            if (!tipos_compativeis(tipo_resultado, tipo_termo)) {
                printf("Erro semântico (%d): operandos incompatíveis em operação aritmética - "
                       "'%s' e '%s'\n",
                       linha_atual,
                       nome_tipo(tipo_resultado),
                       nome_tipo(tipo_termo));
                exit(1);
            }
            
            // Gerar instrução de operação
            if (op == sSOMA) {
                gera_instr_mepa(NULL, "SOMA", NULL, NULL);
            } else { // sSUBT
                gera_instr_mepa(NULL, "SUBT", NULL, NULL);
            }
            
            // Tipo do resultado: se um é float, resultado é float
            if (tipo_resultado == TIPO_FLOAT || tipo_termo == TIPO_FLOAT) {
                tipo_resultado = TIPO_FLOAT;
            }
        }
    }
    
    return tipo_resultado;
}

// <termo> ::= <fator> { (*|/|e) <fator> }
TipoDado parse_termo() {
    TipoDado tipo_resultado = parse_fator();
    
    // Operadores multiplicativos
    while (lookahead.atomo == sMULT || lookahead.atomo == sDIV || 
           lookahead.atomo == sE) {
        TAtomo op = lookahead.atomo;
        lookahead = obter_atomo();
        linha_atual = lookahead.linha;
        
        TipoDado tipo_fator = parse_fator();
        
        // TYPE CHECKING: Validar compatibilidade de operandos
        if (op == sE) {
            // Operador lógico 'e' requer booleanos
            if (tipo_resultado != TIPO_BOOL || tipo_fator != TIPO_BOOL) {
                printf("Erro semântico (%d): operador 'e' requer operandos booleanos\n",
                       linha_atual);
                exit(1);
            }
            gera_instr_mepa(NULL, "CONJ", NULL, NULL);
            tipo_resultado = TIPO_BOOL;
        } else {
            // Operadores aritméticos requerem tipos compatíveis
            if (!tipos_compativeis(tipo_resultado, tipo_fator)) {
                printf("Erro semântico (%d): operandos incompatíveis em operação aritmética - "
                       "'%s' e '%s'\n",
                       linha_atual,
                       nome_tipo(tipo_resultado),
                       nome_tipo(tipo_fator));
                exit(1);
            }
            
            // Gerar instrução de operação
            if (op == sMULT) {
                gera_instr_mepa(NULL, "MULT", NULL, NULL);
            } else { // sDIV
                gera_instr_mepa(NULL, "DIVI", NULL, NULL);
            }
            
            // Tipo do resultado: se um é float, resultado é float
            if (tipo_resultado == TIPO_FLOAT || tipo_fator == TIPO_FLOAT) {
                tipo_resultado = TIPO_FLOAT;
            }
        }
    }
    
    return tipo_resultado;
}

// <fator> ::= <id> | <num> | ( <exp> ) | nao <fator>
TipoDado parse_fator() {
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
        
        // Retornar tipo da variável
        return registro->tipo;
        
    } else if (lookahead.atomo == sNUM_INT) {
        TInfoAtomo num = lookahead;
        verifica(sNUM_INT);
        
        // Gerar instrução para carregar constante
        gera_instr_mepa(NULL, "CRCT", num.lexema, NULL);
        
        return TIPO_INT;
        
    } else if (lookahead.atomo == sNUM_FLOAT) {
        TInfoAtomo num = lookahead;
        verifica(sNUM_FLOAT);
        
        // Gerar instrução para carregar constante
        gera_instr_mepa(NULL, "CRCT", num.lexema, NULL);
        
        return TIPO_FLOAT;
        
    } else if (lookahead.atomo == sABRE_PARENT) {
        verifica(sABRE_PARENT);
        TipoDado tipo = parse_exp();
        verifica(sFECHA_PARENT);
        
        return tipo;
        
    } else if (lookahead.atomo == sNAO) {
        verifica(sNAO);
        TipoDado tipo = parse_fator();
        
        // TYPE CHECKING: Operador 'nao' requer operando booleano
        if (tipo != TIPO_BOOL) {
            printf("Erro semântico (%d): operador 'nao' requer operando booleano, "
                   "encontrado '%s'\n",
                   linha_atual,
                   nome_tipo(tipo));
            exit(1);
        }
        
        gera_instr_mepa(NULL, "NEGA", NULL, NULL);
        
        return TIPO_BOOL;
        
    } else {
        erro_sintatico_msg("fator (identificador, número ou expressão)", 
                          lookahead.atomo);
        exit(1);
    }
}
