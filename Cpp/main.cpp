#include "iostream"
#include <cstdio>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <windows.h>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")

#define BUFSIZE 1024
char buf[BUFSIZE];
char *msg;
WSADATA wsda;
int     sock;
struct hostent *host;
struct sockaddr_in server_addr;
short int       s_port = 993;
const char      *s_ipaddr = "74.125.77.109";
 int SSL_library_init();
 int main () {
    SSL_library_init();
    //RAND_seed(buf, BUFSIZE);




    WSAStartup(MAKEWORD(2,2), &wsda);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(s_port);
    server_addr.sin_addr.s_addr = inet_addr(s_ipaddr);

    if(server_addr.sin_addr.s_addr == INADDR_NONE)
    {
        host = NULL;
        host = gethostbyname(s_ipaddr);
        if(host == NULL)
        {
            return false;
        }
        memcpy(&server_addr.sin_addr, host->h_addr_list[0], host->h_length);
    }

    connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));

    cout << "Logging-in:\n";
    msg = "a1 login emailapp123@gmail.com PASS\n";

    cout << buf << "\n";

    cout << "INBOX\n";

    msg = "a2 SELECT INBOX\n";
    SSL_write(sslConnection, msg, strlen(msg));
    SSL_read(sslConnection, buf, sizeof(buf)-1);
    cout << buf << "\n";
    system("pause");
 }
