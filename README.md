# Compilador Gamma

- Trabalho para a disciplina de compiladores - UFRGS, 2018/2
- Grupo Gamma: Arthur Vedana e Vitor Vanacor

## Etapa 1: Análise Léxica

- Analisador léxico que utiliza a ferramenta de geração de reconhecedores `flex`.
- Os tokens estão definidos no header `tokens.h`
- As expressões regulares para o reconhecimento estão em `scanner.l` - arquivo de entrada do `flex`

## Etapa 2: Análise sintática

- Analisador sintático que utiliza a ferramenta `bison`
- A gramática da linguagem é definida no arquivo `parser.y`
  - O arquivo `tokens.h` deixa de existir, pois os tokens são definidos no `parser.y`
