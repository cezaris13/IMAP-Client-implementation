#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<bits/stdc++.h>
#define MAX_EMAIL_ADDRESS_LENGTH 320
#define MAX_MAILBOX_NAME_SIZE 20
using namespace std;
#include "appLayer.h"
#include "sockets.h"

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
  string result = imap_recv(sslConnection, 100);
  cout<<"S: "<<result<<endl;

  ShowImapCommands(sslConnection);

  SSL_free(sslConnection);
  DestroySSL();
}