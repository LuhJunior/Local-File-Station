#include "Tools.h"

void msg_err_exit(char *msg){
    WSACleanup();
    fprintf(stderr, msg);
}

void inverte(char *s, int tam){
    char ax[tam+1];
    strncpy(ax, s, tam);
    for(int i=0; i<tam; i++) s[i] = ax[(tam-i)-1];
}

void MYCOPY(char *s, char *s2, int tam){
    for(int i=0; i<tam; i++) s[i] = s2[i];
}

void ConverterIp(char* IpNext){
    int ip = inet_addr(IpNext);
    MyItoa(IpNext, ip, 4);
}

void CommandSpaces(){
    printf("\n\n\n\n\n\n\n\n\n\n> ");
}

void Menu(){
    printf("-------------------------------------------------------------------------------\n");
    printf("-------------------------------------------------------------------------------\n\n");
    printf("comandos: \n");
    printf("  buscar - para buscar um arquivo;\n");
    printf("  servidor consulta - para adicionar o IP do servidor de consulta;\n");
    printf("  cache - para adicionar um arquivo disponivel para tranferencia no cache;\n");
    printf("  iniciar servidor - liga o servidor para atender um certo numero de requisicoes;\n");
    printf("  ajuda - para imprimir esse menu;\n");
    printf("  sair - para voltar ao menu anterior.\n\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("-------------------------------------------------------------------------------\n\n");
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

void CriarCache(){
    char r;
    FILE *f = fopen("cache.txt", "r");
    if(f == NULL) fclose(fopen("cache.txt", "w"));
    else{
        printf("Voce ja possui um cache criado deseja apaga-lo? (S/N)\n");
        do{
            r = getchar();
            r = toupper(r);
        }while(r != 'S' || r != 'N' );
        if(r == 'S') fclose(fopen("cache.txt", "w"));
    }
}

int AbrirCache(char *cache, char s[][50]){
    FILE *f;
    f = fopen(cache, "r");
    int i = 0;
    if(f != NULL){
        for(i=0; !feof(f) && i<50; i++){
            fgets(s[i], 20, f);
            s[i][strlen(s[i])-1] = '\0';
        }
    }
    else{
        printf("Ocorreu um erro ao tentar abrir o cache.\n");
        return 0;
    }
    fclose(f);
    return i;
}

void AddNoCache(char *cache, char *s){
    FILE *f;
    f = fopen(cache, "a");
    if(f != NULL) fprintf(f, "%s\n", s);
    else printf("Ocorreu um erro ao tentar abrir o cache.\n");
    fclose(f);
}

bool VerificarNome(char *s, char Arquivos[][50], int tam){
    for(int i=0; i<tam; i++) if(!strcmp(s, Arquivos[i])) return true;
    return false;
}

void ReadServerIP(char *ip){
    do{
        printf("IP do servidor: ");
        scanf("%s", ip);
        fflush(stdin);
        if(strlen(ip) > 15) printf("tamanho do ip invalido.\nUtilize no max 15 digitos\n");
    }while(strlen(ip) > 15);
}

void AddServidorConsulta(){
    FILE *f;
    char s[50];
    ReadServerIP(s);
    f = fopen("consulta.txt", "w");
    if(f != NULL) fprintf(f, "%s", s);
    else printf("Ocorreu um erro ao tentar abrir o arquivo\n");
    fclose(f);
}

bool PegarServidorConsulta(char *s){
    FILE *f;
    f = fopen("consulta.txt", "r");
    if(f != NULL) fgets(s, 16, f);
    else{
        printf("Ocorreu um erro ao tentar abrir o arquivo\n");
        return false;
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

void LerIPsDoBloco(char *block, char *ipS, char *ipC, char *tipo){
    MYCOPY(ipS, block, 4);
    ipS[4]='\0';
    MYCOPY(ipC, block+4, 4);
    ipC[4]='\0';
    *tipo = block[8];
}
