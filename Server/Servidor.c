/* SERVIDOR */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#define BACKLOG_MAX 5
#define BUFFER_SIZE 1041
#define PORT 6000

/* Exibe uma mensagem de erro e termina o programa */
void msg_err_exit(char *msg)
{
    fprintf(stderr, msg);
    system("PAUSE");
    exit(EXIT_FAILURE);
}

void myItoa(char *s, int v){
    s[0] = v/1000000;
    v %= 1000000;
    s[1] = v/10000;
    v %= 10000;
    s[2] = v/100;
    v %= 100;
    s[3] = v;
}

int myAtoi(char *s){
    return (s[0]*1000000+s[1]*10000+s[2]*100+s[3]);
}

void preencherBlocoPositiva(char* block, FILE *f, char* ipServidor, char* ipCliente, char tipo,
                            int sequence, int tam){
    int padding = 0;
    for(int i=0; i<4; i++) block[i] = ipServidor[i];
    for(int i=0; i<4; i++) block[i+4] = ipCliente[i];
    block[8] = tipo;
    block[9] = sequence/100;
    block[10] = sequence%100;
    myItoa(block+11, tam);
    //printf("Tam: %i%i%i%i\n", *(block+11), *(block+12),*(block+13),*(block+14));
    fread(block+15, 1, 1024, f);
    padding = 1024*(sequence+1) - ftell(f);
    memset(block+(1038-padding), 0, padding);
    block[1039] = padding/100;
    block[1040] = padding%100;
}

void preencherBlocoNegativa(char* block, char* ipServidor, char* ipCliente, char tipo, char* prox){
    for(int i=0; i<4; i++) block[i] = ipServidor[i];
    for(int i=0; i<4; i++) block[i+4] = ipCliente[i];
    block[8] = tipo;
    for(int i=0; i<4; i++) block[i+9] = prox[i];
    block[13]='\0';
}

void EnviarBlocos(char *NomeArquivo, char *ipS, char *ipC, int remote_socket){
    FILE *f = fopen(NomeArquivo, "rb");
    char block[BUFFER_SIZE];
    int tam = 0;
    int sequence = 0;
    if(f != NULL){
        fseek(f, 0, SEEK_END);
        tam = ftell(f);
        rewind(f);
        while(!feof(f)){
            memset(block, 0, BUFFER_SIZE);
            preencherBlocoPositiva(block, f, ipS, ipC, '2', sequence++, tam);
            while(send(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR);
            /*if (send(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR){
                WSACleanup();
                closesocket(remote_socket);
                fclose(f);
                msg_err_exit("send() failed\n");
            }*/
        }
        fclose(f);
    }
    else{
        memset(block, 0, BUFFER_SIZE);
        preencherBlocoNegativa(block, ipS, ipC, '3', "127.0.0.1");
        while(send(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR);
    }
}

int main(int argc, char **argv){
    FILE *f;
    char nomeArquivo[21], ipServidor[5], ipCliente[5], tipo, vida;
    char block[BUFFER_SIZE];
    char ipProx[10][5];
    int sequence = 1, tam = 0;
    int local_socket = 0, remote_socket = 0, remote_length = 0, message_length = 0;
    unsigned short local_port = PORT;
    unsigned short remote_port = 0;
    struct sockaddr_in local_address;
    struct sockaddr_in remote_address;

    WSADATA wsa_data;
    do{
        sequence = 1;
        tam = 0;
        if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0) msg_err_exit("WSAStartup() failed\n"); // inicia o Winsock 2.0 (DLL), Only for Windows

        local_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);// criando o socket local para o servidor
        if (local_socket == INVALID_SOCKET){
            WSACleanup();
            msg_err_exit("socket() failed\n");
        }

        memset(&local_address, 0, sizeof(local_address)); // zera a estrutura local_address
        local_address.sin_family = AF_INET; // internet address family
        local_address.sin_port = htons(local_port);// porta local
        local_address.sin_addr.s_addr = htonl(INADDR_ANY);  // endereco // inet_addr("127.0.0.1")
        // interligando o socket com o endereço (local)
        if (bind(local_socket, (struct sockaddr *) &local_address, sizeof(local_address)) == SOCKET_ERROR){
            WSACleanup();
            closesocket(local_socket);
            msg_err_exit("bind() failed\n");
        }

        // coloca o socket para escutar as conexoes
        if (listen(local_socket, BACKLOG_MAX) == SOCKET_ERROR){
            WSACleanup();
            closesocket(local_socket);
            msg_err_exit("listen() failed\n");
        }

        remote_length = sizeof(remote_address);

        printf("Aguardando alguma conexao...\n");
        remote_socket = accept(local_socket, (struct sockaddr *) &remote_address, &remote_length);
        if(remote_socket == INVALID_SOCKET){
            WSACleanup();
            closesocket(local_socket);
            msg_err_exit("accept() failed\n");
        }

        printf("Conexao estabelecida com %s\n", inet_ntoa(remote_address.sin_addr));
        printf("Aguardando requisicoes...\n");
        memset(block, 0, BUFFER_SIZE); // limpa o buffer
        // recebe a bloco do cliente
        while(recv(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR);
        //message_length = recv(remote_socket, block, BUFFER_SIZE, 0);
        //if(message_length == SOCKET_ERROR) msg_err_exit("recv() failed\n");
        printf("%s: %s\n", inet_ntoa(remote_address.sin_addr), block);
        strncpy(ipServidor, block, 4);
        ipServidor[4]='\0';
        strncpy(ipCliente, block+4, 4);
        ipCliente[4]='\0';
        tipo = block[8];
        vida = block[9];
        strncpy(nomeArquivo, block+10, 20);
        printf("Servidor: %s\n", ipServidor);
        printf("Cliente: %s\n", ipCliente);
        printf("Tipo: %c\n", tipo);
        printf("Vida: %c\n", vida);
        printf("Nome do Arquivo: %s\n", nomeArquivo);
        if(tipo == '1') EnviarBlocos(nomeArquivo, ipServidor, ipCliente, remote_socket);
        WSACleanup();
        closesocket(local_socket);
        closesocket(remote_socket);
    }while(1);
    printf("Encerrando servidor\n");
    system("PAUSE");
    return 0;
}
