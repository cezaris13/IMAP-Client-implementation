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

char *imap_recv(int fd, size_t size) {
  size_t cursor = 0;
  int rc;

  char *buffer = (char *)malloc(size * sizeof(char));
  char *result = (char *)malloc(size * sizeof(char));

  while ((rc = (int)recv(fd, buffer, size, 0))) {
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
    };
    if (rc < size)
        break;
  };

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

int main(){
   initializeClient("imap.gmail.com","993");
}
