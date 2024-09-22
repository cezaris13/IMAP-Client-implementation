#ifndef MY_SOCKETS_H
#define MY_SOCKETS_H
#include "sockets.cpp"
using namespace std;
int initializeClient(char host[], char port[]);
string imapRecv(SSL *sslConnection, size_t size);
int checkOK(string str);
SSL *connectSSL(int socketId);
void destroySSL();
#endif
