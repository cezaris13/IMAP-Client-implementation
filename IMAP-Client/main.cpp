#include <iostream>
#include <print>
#include "appLayer.h"
#include "sockets.h"

void showImapCommands(SSL *sslConnection) {
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
           "14. close system \n"
           "15. login user from env\n"
           "16. noop\n"
           "17. search\n");

    scanf("%d", &count);
    switch (count) {
      case 1:
        checkConnectionStatus(sslConnection, &cursor);
        break;
      case 2:
        loginUser(sslConnection, &cursor);
        break;
      case 3:
        selectMailboxByName(sslConnection, &cursor);
        break;
      case 4:
        getMailByUID(sslConnection, &cursor);
        break;
      case 5:
        getAllEmailsFromMailbox(sslConnection, &cursor);
        break;
      case 6:
        moveEmailFromOneMailboxToAnother(sslConnection, &cursor);
        break;
      case 7:
        deleteEmailFromMailbox(sslConnection, &cursor);
        break;
      case 8:
        getMailboxes(sslConnection, &cursor);
        break;
      case 9:
        createMailBox(sslConnection, &cursor);
        break;
      case 10:
        renameMailbox(sslConnection, &cursor);
        break;
      case 11:
        deleteMailbox(sslConnection, &cursor);
        break;
      case 12:
        getEmailCountForMailbox(sslConnection, &cursor);
        break;
      case 13:
        logoutUser(sslConnection, &cursor);
        break;
      case 15:
        loginUserHardcoded(sslConnection, &cursor);
        break;
      case 16:
        noop(sslConnection, &cursor);
        break;
      case 17:
        search(sslConnection, &cursor);
        break;
      default:
        runProgram = 0;
        break;
    }
  }
}

int main() {
  std::unordered_map env = loadEnv(".env");

  if (env.find("IMAP_EMAIL") == env.end() || env.find("IMAP_PORT") == env.end()) {
    std::print("env file does not contain imap email or port.");
    return 0;
  }

  int fd = initializeClient(env["IMAP_EMAIL"].data(), env["IMAP_PORT"].data());
  if (fd < 0) {
    fprintf(stdout, "fd = %d, open file failed\n", fd);
    return 0;
  } else {
    fprintf(stderr, "fd = %d\n", fd);
  }
  SSL *sslConnection = connectSSL(fd);
  std::string result = imapRecv(sslConnection, 100);
  std::print("S: {}\n", result);

  showImapCommands(sslConnection);

  SSL_free(sslConnection);
  destroySSL();
}