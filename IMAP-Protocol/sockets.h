#ifndef SOCKETS_H
#define SOCKETS_H
#include "sockets.c"

int initializeClient(char host[], char port[]);
char *imap_recv(SSL *sslConnection, size_t size);
int check_ok(char *str);
SSL *ConnectSSL(int socketId);
void DestroySSL();
#endif
