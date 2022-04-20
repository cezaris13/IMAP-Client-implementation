#include "appLayer.h"
#include "sockets.h"
#include <string.h>

int SendAndReceiveImapMessage(char *command, SSL *sslConnection, int silent) {
  if (silent == 0)
    printf("C: %s\n", command);
  SSL_write(sslConnection, command, strlen(command));
  char *response = imap_recv(sslConnection, 100);
  int is_ok = check_ok(response);
  if (silent == 0)
    printf("S: %s\n", response);
  free(response);
  if (is_ok)
    return 0;
  else
    return -1;
}

void CheckConnectionStatus(SSL *sslConnection, int *cursor) {
  char message[100];
  snprintf(message, sizeof(message), "A%d CAPABILITY\r\n", (*cursor)++);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Connection lost\n");
}

void LoginUser(SSL *sslConnection, int *cursor) {
  char *userName, *password;
  userName = malloc(MAX_EMAIL_ADDRESS_LENGTH * sizeof(char));
  password = malloc(MAX_EMAIL_ADDRESS_LENGTH * sizeof(char));
  char message[MAX_EMAIL_ADDRESS_LENGTH * 2 + 100];
  printf("Enter your email address: ");
  scanf("%s", userName);
  printf("Enter your password: ");
  scanf("%s", password);
  snprintf(message, sizeof(message), "A%d LOGIN \"%s\" \"%s\"\r\n", (*cursor)++,
           userName, password);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Login failed\n");
  free(userName);
  free(password);
}

void LoginUserHardcoded(SSL *sslConnection, int *cursor) {
  char message[100];
  snprintf(message, sizeof(message), "A%d LOGIN \"%s\" \"%s\"\r\n", (*cursor)++,
           "kt.testimap2022@gmail.com", "Q!w2e3r4t5");
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Login failed\n");
}

void LogoutUser(SSL *sslConnection, int *cursor) {
  char message[100];
  snprintf(message, sizeof(message), "A%d LOGOUT\r\n", (*cursor)++);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Logout failed\n");
}

void SelectMailboxByNameProvided(SSL *sslConnection, int *cursor,
                                 char *mailBoxName) {
  char message[100];
  snprintf(message, sizeof(message), "A%d SELECT \"%s\"\r\n", (*cursor)++,
           mailBoxName);
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1)
    printf("Select failed\n");
}

void SelectMailboxByName(SSL *sslConnection, int *cursor) {
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE)
    printf("Mailbox name is too long\n");
  snprintf(message, sizeof(message), "A%d SELECT \"%s\"\r\n", (*cursor)++,
           mailBoxName);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Select failed\n");
  free(mailBoxName);
}

void GetMailBoxes(SSL *sslConnection, int *cursor) {
  char message[100];
  snprintf(message, sizeof(message), "A%d LIST \"\" \"*\"\r\n", (*cursor)++);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("List failed\n");
}

void CreateMailBox(SSL *sslConnection, int *cursor) {
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  snprintf(message, sizeof(message), "A%d CREATE \"%s\"\r\n", (*cursor)++,
           mailBoxName);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Create failed\n");
  free(mailBoxName);
}

void DeleteMailBox(SSL *sslConnection, int *cursor) {
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  snprintf(message, sizeof(message), "A%d DELETE \"%s\"\r\n", (*cursor)++,
           mailBoxName);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Delete failed\n");
  free(mailBoxName);
}

void RenameMailBox(SSL *sslConnection, int *cursor) {
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
  snprintf(message, sizeof(message), "A%d RENAME \"%s\" \"%s\"\r\n",
           (*cursor)++, mailBoxName, newMailBoxName);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Rename failed\n");
  free(mailBoxName);
  free(newMailBoxName);
}

void noop(SSL *sslConnection, int *cursor) {
  char message[100];
  snprintf(message, sizeof(message), "A%d NOOP\r\n", (*cursor)++);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Noop failed\n");
}