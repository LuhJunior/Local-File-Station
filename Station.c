/*  ESTAÇÃO */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include "Client\Client.h"
#include "Server\Server.h"

int main(int argc, char **argv){
    char command[40], nome[21];
    int n = 0;
    Menu();
    CommandSpaces();
    do{
        fgets(command, 30, stdin);
        command[strlen(command)-1] = '\0';
        Minusculo(command);
        if(!strcmp(command, "buscar")) BuscarArquivo();
        else if(!strcmp(command, "cache")){
            printf("Nome do arquivo: ");
            fgets(nome, 20, stdin);
            nome[strlen(nome)-1] = '\0';
            fflush(stdin);
            AddNoCache("cache.txt", nome);
        }
        else if(!strcmp(command, "servidor consulta")){
            AddServidorConsulta();
        }
        else if(!strcmp(command, "iniciar servidor")){
            printf("Digite o numero de requisicoes que o servidor deve atenter: \n");
            scanf("%i", &n);
            getchar();
            for(int i=0; i<n; i++) Server();
        }
        else if(!strcasecmp(command, "ajuda")){
            Menu();
        }
        else if(!strcmp(command, "sair")){
            break;
        }
        else{
            printf("comando invalido!\n");
            printf("use o comando ajuda para imprimir o os comandos disponiveis\n");
        }
         CommandSpaces();
    }while(1);
    printf("Encerrando Station ...\n");
    return 0;
}
