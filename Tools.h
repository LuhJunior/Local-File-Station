#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <winsock.h>
#define true 1
#define false 0

typedef short int bool;

void msg_err_exit(char *);

//POW BUGADONA VOU FAZER A MINHA
int MyPow(int, int);

void MyItoa(char *, int, int);

int MyAtoi(char *, int);

void MYCOPY(char *, char *, int);

void CommandSpaces();

void Menu();

void Minusculo(char *);

void ConverterIp(char *);

bool PegarServidorConsulta(char *);

void AddServidorConsulta();

bool VerificarNome(char *, char [][50], int);

void CriarCache();

int AbrirCache(char *, char [][50]);

void AddNoCache(char *, char *);

void ReadServerIP(char *);

void inverte(char *, int);

#endif // TOOLS_H
