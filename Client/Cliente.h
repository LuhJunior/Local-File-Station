#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

#define BACKLOG_MAX 5
#define BUFFER_SIZE 1041
#define PORT 6000



void msg_err_exit(char *);

//POW BUGADONA VOU FAZER A MINHA
int MyPow(int, int);

void MyItoa(char *, int, int);

//TUDO BUGADO VOUI USAR NAO
int MyAtoi(char *, int);

//NOMES SUGESTIVOS

void PreencherBlocoRequisicao(char*, char*, char*, char, char, char*);

void ReadServerIP(char *);

void LerNomeArquivo(char *);

void IniciarVariaveisRequisicao(char *, char *, char *, int, int, char *, char *);

void LerIPsDoBloco(char *, char *, char *, char *);

void ReceberBlocoResposta(char *, char *, int, struct sockaddr_in *);
