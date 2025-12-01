# LPDC - Compilador para LPD com Tradução Dirigida a Sintaxe

**Projeto 2 de Compiladores**

## 📋 Descrição

Compilador completo para a linguagem LPD que realiza:
- ✅ Análise Léxica (fornecida via `analex.o`)
- ✅ Análise Sintática (ASDR - Analisador Sintático Descendente Recursivo)
- ✅ Análise Semântica (Tabela de Símbolos + validações)
- ✅ Geração de Código (instruções MEPA)

## 🗂️ Estrutura do Projeto

```
.
├── main.c          # Ponto de entrada e orquestração
├── analex.h        # Interface do analisador léxico (fornecido)
├── analex.o        # Analisador léxico pré-compilado (fornecido)
├── asdr.h          # Interface do parser
├── asdr.c          # Implementação do ASDR
├── tabsimb.h       # Interface da Tabela de Símbolos
├── tabsimb.c       # Implementação da TS
├── gerador.h       # Interface do gerador MEPA
├── gerador.c       # Implementação do gerador
├── Makefile        # Compilação automatizada
└── README.md       # Este arquivo
```

## 🚀 Como Compilar

### Pré-requisitos
- GCC (GNU C Compiler)
- Make
- Sistema Linux (Ubuntu recomendado)
- Arquivo `analex.o` fornecido pelo professor

### Compilação

```bash
make
```

Isso irá:
1. Compilar todos os arquivos `.c`
2. Linkar com `analex.o`
3. Gerar o executável `lpdc`

### Limpeza

```bash
make clean      # Remove arquivos objeto e executável
make distclean  # Limpeza completa (inclui .mepa e .ts)
```

## 💻 Como Usar

```bash
./lpdc programa.lpd
```

### Saídas Geradas

O compilador gera automaticamente:
- **programa.mepa** - Código em linguagem MEPA
- **programa.ts** - Tabela de Símbolos

### Exemplo

Arquivo `teste.lpd`:
```lpd
prg exemplo;
var
    int x, y;
begin
    x <- 10;
    y <- 20;
    write(x + y);
end.
```

Execução:
```bash
./lpdc teste.lpd
```

Saída esperada:
```
Compilando 'teste.lpd'...
Código compilado com sucesso!
```

## 📝 Status de Implementação

### ✅ ETAPA 1 - Estrutura Base (COMPLETA)
- [x] Estrutura de arquivos e módulos
- [x] Interface com analisador léxico
- [x] Sistema de mensagens de erro
- [x] Makefile com flags corretas

### ✅ ETAPA 2 - Tabela de Símbolos (COMPLETA)
- [x] Estrutura de dados (lista encadeada)
- [x] Inserção e busca
- [x] Validação de unicidade
- [x] Salvamento em arquivo `.ts`

### ✅ ETAPA 3 - Parser Básico (PARCIAL)
- [x] Função `verifica()`
- [x] Regras: `<ini>`, `<dcl>`, `<bco>`
- [x] Declaração de variáveis
- [x] Estrutura de blocos
- [ ] Expressões (próxima etapa)
- [ ] Comandos completos (próxima etapa)

### 🔄 ETAPA 4 - Parser Completo (EM DESENVOLVIMENTO)
- [ ] Expressões aritméticas
- [ ] Expressões relacionais e lógicas
- [ ] Comandos: atribuição, leitura, escrita
- [ ] Estruturas de controle: if, while, for, repeat

### 🔄 ETAPA 5 - Geração de Código (PARCIAL)
- [x] Estrutura básica
- [x] Instruções: INPP, AMEM, DMEM, PARA
- [ ] Instruções de expressões
- [ ] Instruções de controle de fluxo

## 🧪 Testes

### Teste Simples
```bash
make test
```

### Teste Manual
```bash
./lpdc seu_programa.lpd
cat seu_programa.mepa    # Ver código gerado
cat seu_programa.ts      # Ver tabela de símbolos
```

## ⚠️ Observações Importantes

1. **Formato MEPA**: O gerador NÃO coloca espaços entre parâmetros (apenas vírgula), conforme especificação
2. **Modo Pânico**: Erros sintáticos interrompem a compilação imediatamente
3. **Análise Léxica**: Fornecida via `analex.o` - não modificar
4. **Gramática**: Seguimos o Apêndice B (simplificada, sem vetores/sub-rotinas para geração de código)

## 📚 Referências

- Documento de Especificação da Linguagem LPD
- Conjunto de Instruções MEPA
- Gramática Livre de Contexto (GLC) - Apêndice A/B

## 🔧 Troubleshooting

### Erro: "undefined reference to 'obter_atomo'"
- Certifique-se que `analex.o` está no diretório
- Verifique se o Makefile inclui `analex.o` na linkagem

### Erro: "Esperado token X, encontrado Y"
- Erro sintático no código fonte LPD
- Verifique a linha indicada

### Warnings do GCC
- O código compila com `-Wall -Wextra` sem warnings

## 👥 Desenvolvimento

**Status Atual**: ETAPA 1 completa e testada
**Próximos Passos**: Implementar expressões e comandos completos

---

*Compiladores - Projeto 2*
