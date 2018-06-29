#include "Client.h"

void msg_err_exit(char *msg){
    WSACleanup();
    fprintf(stderr, msg);
}

void MYCOPY(char *s, char *s2, int tam){
    for(int i=0; i<tam; i++) s[i] = s2[i];
}

void CommandSpaces(){
    printf("\n\n\n\n\n\n\n> ");
}

void Minusculo(char *s){
    for(int i=0; i<strlen(s); i++) s[i] = tolower(s[i]);
}

//POW BUGADONA VOU FAZER A MINHA
int MyPow(int x, int y){
    int r = x;
    if(y == 0) return 1;
    for(int i=1; i<y; i++) r*=x;
    return r;
}

void Menu(){
    printf("-----------------------------------------------------------\n");
    printf("comandos: \n");
    printf("buscar - para buscar um arquivo\n");
    printf("adicionar servidor - para adicionar o ip de consulta\n");
    printf("adicionar cache - para adicionar um arquivo disponivel para tranferencia no cache\n");
    printf("sair - para voltar ao menu anterior\n");
    printf("ajuda - para imprimir esse menu\n");
}

int AbrirCache(char *cache, char **s){
    FILE *f;
    f = fopen(cache, "r");
    int i;
    if(f != NULL){
        for(i=0; !feof(f) && i<50; i++){
            fgets(s[i], 20, f);
            s[i][20]='\0';
        }
    }
    else{
        printf("Ocorreu um erro ao tentar abrir o cache.\n");
        return 0;
    }
    fclose(f);
    return i+1;
}

void AddNoCache(char *cache, char *s){
    FILE *f;
    f = fopen(cache, "a");
    if(f != NULL) fprintf(f, "%s", s);
    else printf("Ocorreu um erro ao tentar abrir o cache.\n");
    fclose(f);
}

bool VerificarNome(char *s, char **Arquivos, int tam){
    for(int i=0; i<tam; i++) if(!strcmp(s, Arquivos[i])) return true;
    return false;
}

void AddServidorConsulta(){
    FILE *f;
    char s[10];
    ReadServerIP(s);
    f = fopen("consulta.txt", "w");
    if(f != NULL) fprintf(f, "%s", s);
    else printf("Ocorreu um erro ao tentar abrir o arquivo\n");
    fclose(f);
}

bool PegarServidorConsulta(char *s){
    FILE *f;
    f = fopen("consulta.txt", "r");
    if(f != NULL) fgets(s, 20, f);
    else{
        printf("Ocorreu um erro ao tentar abrir o arquivo\n");
        return false;
    }
    fclose(f);
    return true;
}

bool CriarArquivo(char *Nome, char **BLOCO, int Tamanho, int Padding){
    FILE *f = fopen(Nome, "wb");
    for(int i=0; i<Tamanho; i++){
        if(i == Tamanho-1) fwrite(BLOCO[i], 1, Padding, f);
        else fwrite(BLOCO[i], 1, 1024, f);
    }
    fclose(f);
    return true;
}

void MyItoa(char *s, int v, int tam){
    for(int i=0, j=tam-1; i<tam; i++, j--){
        s[i] = v/MyPow(100, j);
        v %= MyPow(100, j);
    }
}

