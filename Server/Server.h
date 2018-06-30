#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
#include "..\Tools.h"
#define BUFFER_SIZE 1041
#define BACKLOG_MAX 5
#define BUFFER_SIZE 1041
#define PORT 6000

void ServerMenu();

void preencherBlocoPositiva(char*, FILE *, char*, char*, char, int, int);

void preencherBlocoNegativa(char*, char*, char*, char);

void EnviarBlocos(char *, char *, char *, int);

bool Server();

#endif
