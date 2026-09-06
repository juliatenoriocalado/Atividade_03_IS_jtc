#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct Tarefa{
    char nome[50];
    int periodo;
    int deadline_relativo;
    int burst;
} Tarefa;

int main(int argc, char *argv[]){
    int ModoRate = 0;
    int ModoEdf = 0;

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
    return 0;
}