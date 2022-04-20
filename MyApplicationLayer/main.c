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

#define MAX_EMAIL_ADDRESS_LENGTH 320
#define MAX_MAILBOX_NAME_SIZE 20
int cursor=1;

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

int check_ok(char* str) {//impove this function
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

int SendAndReceiveImapMessage(char *command, SSL* sslConnection){
  printf("C: %s\n", command);
  SSL_write(sslConnection, command, strlen(command));
  char *response = imap_recv(sslConnection, 100);
    int is_ok = check_ok(response);
    printf("S: %s\n", response);
    free(response);
  if (is_ok) 
    return 0;
  else
    return -1;
}

void CheckConnectionStatus(SSL *sslConnection){
  char message[100];
  snprintf(message, sizeof(message), "A%d CAPABILITY\r\n", cursor++);
  if(SendAndReceiveImapMessage(message,sslConnection) == -1)
    printf("Connection lost\n");
}

void LoginUser(SSL *sslConnection){
  char *userName, *password;
  userName = malloc(MAX_EMAIL_ADDRESS_LENGTH*sizeof(char));
  password = malloc(MAX_EMAIL_ADDRESS_LENGTH*sizeof(char));
  char message[MAX_EMAIL_ADDRESS_LENGTH*2+100];
  printf("Enter your email address: ");
  scanf("%s", userName);
  printf("Enter your password: ");
  scanf("%s", password);
  snprintf(message, sizeof(message), "A%d LOGIN \"%s\" \"%s\"\r\n", cursor++, userName, password);
  if(SendAndReceiveImapMessage(message,sslConnection) == -1)
    printf("Login failed\n");
  free(userName);
  free(password);
}
  
void LoginUserHardcoded(SSL *sslConnection){
  char message[100];
  snprintf(message, sizeof(message), "A%d LOGIN \"%s\" \"%s\"\r\n", cursor++, "kt.testimap2022@gmail.com", "Q!w2e3r4t5");
  if(SendAndReceiveImapMessage(message,sslConnection) == -1)
    printf("Login failed\n");
}

// void CheckInboxByUID(SSL *sslConnection){
//   char message[100];
//   snprintf(message, sizeof(message), "A%d SELECT INBOX\r\n", cursor++);
//   if(SendAndReceiveImapMessage(message,sslConnection) == -1)
//     printf("Select failed\n");
//   snprintf(message, sizeof(message), "A%d UID SEARCH ALL\r\n", cursor++);
//   if(SendAndReceiveImapMessage(message,sslConnection) == -1)
//     printf("Search failed\n");
// }

// int UserIsLoggedIn(SSL *sslConnection){
//   char message[100];
//   snprintf(message, sizeof(message), "A%d CAPABILITY\r\n", cursor++);
//   if(SendAndReceiveImapMessage(message,sslConnection) == -1)
//     return 0;
//   char *response = imap_recv(sslConnection, 100);
//   int is_ok = check_ok(response);
//   free(response);
//   return is_ok;
// }

void LogoutUser(SSL *sslConnection){
  char message[100];
  snprintf(message, sizeof(message), "A%d LOGOUT\r\n", cursor++);
  if(SendAndReceiveImapMessage(message,sslConnection) == -1)
    printf("Logout failed\n");
}

// void SendEmail(SSL *sslConnection){
//   char *recipient, *subject, *message;
//   recipient = malloc(MAX_EMAIL_ADDRESS_LENGTH*sizeof(char));
//   subject = malloc(MAX_EMAIL_ADDRESS_LENGTH*sizeof(char));
//   message = malloc(MAX_EMAIL_ADDRESS_LENGTH*sizeof(char));
//   printf("Enter recipient: ");
//   scanf("%s", recipient);
//   printf("Enter subject: ");
//   scanf("%s", subject);
//   printf("Enter message: ");
//   scanf("%s", message);
//   char message_to_send[MAX_EMAIL_ADDRESS_LENGTH*2+100];
//   snprintf(message_to_send, sizeof(message_to_send), "A%d MAIL FROM: <%s>\r\n", cursor++, "
//   snprintf(message_to_send, sizeof(message_to_send), "A%d RCPT TO: <%s>\r\n", cursor++, recipient);
//   snprintf(message_to_send, sizeof(message_to_send), "A%d DATA\r\n", cursor++);
//   snprintf(message_to_send, sizeof(message_to_send), "A%d FROM: <%s>\r\n", cursor++, "
//   snprintf(message_to_send, sizeof(message_to_send), "A%d SUBJECT: %s\r\n", cursor++, subject);
//   snprintf(message_to_send, sizeof(message_to_send), "A%d %s\r\n", cursor++, message);
//   snprintf(message_to_send, sizeof(message_to_send), "A%d .\r\n", cursor++);
//   if(SendAndReceiveImapMessage(message_to_send,sslConnection) == -1)
//     printf("Send failed\n");
//   free(recipient);
//   free(subject);
//   free(message);
// }

void SelectMailboxByName(SSL *sslConnection){
  //check if logged in  
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE*sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  snprintf(message, sizeof(message), "A%d SELECT \"%s\"\r\n", cursor++, mailBoxName);
  if(SendAndReceiveImapMessage(message,sslConnection) == -1)
    printf("Select failed\n");
  free(mailBoxName);
}

void GetMailBoxes(SSL *sslConnection){
  char message[100];
  snprintf(message, sizeof(message), "A%d LIST \"\" \"*\"\r\n", cursor++);
  if(SendAndReceiveImapMessage(message,sslConnection) == -1)
    printf("List failed\n");
}

void CreateMailBox(SSL *sslConnection){
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE*sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if(strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE){
    printf("Mailbox name is too long\n");
    return;
  }
  snprintf(message, sizeof(message), "A%d CREATE \"%s\"\r\n", cursor++, mailBoxName);
  if(SendAndReceiveImapMessage(message,sslConnection) == -1)
    printf("Create failed\n");
  free(mailBoxName);
}

void DeleteMailBox (SSL *sslConnection){
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE*sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if(strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE){
    printf("Mailbox name is too long\n");
    return;
  }
  snprintf(message, sizeof(message), "A%d DELETE \"%s\"\r\n", cursor++, mailBoxName);
  if(SendAndReceiveImapMessage(message,sslConnection) == -1)
    printf("Delete failed\n");
  free(mailBoxName);
}

void RenameMailBox (SSL *sslConnection){
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE*sizeof(char));
  char *newMailBoxName = malloc(MAX_MAILBOX_NAME_SIZE*sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if(strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE){
    printf("Mailbox name is too long\n");
    return;
  }
  printf("Enter new mailbox name: ");
  scanf("%s", newMailBoxName);
  if(strlen(newMailBoxName) > MAX_MAILBOX_NAME_SIZE){
    printf("Mailbox name is too long\n");
    return;
  }
  snprintf(message, sizeof(message), "A%d RENAME \"%s\" \"%s\"\r\n", cursor++, mailBoxName, newMailBoxName);
  if(SendAndReceiveImapMessage(message,sslConnection) == -1)
    printf("Rename failed\n");
  free(mailBoxName);
  free(newMailBoxName);
}

void noop (SSL *sslConnection){
  char message[100];
  snprintf(message, sizeof(message), "A%d NOOP\r\n", cursor++);
  if(SendAndReceiveImapMessage(message,sslConnection) == -1)
    printf("Noop failed\n");
}

void GetEmailCountForMailBox(SSL *sslConnection){
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE*sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if(strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE){
    printf("Mailbox name is too long\n");
    return;
  }
  snprintf(message, sizeof(message), "A%d STATUS \"%s\" (MESSAGES)\r\n", cursor++, mailBoxName);
  if(SendAndReceiveImapMessage(message,sslConnection) == -1)
    printf("Status failed\n");
  free(mailBoxName);
}

void ShowImapCommands(SSL* sslConnection){
  int count = -1;
  int runProgram=1;
  while(runProgram){
    char message[100];
//should i process responses properly?
//
//noop?+
//get count+
//search
//select

//login +
//check response +
//create mailbox+
//deletemailbox+
//rename mailbox+
//get mailboxes+
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
           "16. get mailbox count \n"
           "17. logout \n"
           "18. close system \n");
    scanf("%d",&count);
    char mailbox[100];
    switch(count){
      case 1:
        CheckConnectionStatus(sslConnection);
        break;
      case 2:
        LoginUser(sslConnection);
        break;
      case 3:
        SelectMailboxByName(sslConnection);
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
        GetMailBoxes(sslConnection);
        break;
      case 13:
        CreateMailBox(sslConnection);
        break;
      case 14:
        RenameMailBox(sslConnection);
        break;
      case 15:
        DeleteMailBox(sslConnection);
        break;
      case 16:
        GetEmailCountForMailBox(sslConnection);
        break;
      case 17:
        LogoutUser(sslConnection);
        break;
      case 19:
        LoginUserHardcoded(sslConnection);
        break;
      case 20:
        noop(sslConnection);//what is this?
        break;
      default:
        runProgram=0;
        break;
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
  /*   //"A6 FETCH *:* (UID ENVELOPE)\r\n" */

  ShowImapCommands(sslConnection);

  SSL_free(sslConnection);
  DestroySSL();
}