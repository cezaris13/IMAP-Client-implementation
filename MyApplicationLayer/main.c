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

/* char *imap_recv(SSL *sslConnection, size_t size) { */
/*   size_t cursor = 0; */
/*   int rc; */

/*   char *buffer = (char *)malloc(size * sizeof(char)); */
/*   char *result = (char *)malloc(size * sizeof(char)); */
/*   int read_blocked = 0; */
/*   do{ */
/*     read_blocked = 0; */
/*     rc = (int)SSL_read(sslConnection, buffer, size); */
/*     printf("None\n"); */
/*     switch(SSL_get_error(sslConnection,rc)){ */
/*       case SSL_ERROR_NONE: */
/*         printf("None\n"); */
/*         if (rc == -1) */
/*           continue; */

/*         buffer[rc] = '\0'; */
/*         int len = (int)(sizeof(char)*(cursor++)*size) + rc; */
/*         char *temp_str = buffer; */
/*         char *temp_res = (char *)malloc(len); */
/*         memcpy(temp_res, result, len); */

/*         if (result != NULL) { */
/*           strcat(temp_res, temp_str); */
/*           memcpy(result, temp_res, strlen(temp_res)+1); */
/*         } */
/*         else { */
/*           memcpy(result, buffer, strlen(buffer)+1); */
/*         } */
/*         if (rc < size) */
/*           break; */
/*         break; */
/*       case SSL_ERROR_ZERO_RETURN: */
/*         printf("zero return\n"); */
/*         /\* End of data *\/ */
/*         /\* if(!shutdown_wait) *\/ */
/*         /\*   SSL_shutdown(ssl); *\/ */
/*         /\* goto end; *\/ */
/*         break; */
/*       case SSL_ERROR_WANT_READ: */
/*         printf("want read\n"); */
/*         read_blocked=1; */
/*         break; */
/*       case SSL_ERROR_WANT_WRITE: */
/*         printf("want write\n"); */
/*         /\* read_blocked_on_write=1; *\/ */
/*         break; */
/*       default: */
/*         printf("default\n"); */
/*         /\* berr_exit("SSL read problem"); *\/ */
/*         break; */
/*     } */
/*   } */
/*   while (SSL_pending(sslConnection) && !read_blocked); */

/*   return result; */
/* } */

char *imap_recv(SSL *sslConnection, size_t size) {
  size_t cursor = 0;
  int rc;

  char *buffer = malloc((size) * sizeof(char));
  char *result = malloc((size) * sizeof(char));
  while ((rc = SSL_read(sslConnection, buffer, size))) {
    if (rc == -1)
      continue;

    buffer[rc] = '\0';
    int len = (sizeof(char)*(cursor++)*(size)) + rc;
    char *temp_str = buffer;
    char *temp_res = malloc(len);

    memcpy(temp_res, result, len);

    if (result != NULL) {
      strcat(temp_res, temp_str);
      memcpy(result, temp_res, strlen(temp_res)+1);
    }
    else {
      memcpy(result, buffer, strlen(buffer)+1);
    }
    free(temp_res);
    if (rc < size)
        break;
  }

  free(buffer);
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

void SendAndReceiveImapMessage(char *command, SSL* sslConnection){
   printf("C: %s\n", command);
   SSL_write(sslConnection, command, strlen(command));
   char *result = imap_recv(sslConnection, 100);
   printf("S: %s\n", result);
   free(result);
}

void ShowImapCommands(SSL* sslConnection){
  int count = -1;
  int runProgram=1;
  int cursor = 1;
  while(runProgram){
    char message[100];
//should i process responses properly?
//
//noop?
//get count
//search
//select

//login +
//check response +
//create mailbox
//deletemailbox
//rename mailbox
//get mailboxes
//get unseen emails
//get top emails
//get mail
//move mail
//get all mails
//get mails
//delete mail
    snprintf(message, sizeof(message), "A%d ", cursor++);
    printf("select what to do:\n"
           "1. check connection status \n"
           "2. login \n"
           "3. check inbox \n"
           "4. get mail \n"
           "5. delete mail \n"
           "6. get mails \n"
           "7. get unseen mails \n"
           "8. get top mails \n"
           "9. get all mails \n"
           "10. move mail \n"
           "11. delete mail \n"
           "12. get mailboxes \n"
           "13. create mailbox \n"
           "14. rename mailbox \n"
           "15. delete mailbox \n"
           "16. logout \n"
           "17. close system \n");
    scanf("%d",&count);
    char mailbox[100];
    switch(count){
      case 1:
        strcat(message, "CAPABILITY\r\n");
        break;
      case 2:
        strcat(message,"LOGIN \"kt.testimap2022@gmail.com\" \"Q!w2e3r4t5\"\r\n");
        break;
      case 3:
        printf("enter mailbox\n");
        scanf("%s",mailbox);
        strcat(strcat(strcat(message,"SELECT \""),mailbox),"\"\r\n");
        break;
      case 4:
      //take inbox name and get mail
        break;
      case 5:
        break;
      case 6:
        break;
      case 7:
        printf("enter mailbox\n");
        scanf("%s",mailbox);
        strcat(strcat(strcat(message,"SELECT \""),mailbox),"\"\r\n");
        SendAndReceiveImapMessage(message,sslConnection);
        snprintf(message, sizeof(message), "A%d ", cursor++);
        strcat(message,"uid search unseen\r\n");
        runProgram=0;
        break;
      case 8:
        runProgram=0;
        break;
      case 9:
        runProgram=0;
        break;
      case 10:
        runProgram=0;
        break;
      case 11:
        runProgram=0;
        break;
      case 12:
        strcat(message,"LIST \"\" *\r\n");// see how to format this
        break;
      case 13:
        runProgram=0;
        break;
      case 14:
        runProgram=0;
        break;
      case 15:
        runProgram=0;
        break;
      case 16:
        strcat(message,"LOGOUT\r\n");
        break;
      default:
        runProgram=0;
        break;
    }
    if(runProgram){
        SendAndReceiveImapMessage(message,sslConnection);
    }
  }
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
  printf("S: %s\n",result);
  free(result);

  /* char *buffers[6] = { */
  /*   "A4 ID (\"name\" \"inbox\" \"version\" \"1.0.0\" \"support-url\" \"http://yorkiefixer.me\")\r\n", */
  /*   "A5 SELECT \"INBOX\"\r\n", */
  /*   //"A6 FETCH *:* (UID ENVELOPE)\r\n" */

  ShowImapCommands(sslConnection);
  /* int i = 0; */
  /* do { */
  /*   printf("S: %s\n", result); */
  /*   if (i == 0) { */
  /*     printf("C: %s\n", buffers[i]); */
  /*     SSL_write(sslConnection, buffers[i], strlen(buffers[i])); */
  /*     i++; */
  /*     continue; */
  /*   } */

  /*   if (check_ok(result)) { */
  /*     if (i >= 5) { */
  /*       break; */
  /*     } */
  /*     else { */
  /*       printf("C: %s\n", buffers[i]); */
  /*       SSL_write(sslConnection, buffers[i], strlen(buffers[i])); */
  /*       i++; */
  /*     } */
  /*   } */
  /* } */
  /* while ((result = imap_recv(sslConnection, 100)) || 1); */

  SSL_free(sslConnection);
  DestroySSL();
}
