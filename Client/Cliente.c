/*  CLIENTE */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <winsock.h>
#include "Cliente.h"

int main(int argc, char **argv){
    char block[BUFFER_SIZE], NomeArquivo[21];
    char remote_ip[32], ipServidor[5], ipCliente[5], ipProx[5];
    char tipo, r, vida = '4';
    int remote_socket = 0, message_length = 0;
    unsigned short remote_port = PORT;
    struct sockaddr_in remote_address;
    WSADATA wsa_data;

    do{
        if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0) msg_err_exit("Ocorreu um erro: WSAStartup() failed\nFinalizando ...\n");

        ReadServerIP(remote_ip);

        remote_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (remote_socket == INVALID_SOCKET) msg_err_exit("Ocorreu um erro: socket() failed\nFinalizando ...\n");

        // preenchendo o remote_address (servidor)

        memset(&remote_address, 0, sizeof(remote_address));
        remote_address.sin_family = AF_INET;
        remote_address.sin_addr.s_addr = inet_addr(remote_ip);
        remote_address.sin_port = htons(remote_port);

        if (connect(remote_socket, (struct sockaddr *) &remote_address, sizeof(remote_address)) == SOCKET_ERROR){
            msg_err_exit("Ocorreu um erro: connect() failed\nFinalizando ...\n");
        }

        LerNomeArquivo(NomeArquivo);

        IniciarVariaveisRequisicao(block, ipServidor, ipCliente, remote_address.sin_addr.s_addr, inet_addr("127.0.0.1"), &tipo, &vida);

        //preparando bloco de requisição de arquivo ao servidor

        PreencherBlocoRequisicao(block, ipServidor, ipCliente, tipo, vida, NomeArquivo);

        // envia a requisicao para o servidor
        if (send(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR){
            closesocket(remote_socket);
            msg_err_exit("Ocorreu um erro: send() failed\nFinalizando ...\n");
        }

        ReceberBlocoResposta(block, NomeArquivo, remote_socket, &remote_address);
        printf("Deseja procurar outro arquivo? {S/N}\n");
        do{
            r = getchar();
            if(r != 'S' && r != 'N') printf("Opcao invalida\nDeseja procurar outro arquivo? {S/N}\n");
        }while(r != 'S' && r != 'N');
        WSACleanup();
        closesocket(remote_socket);
    }while(r == 'S');
    printf("Encerrando ...\n");
    system("PAUSE");
    return 0;
}
