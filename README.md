# Compilador Gamma

- Trabalho para a disciplina de compiladores - UFRGS, 2018/2
- Grupo Gamma: Arthur Vedana e Vitor Vanacor

## Etapa 1: Análise Léxica

- Analisador léxico que utiliza a ferramenta de geração de reconhecedores `flex`.
- Os tokens estão definidos no header `tokens.h`
- As expressões regulares para o reconhecimento estão em `scanner.l` - arquivo de entrada do `flex`
