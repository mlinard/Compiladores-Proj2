/*
 * CORREÇÕES NECESSÁRIAS NO asdr.c
 * 
 * 1. REMOVER a linha 92 (que não existe mais):
 *    void inicializar_analex(FILE *arquivo)
 * 
 * 2. A função parse_programa() JÁ ESTÁ CORRETA
 * 
 * O restante do seu código está funcionalmente correto!
 * Apenas copie o asdr.c que você tem, mas VERIFIQUE:
 */

// ✅ Seu código já está assim (correto):
int parse_programa() {
    erro_sintatico = 0;
    parse_ini();
    return !erro_sintatico;
}

// ❌ NÃO DEVE TER esta função (remova se existir):
// void inicializar_analex(FILE *arquivo) { ... }

/*
 * OBSERVAÇÃO IMPORTANTE:
 * 
 * O asdr.c que você enviou está 99% correto!
 * Apenas certifique-se de que NÃO há a função inicializar_analex()
 * dentro dele, pois essa função não existe no analex.o fornecido.
 * 
 * O bootstrap correto é feito no main.c:
 *   lookahead = obter_atomo();
 *   linha_atual = lookahead.linha;
 */
