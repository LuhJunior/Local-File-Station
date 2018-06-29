/*  CLIENTE */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include "Client.h"

int main(int argc, char **argv){
    char command[31], nome[20];
    Menu();
    CommandSpaces();
    do{
        fgets(command, 30, stdin);
        command[strlen(command)-1] = '\0';
        Minusculo(command);
        if(!strcmp(command, "buscar")) BuscarArquivo();
        else if(!strcmp(command, "adicionar no cache")){
            printf("Nome do arquivo: ");
            fgets(command, 20, stdin);
            AddNoCache("cache.txt", nome);
        }
        else if(!strcmp(command, "adicionar servidor")){
            AddServidorConsulta();
        }
        else if(!strcmp(command, "sair")){
            break;
        }
        else if(!strcasecmp(command, "ajuda")){
            Menu();
        }
        else{
            printf("comando invalido!\n");
            printf("use o comando ajuda para imprimir o os comandos disponiveis\n");
        }
         CommandSpaces();
    }while(1);
    printf("Encerrando cliente ...\n");
    return 0;
}
