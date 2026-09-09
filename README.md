# Atividade 03 - Escalonamento

## Arquivos

- `escalonamento.c`: contém a implementação do simulador de escalonamento RATE e EDF.
- `Makefile`: usado para compilar o programa e limpar o executável.
- `README.md`: instruções de compilação, execução e testes.

## Compilação

Para compilar:

```bash
make
```

O comando gera o executável:

```text
scheduler
```

Para remover o executável:

```bash
make clean
```

## Execução

Para executar com Rate Monotonic:

```bash
./scheduler rate arquivo.txt
```

Para executar com EDF:

```bash
./scheduler edf arquivo.txt
```

A saída é salva em:

```text
rate_jtc.out
```

ou:

```text
edf_jtc.out
```

dependendo do algoritmo escolhido.

## Formato da entrada

A primeira linha contém o tempo total da simulação.

As linhas seguintes seguem o formato:

```text
NOME PERIODO DEADLINE BURST
```

Exemplo:

```text
100
ATT 20 12 8
NAV 50 30 15
```

## Testes

Foram testados os dois algoritmos, situações de preempção, perda de deadline, execução completa, períodos de idle e tarefas interrompidas pelo final da simulação.

Também foram testadas entradas inválidas, como:

- quantidade incorreta de argumentos;
- algoritmo inválido;
- arquivo inexistente;
- valores não positivos;
- linha mal formatada;
- deadline maior que o período;
- burst maior que o deadline.

Para um teste simples, pode ser utilizado um arquivo `voo.txt` com:

```text
100
ATT 20 12 8
NAV 50 30 15
```

Executando:

```bash
./scheduler rate voo.txt
./scheduler edf voo.txt
```

Os resultados podem ser verificados nos arquivos `rate_jtc.out` e `edf_jtc.out`.

## Ambiente

O programa foi implementado e testado em Ubuntu através do WSL no Windows, utilizando GCC, GNU Make e Visual Studio Code.