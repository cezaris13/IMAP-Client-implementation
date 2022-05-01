#include "appLayer.h"
#include "sockets.h"
#include <ctype.h>
#include <string.h>
#define MESSAGE_LENGTH 320

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
    free(mailBoxName);
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
    free(mailBoxName);
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
    free(mailBoxName);
    free(newMailBoxName);
    return;
  }
  printf("Enter new mailbox name: ");
  scanf("%s", newMailBoxName);
  if (strlen(newMailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    free(mailBoxName);
    free(newMailBoxName);
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

void GetEmailCountForMailBox(SSL *sslConnection, int *cursor) {
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    free(mailBoxName);
    return;
  }
  snprintf(message, sizeof(message), "A%d EXAMINE \"%s\"\r\n", (*cursor)++,
           mailBoxName);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Status failed\n");
  free(mailBoxName);
}

void DeleteEmailFromMailBox(SSL *sslConnection, int *cursor) {
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    free(mailBoxName);
    return;
  }
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  char *uid = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter uid: ");
  scanf("%s", uid);

  snprintf(message, sizeof(message), "A%d UID STORE %s +FLAGS (\\Deleted)\r\n",
           (*cursor)++, uid);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1) {
    printf("Store failed\n");
    free(uid);
    free(mailBoxName);
    return;
  }
  snprintf(message, sizeof(message), "A%d UID EXPUNGE %s\r\n", (*cursor)++,
           uid);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Expunge failed\n");
  free(mailBoxName);
  free(uid);
}

void MoveEmailFromOneMailBoxToAnother(SSL *sslConnection, int *cursor) {
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  char *newMailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  char *uid = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    free(mailBoxName);
    free(newMailBoxName);
    free(uid);
    return;
  }
  printf("Enter new mailbox name: ");
  scanf("%s", newMailBoxName);
  if (strlen(newMailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    free(mailBoxName);
    free(newMailBoxName);
    free(uid);
    return;
  }

  printf("Enter uid of email: ");
  scanf("%s", uid);
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  snprintf(message, sizeof(message), "A%d UID COPY %s %s\r\n", (*cursor)++, uid,
           newMailBoxName);
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1) {
    printf("Copy failed\n");
    free(mailBoxName);
    free(newMailBoxName);
    free(uid);
    return;
  }
  snprintf(message, sizeof(message), "A%d UID STORE %s +FLAGS (\\Deleted)\r\n",
           (*cursor)++, uid);
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1) {
    printf("Store failed\n");
    free(mailBoxName);
    free(newMailBoxName);
    free(uid);
    return;
  }
  snprintf(message, sizeof(message), "A%d UID EXPUNGE %s\r\n", (*cursor)++,
           uid);
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1)
    printf("Expunge failed\n");
  free(mailBoxName);
  free(newMailBoxName);
  free(uid);
}

void Search(SSL *sslConnection, int *cursor) {
  char message[1000];
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
    free(mailBoxName);
    free(from);
    free(to);
    free(subject);
    free(text);
    free(nottext);
    free(since);
    free(before);
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
  if (strlen(from) != 0) {
    strcat(strcat(strcat(message, " FROM \""), from), "\"");
  }
  if (strlen(to) != 0) {
    strcat(strcat(strcat(message, " TO \""), to), "\"");
  }
  if (strlen(subject) != 0) {
    strcat(strcat(strcat(message, " SUBJECT \""), subject), "\"");
  }
  if (strlen(text) != 0) {
    strcat(strcat(strcat(message, " TEXT \""), text), "\"");
  }
  if (strlen(nottext) != 0) {
    strcat(strcat(strcat(message, " NOT TEXT \""), nottext), "\"");
  }
  if (strlen(since) != 0) {
    strcat(strcat(strcat(message, " SINCE \""), since), "\"");
  }
  if (strlen(before) != 0) {
    strcat(strcat(strcat(message, " BEFORE \""), before), "\"");
  }
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  strcat(message, "\r\n");
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

// analyze
void GetHeaderOfEmailByUID(SSL *sslConnection, int *cursor, long uid) {
  char message[100];
  snprintf(message, sizeof(message),
           "A%d FETCH %ld (UID BODY[HEADER.FIELDS (FROM SUBJECT)])\r\n",
           (*cursor)++, uid);
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Fetch failed\n");
}

void GetIntsFromString(SSL *sslConnection, int *cursor, char *str) {
  char *p = str;
  while (*p) {
    if (isdigit(*p) || ((*p == '-' || *p == '+') && isdigit(*(p + 1)))) {
      long val = strtol(p, &p, 10);
      printf("%ld\n", val);
      GetHeaderOfEmailByUID(sslConnection, cursor, val);
    } else {
      p++;
    }
  }
}

int SendAndReceiveImapMessage1(char *command, SSL *sslConnection, int *cursor,
                               int silent) {
  if (silent == 0)
    printf("C: %s\n", command);
  SSL_write(sslConnection, command, strlen(command));
  char *response = imap_recv(sslConnection, 100);
  int is_ok = check_ok(response);
  if (silent == 0)
    printf("S: %s\n", response);
  free(response);
  if (is_ok) {
    GetIntsFromString(sslConnection, cursor, response);
    return 0;
  } else {
    return -1;
  }
}

void GetAllEmailsFromMailBox(SSL *sslConnection, int *cursor) {
  char message[100];
  char *mailBoxName = malloc(MAX_MAILBOX_NAME_SIZE * sizeof(char));
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (strlen(mailBoxName) > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    free(mailBoxName);
    return;
  }
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  snprintf(message, sizeof(message), "A%d UID SEARCH ALL\r\n", (*cursor)++);
  if (SendAndReceiveImapMessage1(message, sslConnection, cursor, 0) == -1)
    printf("Fetch failed\n");
  free(mailBoxName);
}
// analyze later end

void ShowFileContents(char *fileName) {
  char *extension = strrchr(fileName, '.');
  if (extension != NULL && strcmp(extension, ".txt") == 0) {
    printf("\nPrinting contents of %s\n", fileName);
    FILE *file = fopen(fileName, "r");
    if (file != NULL) {
      char line[256];
      while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
      }
      fclose(file);
    } else {
      printf("\nCould not open file %s\n", fileName);
    }
  }
}