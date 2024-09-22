#ifndef MY_SOCKETS_H
#define MY_SOCKETS_H
#include "sockets.cpp"
using namespace std;
int initializeClient(char host[], char port[]);
string imap_recv(SSL *sslConnection, size_t size);
int check_ok(string str);
SSL *ConnectSSL(int socketId);
void DestroySSL();
#endif
