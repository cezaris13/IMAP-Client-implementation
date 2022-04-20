#include "sockets.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
// test with old version if it works if spammed commands
int initializeClient(char host[], char port[]) {
  int socketId;
  struct addrinfo *servInfo, *currInfo, hints;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

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

  freeaddrinfo(servInfo);
  return socketId;
}

char *imap_recv(SSL *sslConnection, size_t size) {
  size_t cursor = 0;
  int rc;

  char *buffer = malloc((size) * sizeof(char));
  char *result = malloc((size) * sizeof(char));
  while ((rc = SSL_read(sslConnection, buffer, size))) {
    if (rc == -1)
      continue;

    buffer[rc] = '\0';
    int len = (sizeof(char) * (cursor++) * (size)) + rc;
    char *temp_str = buffer;
    char *temp_res = malloc(len);

    memcpy(temp_res, result, len);

    if (result != NULL) {
      strcat(temp_res, temp_str);
      memcpy(result, temp_res, strlen(temp_res) + 1);
    } else {
      memcpy(result, buffer, strlen(buffer) + 1);
    }
    free(temp_res);
    if (rc < size)
      break;
  }

  free(buffer);
  return result;
}

int check_ok(char *str) {
  int len = (int)strlen(str);
  int is_ok = 0;
  for (int i = 0; i < len; i++) {
    if (i + 4 > len)
      break;
    if (str[i] == 'O' && str[i + 1] == 'K') {
      is_ok = 1;
      break;
    };
  };
  return is_ok;
}

SSL *ConnectSSL(int socketId) {
  SSL_load_error_strings();
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_CTX *sslContext = SSL_CTX_new(SSLv23_client_method());
  if (sslContext == NULL) {
    printf("err\n");
  }
  SSL_CTX_set_options(sslContext, SSL_OP_SINGLE_DH_USE);

  SSL *sslConnection = SSL_new(sslContext);
  if (sslConnection == NULL) {
    printf("err\n");
  }
  SSL_set_fd(sslConnection, socketId);

  SSL_set_mode(sslConnection, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER |
                                  SSL_MODE_ENABLE_PARTIAL_WRITE);
  SSL_connect(sslConnection);
  return sslConnection;
}

void DestroySSL() {
  ERR_free_strings();
  EVP_cleanup();
}