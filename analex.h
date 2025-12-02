/*
 * analex.h - Interface do Analisador Léxico
 * Este arquivo de cabeçalho deve ser fornecido junto com analex.o
 */

#ifndef ANALEX_H
#define ANALEX_H

#include <stdio.h>

// Definição dos tokens (átomos) da linguagem LPD
typedef enum {
    sPRG = 0,           // prg
    sVAR = 1,           // var
    sSUBROT = 2,        // subrot
    sRETURN = 3,        // return
    sINT = 4,           // int
    sFLOAT = 5,         // float
    sBOOL = 6,          // bool
    sCHAR = 7,          // char
    sBEGIN = 8,         // begin
    sEND = 9,           // end
    sWRITE = 10,        // write
    sREAD = 11,         // read
    sIF = 12,           // if
    sTHEN = 13,         // then
    sELSE = 14,         // else
    sWHILE = 15,        // while
    sDO = 16,           // do
    sREPEAT = 17,       // repeat
    sUNTIL = 18,        // until
    sFOR = 19,          // for
    sTO = 20,           // to
    sVOID = 21,         // void (tipo de retorno)
    sIDENT = 22,        // identificador
    sNUM_INT = 23,      // número inteiro
    sNUM_FLOAT = 24,    // número real
    sCHAR_CONST = 25,   // constante caractere
    sSTRING = 26,       // string
    sMENOR = 27,        // <
    sMENOR_IG = 28,     // <=
    sIGUAL = 29,        // =
    sDIFERENTE = 30,    // !=
    sMAIOR = 31,        // >
    sMAIOR_IG = 32,     // >=
    sSOMA = 33,         // +
    sSUBT = 34,         // -
    sOU = 35,           // ou
    sMULT = 36,         // *
    sDIV = 37,          // /
    sE = 38,            // e
    sNAO = 39,          // nao
    sABRE_PARENT = 40,  // (
    sFECHA_PARENT = 41, // )
    sABRE_COLCH = 42,   // [
    sFECHA_COLCH = 43,  // ]
    sPONTO = 44,        // .
    sVIRG = 45,         // ,
    sPONTO_VIRG = 46,   // ;
    sATRIB = 47,        // <-
    sEOF = 48           // fim de arquivo
} TAtomo;

// Estrutura de informação do átomo
typedef struct {
    TAtomo atomo;       // Tipo do token
    int linha;          // Linha onde foi encontrado
    char lexema[100];   // Lexema (texto) do token
} TInfoAtomo;

// Variável global do arquivo fonte (usada pelo analex.o)
extern FILE *fonte;

// Função principal do analisador léxico
// Retorna o próximo átomo do arquivo fonte
TInfoAtomo obter_atomo(void);

#endif
