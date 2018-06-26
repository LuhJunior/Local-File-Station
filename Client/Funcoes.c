#include "Cliente.h"

void msg_err_exit(char *msg){
    WSACleanup();
    fprintf(stderr, msg);
    system("PAUSE");
    exit(EXIT_FAILURE);
}

//POW BUGADONA VOU FAZER A MINHA
int MyPow(int x, int y){
    if(y == 0) return 1;
    for(int i=1; i<y; i++) x*=x;
    return x;
}


void MyItoa(char *s, int v, int tam){
    for(int i=0, j=tam-1; i<tam; i++, j--){
        s[i] = v/MyPow(100, j);
        v = v/100;
    }
}

//TUDO BUGADO VOUI USAR NAO
int MyAtoi(char *s, int tam){
    int r=0;
    for(int i=0, j=tam-1; i<tam; i++, j--) r = r + s[i] * MyPow(100, j);
    if(s[0] == 49 && s[1] > 90 ) printf("%i", r);
    return r;
}

void PreencherBlocoRequisicao(char* block, char* ipServidor, char* ipCliente, char tipo, char vida, char* nomeArquivo){
    for(int i=0; i<4; i++) block[i] = ipServidor[i];
    for(int i=0; i<4; i++) block[i+4] = ipCliente[i];
    block[8] = tipo;
    block[9] = vida;
    for(int i=0; i<20; i++) block[i+10] = nomeArquivo[i];
    block[30]='\0';
}

void ReadServerIP(char *ip){
    do{
        printf("IP do servidor: ");
        scanf("%s", ip);
        fflush(stdin);
        if(strlen(ip) > 8) printf("tamanho do ip invalido.\nutilize no max 8 digitos");
    }while(strlen(ip) > 8);
}

void LerNomeArquivo(char *s){
    printf("Digite o nome do arquivo que deseja buscar\n");
    printf("Nome do arquivo: ");
    fgets(s, 20, stdin);
    s[strlen(s)-1] = '\0';
}

void IniciarVariaveisRequisicao(char *block, char *ipServidor, char *ipCliente, int ipS, int ipC, char *tipo, char *vida){
    memset(block, 0, BUFFER_SIZE);// limpa o buffer
    MyItoa(ipServidor, ipS, 4);
    MyItoa(ipCliente, ipC, 4);
    *tipo = '1';
    *vida = '4';
}

void LerIPsDoBloco(char *block, char *ipS, char *ipC, char *tipo){
    strncpy(ipS, block, 4);
    ipS[4]='\0';
    strncpy(ipC, block+4, 4);
    ipC[4]='\0';
    *tipo = block[8];
}

