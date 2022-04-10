#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int initializeClient(char host[],char port[]){
  int socketId;
  struct addrinfo *servInfo, *currInfo, hints;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(host, port, &hints, &servInfo) != 0) {
    printf("getaddrinfo error\n");
    return -1;
  }
  for(currInfo = servInfo; currInfo != NULL; currInfo = currInfo->ai_next) {
    if((socketId = socket(currInfo->ai_family, currInfo->ai_socktype, currInfo->ai_protocol)) < 0){
      printf("could not create soctet\n");
      continue;
    }
    if(connect(socketId, currInfo->ai_addr, currInfo->ai_addrlen) < 0){
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

  char *buffer = (char *)malloc(size * sizeof(char));
  char *result = (char *)malloc(size * sizeof(char));
  while ((rc = (int)SSL_read(sslConnection, buffer, size))) {
    if (rc == -1)
      continue;

    buffer[rc] = '\0';
    int len = (int)(sizeof(char)*(cursor++)*size) + rc;
    char *temp_str = buffer;
    char *temp_res = (char *)malloc(len);
    memcpy(temp_res, result, len);

    if (result != NULL) {
      strcat(temp_res, temp_str);
      memcpy(result, temp_res, strlen(temp_res)+1);
    }
    else {
      memcpy(result, buffer, strlen(buffer)+1);
    }
    if (rc < size)
        break;
  }

  return result;
}

int check_ok(char* str) {
  int len = (int)strlen(str);
  int is_ok = 0;
  for (int i=0; i<len; i++) {
    if (i+4 > len) break;
    if (str[i] == 'O' && str[i+1] == 'K') {
      is_ok = 1;
      break;
    };
  };
  return is_ok;
}

SSL* ConnectSSL(int socketId){
  SSL_load_error_strings();
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_CTX *sslContext = SSL_CTX_new(SSLv23_client_method());
  if (sslContext == NULL){
    printf("err\n");
  }
  SSL_CTX_set_options(sslContext, SSL_OP_SINGLE_DH_USE);

  SSL *sslConnection = SSL_new(sslContext);
  if(sslConnection == NULL){
    printf("err\n");
  }
  SSL_set_fd(sslConnection, socketId);


  SSL_set_mode(sslConnection,SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER|SSL_MODE_ENABLE_PARTIAL_WRITE);
  SSL_connect(sslConnection);
  return sslConnection;
}

void DestroySSL(){
    ERR_free_strings();
    EVP_cleanup();
}

int main(){
  int fd = initializeClient("imap.gmail.com","993");
  if (fd < 0) {
    fprintf(stdout, "fd = %d, open file failed\n", fd);
    return 0;
  }
  else {
    fprintf(stderr, "fd = %d\n", fd);
  }
  SSL *sslConnection = ConnectSSL(fd);
  char *result=imap_recv(sslConnection, 100);

  printf("%s\n",result);
  char login[50];
  snprintf(login, 50, "A2 LOGIN \"kt.testimap2022@gmail.com\" \"Q!w2e3r4t5\"\n");

  char *buffers[6] = {
    "A1 CAPABILITY\n",
    login,
    "A3 CAPABILITY\n",
    "A4 ID (\"name\" \"inbox\" \"version\" \"1.0.0\" \"support-url\" \"http://yorkiefixer.me\")\n",
    "A5 SELECT \"INBOX\"\n",
    //"A6 FETCH *:* (UID ENVELOPE)\n"
    //"A7 LOGOUT\n"
  };

  int i = 0;
  do {
    printf("S: %s\n", result);
    if (i == 0) {
      printf("C: %s\n", buffers[i]);
      int a =SSL_write(sslConnection, buffers[i], strlen(buffers[i]));
      printf("%d\n",a);
      i++;
      continue;
    }

    if (check_ok(result)) {
      if (i >= 5) {
        break;
      }
      else {
        printf("C: %s\n", buffers[i]);
        int a =SSL_write(sslConnection, buffers[i], strlen(buffers[i]));
        printf("%d\n",a);
        i++;
      }
    }
  }
  while ((result = imap_recv(sslConnection, 100)) || 1);

  SSL_free(sslConnection);
  DestroySSL();
}