//TUDO BUGADO VOU USAR NAO
int MyAtoi(char *s, int tam){
    int r=0;
    for(int i=0, j=tam-1; i<tam; i++, j--) r = r + s[i] * MyPow(100, j);
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
    printf("%i\n%i\n", ipS, ipC);
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

bool ReceberBlocoResposta(char *block, char *NomeArquivo, int remote_socket, struct sockaddr_in *remote_address){
    char ipS[5], ipC[5], ipNext[5], tipo, vida = '4';
    char **BLOCO = NULL;
    int  sequence = 0, message_length = 0,  tam = 0, padding = 0;

    memset(block, 0, BUFFER_SIZE);
    while(recv(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR) printf("Erro ao receber o bloco %i\nTentando novamente\n", sequence);

    LerIPsDoBloco(block, ipS, ipC, &tipo);
    if(tipo == '2'){
        sequence = MyAtoi(block+9, 2);
        tam = MyAtoi(block+11, 4);
        padding = MyAtoi(block+1039, 2);
        BLOCO = (char **) malloc(sizeof(char *) * (tam/1024 + (tam%1024>0)));
        BLOCO[sequence] = (char *) malloc(1024);
        memset(BLOCO[sequence], 0, 1024);
        MYCOPY(BLOCO[sequence], block+15, 1024);
        for(int i = 1; i<tam/1024; i++){
            memset(block, 0, BUFFER_SIZE);
            while(recv(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR) printf("Erro ao receber o bloco %i\nTentando novamente\n", sequence);
            LerIPsDoBloco(block, ipS, ipC, &tipo);
            sequence = MyAtoi(block+9, 2);
            tam = MyAtoi(block+11, 4);
            padding = MyAtoi(block+1039, 2);
            BLOCO[sequence] = (char *) malloc(1024);
            memset(BLOCO[sequence], 0, 1024);
            MYCOPY(BLOCO[sequence], block+15, 1024);
        }
        if(tam%1024){
            memset(block, 0, BUFFER_SIZE);
            while(recv(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR) printf("Erro ao receber o bloco %i\n tentando novamente", sequence);
            LerIPsDoBloco(block, ipS, ipC, &tipo);
            sequence = MyAtoi(block+9, 2);
            tam = MyAtoi(block+11, 4);
            padding = MyAtoi(block+1039, 2);
            BLOCO[sequence] = (char *) malloc(1024);
            memset(BLOCO[sequence], 0, 1024);
            MYCOPY(BLOCO[sequence], block+15, padding);
        }
        printf("%i\n", sequence);
        if(CriarArquivo(NomeArquivo, BLOCO, sequence+1, padding)) printf("\nArquivo recebido com sucesso!\n\n");
        return true;
    }
    else if(tipo == '3'){
        printf("O Servidor nao possui o arquivo\n");
        closesocket(remote_socket);
        vida--;
        if(vida > '0'){
            MYCOPY(ipS, block, 4);
            MYCOPY(ipC, block+4, 4);
            tipo = block[8];
            MYCOPY(ipNext, block+9, 4);
            memset(remote_address, 0, sizeof(*remote_address));
            remote_address->sin_family = AF_INET;
            remote_address->sin_addr.s_addr = inet_addr(ipNext);
            remote_address->sin_port = htons(PORT);
            if (connect(remote_socket, (struct sockaddr *) &remote_address, sizeof(*remote_address)) == SOCKET_ERROR){
                msg_err_exit("Ocorreu um erro: connect() failed\nFinalizando ...\n");
                return false;
            }

            IniciarVariaveisRequisicao(block, ipS, ipC, remote_address->sin_addr.s_addr, inet_addr("127.0.0.1"), &tipo, &vida);

            PreencherBlocoRequisicao(block, ipS, ipC, tipo, vida, NomeArquivo);

            while(send(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR) printf("Erro ao enviar bloco de requisicao tentando novamente");

            return ReceberBlocoResposta(block, NomeArquivo, remote_socket, remote_address);
        }
        else {
            printf("Nenhum dos Servidores possui o arquivo ...\n");
            return false;
        }
    }
}

bool BuscarArquivoNoSevidor(char *NomeArquivo){
    char block[BUFFER_SIZE];
    char remote_ip[32], ipServidor[5], ipCliente[5], ipProx[5];
    char tipo, r, vida = '4';
    int remote_socket = 0, message_length = 0;
    unsigned short remote_port = PORT;
    struct sockaddr_in remote_address;
    WSADATA wsa_data;

    if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0) {
        msg_err_exit("Ocorreu um erro: WSAStartup() failed\nFinalizando ...\n");
        return false;
    }

    if(!PegarServidorConsulta(remote_ip)){
        printf("Servidor de consulta nao foi adicionado!\nAdicione e tente novamente");
        return false;
    }

    remote_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (remote_socket == INVALID_SOCKET) {
        msg_err_exit("Ocorreu um erro: socket() failed\nFinalizando ...\n");
        return false;
    }

    // preenchendo o remote_address (servidor)

    memset(&remote_address, 0, sizeof(remote_address));
    remote_address.sin_family = AF_INET;
    remote_address.sin_addr.s_addr = inet_addr(remote_ip);
    remote_address.sin_port = htons(remote_port);

    if (connect(remote_socket, (struct sockaddr *) &remote_address, sizeof(remote_address)) == SOCKET_ERROR){
        msg_err_exit("Ocorreu um erro: connect() failed\nFinalizando ...\n");
        return false;
    }
    IniciarVariaveisRequisicao(block, ipServidor, ipCliente, remote_address.sin_addr.s_addr, inet_addr("127.0.0.1"), &tipo, &vida);

    //preparando bloco de requisição de arquivo ao servidor

    PreencherBlocoRequisicao(block, ipServidor, ipCliente, tipo, vida, NomeArquivo);
    // envia a requisicao para o servidor
    while(send(remote_socket, block, BUFFER_SIZE, 0) == SOCKET_ERROR) printf("Erro ao enviar bloco de requisicao\nTentando enviar novamente\n");


    return ReceberBlocoResposta(block, NomeArquivo, remote_socket, &remote_address);
}

bool BuscarArquivo(){
    char Arquivos[50][20], NomeArquivo[22];
    printf("Digite o nome do arquivo que deseja buscar: ");
    fgets(NomeArquivo, 20, stdin);
    NomeArquivo[strlen(NomeArquivo)-1] = '\0';
    int tam = AbrirCache("cache.txt", (char **) Arquivos);
    if(VerificarNome(NomeArquivo, (char **) Arquivos, tam)){
        printf("Voce ja possui o arquivo em seu servidor\n");
        return true;
    }
    else{
        if(BuscarArquivoNoSevidor(NomeArquivo)){
            AddNoCache("cache.txt", NomeArquivo);
            printf("Arquivo encontrado e adicionado ao seu cache\n");
            return true;
        }
        else{
            printf("Arquivo nao encontrado\n");
            return false;
        }
    }
}