void ReceberBlocoResposta(char *block, char *NomeArquivo, int remote_socket, struct sockaddr_in *remote_address){
    FILE *f;
    char ipS[5], ipC[5], ipNext[5],tipo, vida = '4';
    int  sequence = 0, message_length = 0,  tam = 0, padding = 0;

    memset(block, 0, BUFFER_SIZE);

    message_length = recv(remote_socket, block, BUFFER_SIZE, 0);
    if(message_length == SOCKET_ERROR) msg_err_exit("Ocorreu um erro: recv() failed\nFinalizando ...\n");

    LerIPsDoBloco(block, ipS, ipC, &tipo);

    if(tipo == '2'){
        sequence = block[9]*100 +  block[10];
        tam = block[11]*1000000 + block[12]*10000 + block[13]*100 + block[14];
        padding = block[1039]*100 + block[1040];
        f = fopen(NomeArquivo, "wb");
        if(!f){
            printf("Ocorreu um erro ao tentar criar o arquivo");
            return;
        }
        fwrite(block+15, 1, 1024, f);

        for(int i = 1; i<tam/1024; i++){
            memset(block, 0, BUFFER_SIZE);
            message_length = recv(remote_socket, block, BUFFER_SIZE, 0);
            if(message_length == SOCKET_ERROR) msg_err_exit("Ocorreu um erro: recv() failed\nFinalizando ...\n");
            LerIPsDoBloco(block, ipS, ipC, &tipo);
            sequence = block[9]*100 +  block[10];
            tam = block[11]*1000000 + block[12]*10000 + block[13]*100 + block[14];
            padding = block[1039]*100 + block[1040];
            fwrite(block+15, 1, 1024, f);
        }
        if(tam%1024){
            memset(block, 0, BUFFER_SIZE);
            message_length = recv(remote_socket, block, BUFFER_SIZE, 0);
            if(message_length == SOCKET_ERROR) msg_err_exit("Ocorreu um erro: recv() failed\nFinalizando ...\n");
            LerIPsDoBloco(block, ipS, ipC, &tipo);
            sequence = block[9]*100 +  block[10];
            tam = block[11]*1000000 + block[12]*10000 + block[13]*100 + block[14];
            padding = block[1039]*100 + block[1040];
            fwrite(block+15, 1, padding, f);
        }
        fclose(f);
        printf("\nArquivo recebido com sucesso!\n\n");
    }
    else{
        printf("O Servidor nao possui o arquivo");
        closesocket(remote_socket);
        return;// Funcao incompleta
        for(;vida > '0'; vida--){
            strncpy(ipNext, block+8, 4);
            vida--;

            memset(remote_address, 0, sizeof(*remote_address));
            remote_address->sin_family = AF_INET;
            remote_address->sin_addr.s_addr = inet_addr(ipNext);
            remote_address->sin_port = htons(PORT);

            if (connect(remote_socket, (struct sockaddr *) &remote_address, sizeof(*remote_address)) == SOCKET_ERROR){
                msg_err_exit("Ocorreu um erro: connect() failed\nFinalizando ...\n");
            }

            IniciarVariaveisRequisicao(block, ipS, ipC, remote_address->sin_addr.s_addr, inet_addr("127.0.0.1"), &tipo, &vida);

            PreencherBlocoRequisicao(block, ipS, ipC, tipo, vida, NomeArquivo);
            if (send(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR){
                closesocket(remote_socket);
                msg_err_exit("Ocorreu um erro: send() failed\n");
            }

            memset(&block, 0, BUFFER_SIZE);
            message_length = recv(remote_socket, block, BUFFER_SIZE, 0);
            if(message_length == SOCKET_ERROR) msg_err_exit("Ocorreu um erro: recv() failed\nFinalizando ...\n");

            LerIPsDoBloco(block, ipS, ipC, &tipo);

            if(tipo == '2'){
                sequence = block[9]*100 +  block[10];
                tam = block[11]*1000000 + block[12]*10000 + block[13]*100 + block[14];
                padding = block[1039]*100 + block[1040];

                f = fopen(NomeArquivo, "wb");
                if(!f){
                    printf("Ocorreu um erro ao tentar criar o arquivo");
                    return;
                }
                fwrite(block+15, 1, 1024, f);
                for(int i = 1; i<tam/1024 || padding ; i++){
                    memset(block, 0, BUFFER_SIZE);
                    message_length = recv(remote_socket, block, BUFFER_SIZE, 0);
                    if(message_length == SOCKET_ERROR) msg_err_exit("Ocorreu um erro: recv() failed\nFinalizando ...\n");
                    LerIPsDoBloco(block, ipS, ipC, &tipo);
                    sequence = block[9]*100 +  block[10];
                    tam = block[11]*1000000 + block[12]*10000 + block[13]*100 + block[14];
                    padding = block[1039]*100 + block[1040];
                    fwrite(block+15, 1, 1024, f);
                }
                if(tam%1024){
                    memset(block, 0, BUFFER_SIZE);
                    message_length = recv(remote_socket, block, BUFFER_SIZE, 0);
                    if(message_length == SOCKET_ERROR) msg_err_exit("Ocorreu um erro: recv() failed\nFinalizando ...\n");
                    LerIPsDoBloco(block, ipS, ipC, &tipo);
                    sequence = block[9]*100 +  block[10];
                    tam = block[11]*1000000 + block[12]*10000 + block[13]*100 + block[14];
                    padding = block[1039]*100 + block[1040];
                    fwrite(block+15, 1, padding, f);
                }
                fclose(f);
            }
            else{
                closesocket(remote_socket);
                printf("O Servidor nao possui o arquivo\n");
            }
        }
        printf("Nenhum dos Servidores possui o arquivo ...\n");
    }
}
