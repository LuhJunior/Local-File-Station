#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <ctype.h>
#define BACKLOG_MAX 5
#define BUFFER_SIZE 1041
#define PORT 6000
#define true 1
#define false 0


typedef short int bool;

void msg_err_exit(char *);

//POW BUGADONA VOU FAZER A MINHA
int MyPow(int, int);

void MyItoa(char *, int, int);

int MyAtoi(char *, int);

void MYCOPY(char *, char *, int);

void Menu();

void CommandSpaces();

bool VerificarNome(char *, char **, int);

void AddServidorConsulta();

bool PegarServidorConsulta(char *);

bool CriarArquivo(char *, char **, int, int);

void Minusculo(char *);

//NOMES SUGESTIVOS

int AbrirCache(char *, char **);

void AddNoCache(char *, char *);

void PreencherBlocoRequisicao(char*, char*, char*, char, char, char*);

void ReadServerIP(char *);

void LerNomeArquivo(char *);

void IniciarVariaveisRequisicao(char *, char *, char *, int, int, char *, char *);

void LerIPsDoBloco(char *, char *, char *, char *);

bool ReceberBlocoResposta(char *, char *, int, struct sockaddr_in *);

bool BuscarArquivo();

bool BuscarArquivoNoSevidor(char *);
