#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include <ctype.h>
#include "..\Tools.h"
#define BACKLOG_MAX 5
#define BUFFER_SIZE 1041
#define PORT 6000

typedef short int bool;

void msg_err_exit(char *);

void MenuCliente();

bool CriarArquivo(char *, char **, int, int);

//NOMES SUGESTIVOS

void PreencherBlocoRequisicao(char*, char*, char*, char, char, char*);

void LerNomeArquivo(char *);

void IniciarVariaveisRequisicao(char *, char *, char *, int, int, char *, char *);

void LerIPsDoBloco(char *, char *, char *, char *);

bool ReceberBlocoResposta(char *, char *, int, struct sockaddr_in *);

bool BuscarArquivo();

bool BuscarArquivoNoSevidor(char *);

#endif
