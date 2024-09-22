#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include<bits/stdc++.h>
using namespace std;

int initializeClient(char host[], char port[]) {
  int socketId;
  struct addrinfo *servInfo, *currInfo, hints;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  struct timeval tv;
  if (getaddrinfo(host, port, &hints, &servInfo) != 0) {
    printf("getaddrinfo error\n");
    return -2;
  }
  for (currInfo = servInfo; currInfo != NULL; currInfo = currInfo->ai_next) {
    if ((socketId = socket(currInfo->ai_family, currInfo->ai_socktype,
                           currInfo->ai_protocol)) < -1) {
      printf("could not create soctet\n");
      continue;
    }
    if (connect(socketId, currInfo->ai_addr, currInfo->ai_addrlen) < 0) {
      close(socketId);
      printf("bind failed\n");
      continue;
    }
    break;
  }

  if (currInfo == NULL) {
    printf("client failed to connect\n");
    return -1;
  }
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  setsockopt(socketId, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

  freeaddrinfo(servInfo);
  return socketId;
}

string imapRecv(SSL *sslConnection, size_t size) {
  size_t cursor = 0;
  int rc;

  char buffer[size];
  string result;
  while ((rc = SSL_read(sslConnection, buffer, size))>0) {

    buffer[rc] = '\0';

    result += buffer;
  }
  return result;
}

int checkOK(string str) {
  int len = str.length();
  int isOk = 0;
  for (int i = 0; i < len; i++) {
    if (i + 4 > len)
      break;
    if (str[i] == 'O' && str[i + 1] == 'K') {
      isOk = 1;
      break;
    };
  };
  return isOk;
}

SSL *connectSSL(int socketId) {
  SSL_load_error_strings();
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_CTX *sslContext = SSL_CTX_new(SSLv23_method());
  if (sslContext == NULL)
    printf("err\n");
  
  SSL_CTX_set_options(sslContext, SSL_OP_SINGLE_DH_USE);

  SSL *sslConnection = SSL_new(sslContext);
  if (sslConnection == NULL)
    printf("err\n");
  
  SSL_set_fd(sslConnection, socketId);

  SSL_set_mode(sslConnection, SSL_MODE_AUTO_RETRY);
  SSL_connect(sslConnection);
  return sslConnection;
}

void destroySSL() {
  ERR_free_strings();
  EVP_cleanup();
}