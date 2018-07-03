/* SERVIDOR */
#include "Server.h"

void ServerMenu(){
    printf("-------------------------------------------------------------------------------\n");
    printf("-------------------------------------------------------------------------------\n\n");
    printf("comandos: \n");
    printf("  iniciar servidor - liga o servidor para atender um certo numero de requisicoes;\n");
    printf("  ajuda - para imprimir esse menu;\n");
    printf("  sair - para voltar ao menu anterior.\n\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("-------------------------------------------------------------------------------\n");
}

void preencherBlocoPositiva(char* block, FILE *f, char* ipServidor, char* ipCliente, char tipo,
                            int sequence, int tam){
    int padding = 0;
    MYCOPY(block, ipServidor, 4);
    MYCOPY(block+4, ipCliente, 4);
    block[8] = tipo;
    MyItoa(block+9, sequence, 2);
    MyItoa(block+11, tam, 4);
    fread(block+15, 1, 1024, f);
    padding = 1024*(sequence+1) - ftell(f);
    memset(block+(1038-padding), 0, padding);
    MyItoa(block+1039, padding, 2);
}

void preencherBlocoNegativa(char* block, char* ipServidor, char* ipCliente, char tipo){
    char IpNext[16];
    MYCOPY(block, ipServidor, 4);
    MYCOPY(block+4, ipCliente, 4);
    block[8] = tipo;
    if(!PegarServidorConsulta(IpNext)){
        printf("Servidor de consulta nao foi adicionado!\n");
        AddServidorConsulta();
        if(PegarServidorConsulta(IpNext)) strcpy(IpNext, "0.0.0.0");
    }
    ConverterIp(IpNext);
    MYCOPY(block+9, IpNext, 4);
}

void EnviarBlocos(char *NomeArquivo, char *ipS, char *ipC, int remote_socket){
    FILE *f = fopen(NomeArquivo, "rb");
    char block[BUFFER_SIZE];
    int tam = 0, sequence = 0;
    if(f != NULL){
        fseek(f, 0, SEEK_END);
        tam = ftell(f);
        rewind(f);
        while(!feof(f)){
            memset(block, 0, BUFFER_SIZE);
            preencherBlocoPositiva(block, f, ipS, ipC, 2, sequence++, tam);
            while(send(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR) printf("Erro ao enviar bloco de resposta\nTentando enviar novamente\n");
        }
        fclose(f);
        printf("Arquivo Enviado!\n");
    }
    else{
        memset(block, 0, BUFFER_SIZE);
        preencherBlocoNegativa(block, ipS, ipC, 3);
        while(send(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR) printf("Erro ao enviar bloco de resposta\nTentando enviar novamente\n");
    }
}

bool Server(){
    char nomeArquivo[30], ipServidor[10], ipCliente[10], tipo, vida, Arquivos[30][50];
    char block[BUFFER_SIZE];
    int local_socket = 0, remote_socket = 0, remote_length = 0, message_length = 0, tam = 0;
    unsigned short local_port = PORT;
    unsigned short remote_port = 0;
    struct sockaddr_in local_address;
    struct sockaddr_in remote_address;

    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0){
        msg_err_exit("WSAStartup() failed\n"); // inicia o Winsock 2.0 (DLL), Only for Windows
        return false;
    }
    local_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);// criando o socket local para o servidor
    if (local_socket == INVALID_SOCKET){
        WSACleanup();
        msg_err_exit("socket() failed\n");
        return false;
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
        return false;
    }

    // coloca o socket para escutar as conexoes
    if (listen(local_socket, BACKLOG_MAX) == SOCKET_ERROR){
        WSACleanup();
        closesocket(local_socket);
        msg_err_exit("listen() failed\n");
        return false;
    }

    remote_length = sizeof(remote_address);

    printf("Aguardando alguma conexao...\n");
    remote_socket = accept(local_socket, (struct sockaddr *) &remote_address, &remote_length);
    if(remote_socket == INVALID_SOCKET){
        WSACleanup();
        closesocket(local_socket);
        msg_err_exit("accept() failed\n");
        return false;
    }

    printf("Conexao estabelecida com %s\n", inet_ntoa(remote_address.sin_addr));
    printf("Aguardando requisicoes...\n");

    memset(block, 0, BUFFER_SIZE); // limpa o buffer

    // recebe a bloco do cliente
    while(recv(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR);

    MYCOPY(ipServidor, block, 4);
    MYCOPY(ipCliente, block+4, 4);
    tipo = block[8];
    vida = block[9];
    MYCOPY(nomeArquivo, block+10, 20);
    nomeArquivo[20] = '\0';
    if(tipo == 1){
        tam = AbrirCache("cache.txt", Arquivos);
        if(VerificarNome(nomeArquivo, Arquivos, tam)) EnviarBlocos(nomeArquivo, ipServidor, ipCliente, remote_socket);
        else{
            memset(block, 0, BUFFER_SIZE);
            preencherBlocoNegativa(block, ipServidor, ipCliente, 3);
            while(send(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR)printf("Erro ao enviar bloco de resposta negativa\nTentando enviar novamente\n");
        }
    }
    WSACleanup();
    closesocket(local_socket);
    closesocket(remote_socket);
    return true;
}

void Servidor(){
    char command[31];
    int n = 0;
    ServerMenu();
    CommandSpaces();
    do{
        fgets(command, 30, stdin);
        command[strlen(command)-1] = '\0';
        Minusculo(command);
        if(!strcmp(command, "iniciar servidor")){
            printf("Digite o numero de requisicoes que o servidor deve atenter: \n");
            scanf("%i", &n);
            getchar();
            for(int i=0; i<n; i++) Server();
        }
        else if(!strcmp(command, "sair")){
            break;
        }
        else if(!strcasecmp(command, "ajuda")){
            ServerMenu();
        }
        else{
            printf("comando invalido!\n");
            printf("use o comando ajuda para imprimir o os comandos disponiveis\n");
        }
         CommandSpaces();
    }while(1);
    printf("Encerrando servidor ...\n");
}
