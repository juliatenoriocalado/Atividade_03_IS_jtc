#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

typedef struct Tarefa{
    //fixos
    char nome[50];
    int periodo;
    int deadline_relativo;
    int burst;
    //mudam durante a execução
    int restante;
    int deadline_absoluto;
    int proxima_chegada;
    int ativa;
    //são meio que os contadores ou acumuladores da tarefa durante a simulaçao inteira
    int deadlines_perdidos;
    int execucao_completa;
    int morta;
} Tarefa;

int main(int argc, char *argv[]){
    int ModoRate = 0;
    int ModoEdf = 0;
    char linha[100];
    char *fim;
    int tempoTotalSimulacao = 0;
    FILE *arquivo;

    if (argc != 3){
        fprintf(stderr, "Erro: numero incorreto de argumentos.\n");
        exit(1);
    }

    if (strcmp(argv[1], "rate") == 0){
        ModoRate = 1;
    }
    else if (strcmp(argv[1], "edf") == 0){
        ModoEdf = 1;
    }
    else{
        fprintf(stderr, "Erro: algoritmo deve ser rate ou edf.\n");
        exit(1);
    }
    
    arquivo = fopen(argv[2], "r");

    if (arquivo == NULL){
        perror("Erro ao abrir arquivo de entrada");;
        exit(1);
    }

    if (fgets(linha, sizeof(linha), arquivo) == NULL){
        fprintf(stderr, "Erro: arquivo sem tempo total de simulacao.\n");
        fclose(arquivo);
        exit(1);
    }

    linha[strcspn(linha, "\r\n")] = '\0';

    errno = 0;
    long valor = strtol(linha, &fim, 10);

    if (fim == linha){
        fprintf(stderr, "Erro: tempo total nao numerico.\n");
        fclose(arquivo);
        exit(1);
    }

    if (*fim != '\0'){
        fprintf(stderr, "Erro: tempo total possui caracteres invalidos.\n");
        fclose(arquivo);
        exit(1);
    }

    if (errno == ERANGE || valor > INT_MAX){
        fprintf(stderr, "Erro: tempo total fora do intervalo valido.\n");
        fclose(arquivo);
        exit(1);
    }

    if (valor <= 0){
        fprintf(stderr, "Erro: tempo total deve ser positivo.\n");
        fclose(arquivo);
        exit(1);
    }

    tempoTotalSimulacao = valor;
    Tarefa tarefa;
    Tarefa *listaDeTarefas = NULL;
    int quantidadeDeTarefas = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL){

        char extra;

        int camposLidos = sscanf(
            linha,
            "%49s %d %d %d %c",
            tarefa.nome,
            &tarefa.periodo,
            &tarefa.deadline_relativo,
            &tarefa.burst,
            &extra
        );

        if (camposLidos != 4){
            fprintf(stderr, "Erro: linha de tarefa mal formatada.\n");
            free(listaDeTarefas);
            fclose(arquivo);
            exit(1);
        }

        if (tarefa.periodo <= 0 ||
            tarefa.deadline_relativo <= 0 ||
            tarefa.burst <= 0){

            fprintf(stderr, "Erro: valores da tarefa devem ser positivos.\n");
            free(listaDeTarefas);
            fclose(arquivo);
            exit(1);
        }

        if (tarefa.deadline_relativo > tarefa.periodo){
            fprintf(stderr, "Erro: deadline maior que periodo.\n");
            free(listaDeTarefas);
            fclose(arquivo);
            exit(1);
        }

        if (tarefa.burst > tarefa.deadline_relativo){
            fprintf(stderr, "Erro: burst maior que deadline.\n");
            free(listaDeTarefas);
            fclose(arquivo);
            exit(1);
        }

        tarefa.restante = tarefa.burst;
        tarefa.deadline_absoluto = tarefa.deadline_relativo;
        tarefa.proxima_chegada = tarefa.periodo;
        tarefa.ativa = 1;
        tarefa.deadlines_perdidos = 0;
        tarefa.execucao_completa = 0;
        tarefa.morta = 0;

        Tarefa *temporario = realloc(listaDeTarefas, sizeof(Tarefa) * (quantidadeDeTarefas + 1));

        if (temporario == NULL){
            fprintf(stderr, "Erro ao alocar memoria.\n");
            free(listaDeTarefas);
            fclose(arquivo);
            exit(1);

        }

        listaDeTarefas = temporario;
        listaDeTarefas[quantidadeDeTarefas] = tarefa;
        quantidadeDeTarefas++;

    }

    int ultimoExecutado = -1;
    int unidadeDoBloco = 0;
    int unidadesIdle = 0;

    for (int tempo = 0; tempo < tempoTotalSimulacao; tempo++){

        for (int i = 0; i < quantidadeDeTarefas; i++){

            if (listaDeTarefas[i].ativa && tempo == listaDeTarefas[i].deadline_absoluto && listaDeTarefas[i].restante > 0){
                listaDeTarefas[i].deadlines_perdidos++;
                if (i == ultimoExecutado && unidadeDoBloco > 0){
                    printf("[%s] for %d units - L\n", listaDeTarefas[i].nome, unidadeDoBloco);
                    ultimoExecutado = -1;
                    unidadeDoBloco = 0;
                }
                listaDeTarefas[i].restante = 0;
                listaDeTarefas[i].ativa = 0;
            }

            if (tempo == listaDeTarefas[i].proxima_chegada){
                listaDeTarefas[i].restante = listaDeTarefas[i].burst;
                listaDeTarefas[i].deadline_absoluto = tempo + listaDeTarefas[i].deadline_relativo;
                listaDeTarefas[i].proxima_chegada = tempo + listaDeTarefas[i].periodo;
                listaDeTarefas[i].ativa = 1;
            }
        }

        int indiceEscolhido = -1;

        if (ModoRate){
            for (int i = 0; i < quantidadeDeTarefas; i++){
                if (listaDeTarefas[i].ativa == 0){
                    continue;
                }

                if (indiceEscolhido == -1){
                    indiceEscolhido = i;
                }

                else if (listaDeTarefas[i].periodo < listaDeTarefas[indiceEscolhido].periodo){
                    indiceEscolhido = i;
                }
            }
        }

        if (ModoEdf){
            for (int i = 0; i < quantidadeDeTarefas; i++){
                if (listaDeTarefas[i].ativa == 0){
                    continue;
                }

                if (indiceEscolhido == -1){
                    indiceEscolhido = i;
                }

                else if (listaDeTarefas[i].deadline_absoluto < listaDeTarefas[indiceEscolhido].deadline_absoluto){
                    indiceEscolhido = i;
                }
            }
        }
        if (indiceEscolhido == -1){
            unidadesIdle++;
        }

        else{
            if (unidadesIdle > 0){
                printf("idle for %d units\n", unidadesIdle);
                unidadesIdle = 0;
            }
        }

        if (indiceEscolhido != -1){

            if (ultimoExecutado == -1){
                ultimoExecutado = indiceEscolhido;
                unidadeDoBloco = 0;
            }

            else if (indiceEscolhido != ultimoExecutado){

                printf("[%s] for %d units - H\n",
                    listaDeTarefas[ultimoExecutado].nome,
                    unidadeDoBloco);

                ultimoExecutado = indiceEscolhido;
                unidadeDoBloco = 0;
            }

            listaDeTarefas[indiceEscolhido].restante--;
            unidadeDoBloco++;

            if (listaDeTarefas[indiceEscolhido].restante == 0){
                printf("[%s] for %d units - F\n", listaDeTarefas[indiceEscolhido].nome, unidadeDoBloco);
                listaDeTarefas[indiceEscolhido].ativa = 0;
                ultimoExecutado = -1;
                unidadeDoBloco = 0;
                listaDeTarefas[indiceEscolhido].execucao_completa++;
            }
        }
    }

    for (int i = 0; i < quantidadeDeTarefas; i++){
        if (listaDeTarefas[i].ativa && listaDeTarefas[i].restante > 0){
            listaDeTarefas[i].morta = 1;
        }
    }

    if (unidadesIdle > 0){
        printf("idle for %d units\n", unidadesIdle);
    }

    printf("LOST DEADLINES\n");

    for (int i = 0; i < quantidadeDeTarefas; i++){
        printf("[%s] %d\n",
            listaDeTarefas[i].nome,
            listaDeTarefas[i].deadlines_perdidos);
    }

    printf("COMPLETE EXECUTION\n");

    for (int i = 0; i < quantidadeDeTarefas; i++){
        printf("[%s] %d\n",
            listaDeTarefas[i].nome,
            listaDeTarefas[i].execucao_completa);
    }

    printf("KILLED\n");

    for (int i = 0; i < quantidadeDeTarefas; i++){
        printf("[%s] %d\n",
            listaDeTarefas[i].nome,
            listaDeTarefas[i].morta);
    }

    fclose(arquivo);
    free(listaDeTarefas);

    return 0;
}