#include "appLayer.h"
#include "sockets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// should i process responses properly?
// search
#define MAX_EMAIL_ADDRESS_LENGTH 320
#define MAX_MAILBOX_NAME_SIZE 20

void GetEmailCountForMailBox(SSL *sslConnection, int *cursor) {
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  snprintf(message, sizeof(message), "A%d STATUS \"%s\" (MESSAGES)\r\n",
           (*cursor)++, mailBoxName);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Status failed\n");
  // printf("Number of emails in mailbox: %d\n", numberOfEmails);//detailed info
  free(mailBoxName);
}

void GetUnseenMailsFromMailBox(SSL *sslConnection, int *cursor) { // TODO
  // printf("enter mailbox\n");
  // scanf("%s",mailbox);
  // strcat(strcat(strcat(message,"SELECT \""),mailbox),"\"\r\n");
  // SendAndReceiveImapMessage(message,sslConnection,&cursor);
  // snprintf(message, sizeof(message), "A%d ", (*cursor)++);
  // strcat(message,"uid search unseen\r\n");
  // runProgram=0;
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  snprintf(message, sizeof(message), "A%d SEARCH UNSEEN\r\n", (*cursor)++);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Search failed\n");
  free(mailBoxName);
}

void GetTopMailsFromMailBox(SSL *sslConnection, int *cursor) { // TODO
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  snprintf(message, sizeof(message),
           "A%d FETCH 1:* (UID BODY[HEADER.FIELDS (FROM TO SUBJECT DATE)])\r\n",
           (*cursor)++);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Fetch failed\n");
  free(mailBoxName);
}

void MoveEmailFromOneMailBoxToAnother(SSL *sslConnection, int *cursor) { // TODO
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  char *newMailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  printf("Enter new mailbox name: ");
  scanf("%s", newMailBoxName);
  if (strlen(newMailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  snprintf(message, sizeof(message), "A%d COPY 1:* \"%s\"\r\n", (*cursor)++,
           newMailBoxName);
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1)
    printf("Copy failed\n");
  snprintf(message, sizeof(message),
           "A%d STORE 1:* +FLAGS.SILENT (\\Deleted)\r\n", (*cursor)++);
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1)
    printf("Store failed\n");
  snprintf(message, sizeof(message), "A%d EXPUNGE\r\n", (*cursor)++);
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1)
    printf("Expunge failed\n");
  free(mailBoxName);
  free(newMailBoxName);
}

void GetAllEmailsFromMailBox(SSL *sslConnection, int *cursor) { // TODO
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  snprintf(message, sizeof(message),
           "A%d FETCH 1:* (UID BODY[HEADER.FIELDS (FROM TO SUBJECT DATE)])\r\n",
           (*cursor)++);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Fetch failed\n");
  free(mailBoxName);
}

void DeleteEmailFromMailBox(SSL *sslConnection, int *cursor) { // TODO
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  snprintf(message, sizeof(message),
           "A%d STORE 1:* +FLAGS.SILENT (\\Deleted)\r\n", (*cursor)++);
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1)
    printf("Store failed\n");
  snprintf(message, sizeof(message), "A%d EXPUNGE\r\n", (*cursor)++);
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1)
    printf("Expunge failed\n");
  free(mailBoxName);
}

void GetMailByUID(SSL *sslConnection, int *cursor) { // retrieve data needed
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  char *uid = malloc(10 * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
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

// void CheckInboxByUID(SSL *sslConnection,int *cursor){
//   char message[100];
//   snprintf(message, sizeof(message), "A%d SELECT INBOX\r\n", (*cursor)++);
//   if(SendAndReceiveImapMessage(message,sslConnection,&cursor) == -1)
//     printf("Select failed\n");
//   snprintf(message, sizeof(message), "A%d UID SEARCH ALL\r\n", (*cursor)++);
//   if(SendAndReceiveImapMessage(message,sslConnection,&cursor) == -1)
//     printf("Search failed\n");
// }

// void SendEmail(SSL *sslConnection,int *cursor){
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
//   snprintf(message_to_send, sizeof(message_to_send), "A%d MAIL FROM:
//   <%s>\r\n", (*cursor)++, " snprintf(message_to_send,
//   sizeof(message_to_send), "A%d RCPT TO: <%s>\r\n", (*cursor)++, recipient);
//   snprintf(message_to_send, sizeof(message_to_send), "A%d DATA\r\n",
//   (*cursor)++); snprintf(message_to_send, sizeof(message_to_send), "A%d FROM:
//   <%s>\r\n", (*cursor)++, " snprintf(message_to_send,
//   sizeof(message_to_send), "A%d SUBJECT: %s\r\n", (*cursor)++, subject);
//   snprintf(message_to_send, sizeof(message_to_send), "A%d %s\r\n",
//   (*cursor)++, message); snprintf(message_to_send, sizeof(message_to_send),
//   "A%d .\r\n", (*cursor)++);
//   if(SendAndReceiveImapMessage(message_to_send,sslConnection,&cursor) == -1)
//     printf("Send failed\n");
//   free(recipient);
//   free(subject);
//   free(message);
// }

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
           "5. get unseen mails \n"
           "6. get top mails \n"
           "7. get all mails \n"
           "8. move mail \n"
           "9. delete mail \n"
           "10. get mailboxes \n"
           "11. create mailbox \n"
           "12. rename mailbox \n"
           "13. delete mailbox \n"
           "14. get mailbox email count \n"
           "15. logout \n"
           "16. close system \n");
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
      GetUnseenMailsFromMailBox(sslConnection, &cursor);
      break;
    case 6:
      GetTopMailsFromMailBox(sslConnection, &cursor);
      break;
    case 7:
      GetAllEmailsFromMailBox(sslConnection, &cursor);
      break;
    case 8:
      MoveEmailFromOneMailBoxToAnother(sslConnection, &cursor);
      break;
    case 9:
      DeleteEmailFromMailBox(sslConnection, &cursor);
      break;
    case 10:
      GetMailBoxes(sslConnection, &cursor);
      break;
    case 11:
      CreateMailBox(sslConnection, &cursor);
      break;
    case 12:
      RenameMailBox(sslConnection, &cursor);
      break;
    case 13:
      DeleteMailBox(sslConnection, &cursor);
      break;
    case 14:
      GetEmailCountForMailBox(sslConnection, &cursor);
      break;
    case 15:
      LogoutUser(sslConnection, &cursor);
      break;
    case 17:
      LoginUserHardcoded(sslConnection, &cursor);
      break;
    case 18:
      noop(sslConnection, &cursor); // what is this?
      break;
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

  /* char *buffers[6] = { */
  /*   "A4 ID (\"name\" \"inbox\" \"version\" \"1.0.0\" \"support-url\"
   * \"http://yorkiefixer.me\")\r\n", */
  /*   //"A6 FETCH *:* (UID ENVELOPE)\r\n" */

  ShowImapCommands(sslConnection);

  SSL_free(sslConnection);
  DestroySSL();
}