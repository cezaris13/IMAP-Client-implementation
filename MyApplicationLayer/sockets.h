#ifndef MY_SOCKETS_H
#define MY_SOCKETS_H
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
int initializeClient(char host[], char port[]);
char *imap_recv(SSL *sslConnection, size_t size);
int check_ok(char *str);
SSL *ConnectSSL(int socketId);
void DestroySSL();
#endif
