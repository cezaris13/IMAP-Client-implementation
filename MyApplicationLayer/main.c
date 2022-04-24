#include "appLayer.h"
#include "sockets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// should i process responses properly?
#define MAX_EMAIL_ADDRESS_LENGTH 320
#define MAX_MAILBOX_NAME_SIZE 20

void GetMailByUID(SSL *sslConnection, int *cursor) {
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  char *uid = malloc(10 * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    free(mailBoxName);
    free(uid);
    return;
  }
  printf("Enter UID: ");
  scanf("%s", uid);
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);

  snprintf(message, sizeof(message),
           "A%d FETCH %s (UID BODY[HEADER.FIELDS (FROM TO SUBJECT DATE)])\r\n",
           (*cursor)++, uid);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Fetch failed\n");
  free(mailBoxName);
  free(uid);
}

void Search(SSL *sslConnection, int *cursor) { // TODO
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  char *from = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char)); // change sizes
  char *to = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  char *subject = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  char *text = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  char *nottext = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  char *since = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  char *before = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  size_t bufsize = 32;
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  getline(&from, &bufsize, stdin);
  printf("Enter from: ");
  getline(&from, &bufsize, stdin);
  printf("Enter to: ");
  getline(&to, &bufsize, stdin);
  printf("Enter subject: ");
  getline(&subject, &bufsize, stdin);
  printf("Enter text: ");
  getline(&text, &bufsize, stdin);
  printf("Enter nottext: ");
  getline(&nottext, &bufsize, stdin);
  printf("Enter since: ");
  getline(&since, &bufsize, stdin);
  printf("Enter before: ");
  getline(&before, &bufsize, stdin);
  from[strcspn(from, "\n")] = 0;
  to[strcspn(to, "\n")] = 0;
  subject[strcspn(subject, "\n")] = 0;
  text[strcspn(text, "\n")] = 0;
  nottext[strcspn(nottext, "\n")] = 0;
  since[strcspn(since, "\n")] = 0;
  before[strcspn(before, "\n")] = 0;
  snprintf(message, sizeof(message), "A%d SEARCH", (*cursor)++);
  printf ("%s\n", message);
  if (strlen(from) != 0) {
    strcat(message,strcat(" FROM \"",strcat(from,"\"")));
  }
  printf ("%s\n", message);
  if (strlen(to) != 0) {
    strcat(message,strcat(" TO \"",strcat(to,"\"")));
  }
  printf ("%s\n", message);
  if (strlen(subject) != 0) {
    strcat(message,strcat(" SUBJECT \"",strcat(subject,"\"")));
  }
  printf ("%s\n", message);
  if (strlen(text) != 0) {
    strcat(message,strcat(" TEXT \"",strcat(text,"\"")));
  }
  printf ("%s\n", message);
  if (strlen(nottext) != 0) {
    strcat(message,strcat(" NOTEXT \"",strcat(nottext,"\"")));
  }
  printf ("%s\n", message);
  if (strlen(since) != 0) {
    strcat(message,strcat(" SINCE \"",strcat(since,"\"")));
  }
  printf ("%s\n", message);
  if (strlen(before) != 0) {
    strcat(message,strcat(" BEFORE \"",strcat(before,"\"")));
  }
  printf ("%s\n", message);

  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  strcat(message,"\r\n");
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Search failed\n");
  free(mailBoxName);
  free(from);
  free(to);
  free(subject);
  free(text);
  free(nottext);
  free(since);
  free(before);
}

void ShowImapCommands(SSL *sslConnection) {
  int cursor = 1;
  int count = -1;
  int runProgram = 1;
  while (runProgram) {
    printf("select what to do:\n"
           "1. check connection status \n"
           "2. login \n"
           "3. select inbox \n"
           "4. get mail by uid \n"
           "5. get all mails \n"
           "6. move mail \n"
           "7. delete mail \n"
           "8. get mailboxes \n"
           "9. create mailbox \n"
           "10. rename mailbox \n"
           "11. delete mailbox \n"
           "12. get mailbox email count \n"
           "13. logout \n"
           "14. close system \n");
    scanf("%d", &count);
    switch (count) {
    case 1:
      CheckConnectionStatus(sslConnection, &cursor);
      break;
    case 2:
      LoginUser(sslConnection, &cursor);
      break;
    case 3:
      SelectMailboxByName(sslConnection, &cursor);
      break;
    case 4:
      GetMailByUID(sslConnection, &cursor);
      break;
    case 5:
      GetAllEmailsFromMailBox(sslConnection, &cursor);
      break;
    case 6:
      MoveEmailFromOneMailBoxToAnother(sslConnection, &cursor);
      break;
    case 7:
      DeleteEmailFromMailBox(sslConnection, &cursor);
      break;
    case 8:
      GetMailBoxes(sslConnection, &cursor);
      break;
    case 9:
      CreateMailBox(sslConnection, &cursor);
      break;
    case 10:
      RenameMailBox(sslConnection, &cursor);
      break;
    case 11:
      DeleteMailBox(sslConnection, &cursor);
      break;
    case 12:
      GetEmailCountForMailBox(sslConnection, &cursor);
      break;
    case 13:
      LogoutUser(sslConnection, &cursor);
      break;
    case 15:
      LoginUserHardcoded(sslConnection, &cursor);
      break;
    case 16:
      noop(sslConnection, &cursor);
      break;
    case 17:
      Search(sslConnection, &cursor);
    default:
      runProgram = 0;
      break;
    }
  }
}

int main() {
  int fd = initializeClient("imap.gmail.com", "993");
  if (fd < 0) {
    fprintf(stdout, "fd = %d, open file failed\n", fd);
    return 0;
  } else {
    fprintf(stderr, "fd = %d\n", fd);
  }
  SSL *sslConnection = ConnectSSL(fd);
  char *result = imap_recv(sslConnection, 100);
  printf("S: %s\n", result);
  free(result);

  ShowImapCommands(sslConnection);

  SSL_free(sslConnection);
  DestroySSL();
}