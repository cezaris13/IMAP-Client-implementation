#include "appLayer.h"
#include "sockets.h"
#include <ctype.h>
#define MESSAGE_LENGTH 320
using namespace std;

int SendAndReceiveImapMessage(string command, SSL *sslConnection, int silent) {
  if (silent == 0)
    cout << "C: " << command << endl;
  SSL_write(sslConnection, command.c_str(), command.length());
  string response = imap_recv(sslConnection, 100);
  int is_ok = check_ok(response);
  if (silent == 0)
    cout << "S: " << response << endl;
  if (is_ok)
    return 0;
  else
    return -1;
}

void CheckConnectionStatus(SSL *sslConnection, int *cursor) {
  string message;
  message += "A" + to_string((*cursor)++) + " CAPABILITY\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Connection lost\n");
}

void LoginUser(SSL *sslConnection, int *cursor) {
  string userName, password;
  string message;
  printf("Enter your email address: ");
  cin >> userName;
  printf("Enter your password: ");
  cin >> password;
  message += "A" + to_string((*cursor)++) + " LOGIN \"" + userName + "\" \"" +
             password + "\"\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Login failed\n");
}

void LoginUserHardcoded(SSL *sslConnection, int *cursor) {
  string message;
  message += "A" + to_string((*cursor)++) + " LOGIN \"" +
             "kt.testimap2022@gmail.com" + "\" \"" + "Q!w2e3r4t5" + "\"\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Login failed\n");
}

void LogoutUser(SSL *sslConnection, int *cursor) {
  string message;
  message += "A" + to_string((*cursor)++) + " LOGOUT\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Logout failed\n");
}

void SelectMailboxByNameProvided(SSL *sslConnection, int *cursor,
                                 string mailBoxName) {
  string message;
  message +=
      "A" + to_string((*cursor)++) + " SELECT \"" + mailBoxName + "\"\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1)
    printf("Select failed\n");
}

void SelectMailboxByName(SSL *sslConnection, int *cursor) {
  string message;
  string mailBoxName;
  printf("Enter mailbox name: ");
  cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE)
    printf("Mailbox name is too long\n");
  message +=
      "A" + to_string((*cursor)++) + " SELECT \"" + mailBoxName + "\"\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Select failed\n");
}

void GetMailBoxes(SSL *sslConnection, int *cursor) {
  string message;
  message += "A" + to_string((*cursor)++) + " LIST \"\" \"*\"\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("List failed\n");
}

void CreateMailBox(SSL *sslConnection, int *cursor) {
  string message;
  string mailBoxName;
  printf("Enter mailbox name: ");
  cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  message +=
      "A" + to_string((*cursor)++) + " CREATE \"" + mailBoxName + "\"\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Create failed\n");
}

void DeleteMailBox(SSL *sslConnection, int *cursor) {
  string message;
  string mailBoxName;
  printf("Enter mailbox name: ");
  cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  message +=
      "A" + to_string((*cursor)++) + " DELETE \"" + mailBoxName + "\"\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Delete failed\n");
}

void RenameMailBox(SSL *sslConnection, int *cursor) {
  string message;
  string mailBoxName;
  string newMailBoxName;
  printf("Enter mailbox name: ");
  cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  printf("Enter new mailbox name: ");
  cin >> newMailBoxName;
  if (newMailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  message += "A" + to_string((*cursor)++) + " RENAME \"" + mailBoxName +
             "\" \"" + newMailBoxName + "\"\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Rename failed\n");
}

void noop(SSL *sslConnection, int *cursor) {
  string message;
  message += "A" + to_string((*cursor)++) + " NOOP\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Noop failed\n");
}

void GetEmailCountForMailBox(SSL *sslConnection, int *cursor) {
  string message;
  string mailBoxName;
  printf("Enter mailbox name: ");
  cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  message +=
      "A" + to_string((*cursor)++) + " EXAMINE \"" + mailBoxName + "\"\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Status failed\n");
}

void DeleteEmailFromMailBox(SSL *sslConnection, int *cursor) {
  string message;
  string mailBoxName;
  printf("Enter mailbox name: ");
  cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  string uid;
  printf("Enter uid: ");
  cin >> uid;

  message += "A" + to_string((*cursor)++) + " UID STORE " + uid +
             " +FLAGS (\\Deleted)\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1) {
    printf("Store failed\n");
    return;
  }
  message = "A" + to_string((*cursor)++) + " UID EXPUNGE " + uid + "\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Expunge failed\n");
}

void MoveEmailFromOneMailBoxToAnother(SSL *sslConnection, int *cursor) {
  string message;
  string mailBoxName;
  string newMailBoxName;
  string uid;
  printf("Enter mailbox name: ");
  cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  printf("Enter new mailbox name: ");
  cin >> newMailBoxName;
  if (newMailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }

  printf("Enter uid of email: ");
  cin >> uid;
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  message += "A" + to_string((*cursor)++) + " UID COPY " + uid + " " +
             newMailBoxName + "\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1) {
    printf("Copy failed\n");
    return;
  }
  message = "A" + to_string((*cursor)++) + " UID STORE " + uid +
            " +FLAGS (\\Deleted)\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1) {
    printf("Store failed\n");
    return;
  }
  message = "A" + to_string((*cursor)++) + " UID EXPUNGE " + uid + "\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 1) == -1)
    printf("Expunge failed\n");
}

void Search(SSL *sslConnection, int *cursor) {
  string message;
  string mailBoxName;
  string from;
  string to;
  string subject;
  string text;
  string nottext;
  string since;
  string before;
  size_t bufsize = 32;
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  getline(cin, from);
  printf("Enter from: ");
  getline(cin, from);
  printf("Enter to: ");
  getline(cin, to);
  printf("Enter subject: ");
  getline(cin, subject);
  printf("Enter text: ");
  getline(cin, text);
  printf("Enter nottext: ");
  getline(cin, nottext);
  printf("Enter since: ");
  getline(cin, since);
  printf("Enter before: ");
  getline(cin, before);
  from[from.size()] = 0;
  to[to.size()] = 0;
  subject[subject.size()] = 0;
  text[text.size()] = 0;
  nottext[nottext.size()] = 0;
  since[since.size()] = 0;
  before[before.size()] = 0;

  message += "A" + to_string((*cursor)++) + " SEARCH";
  if (from.length() != 0) {
    message += " FROM \"" + from + "\"";
  }
  if (to.length() != 0) {
    message += " TO \"" + to + "\"";
  }
  if (subject.length() != 0) {
    message += " SUBJECT \"" + subject + "\"";
  }
  if (text.length() != 0) {
    message += " TEXT \"" + text + "\"";
  }
  if (nottext.length() != 0) {
    message += " NOT TEXT \"" + nottext + "\"";
  }
  if (since.length() != 0) {
    message += " SINCE \"" + since + "\"";
  }
  if (before.length() != 0) {
    message += " BEFORE \"" + before + "\"";
  }
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  message += "\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Search failed\n");
}

void GetMailByUID(SSL *sslConnection, int *cursor) {
  string message;
  string mailBoxName;
  string uid;
  printf("Enter mailbox name: ");
  cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  printf("Enter UID: ");
  cin >> uid;
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);

  message = "A" + to_string((*cursor)++) + " UID FETCH " + uid +
            " (UID BODY[HEADER.FIELDS (FROM TO SUBJECT DATE)])\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Fetch failed\n");
}

// analyze
void GetHeaderOfEmailByUID(SSL *sslConnection, int *cursor, long uid) {
  string message;
  message = "A" + to_string((*cursor)++) + " UID FETCH " + to_string(uid) +
            " (UID BODY[HEADER.FIELDS (FROM TO SUBJECT DATE)])\r\n";
  if (SendAndReceiveImapMessage(message, sslConnection, 0) == -1)
    printf("Fetch failed\n");
}

void GetIntsFromString(SSL *sslConnection, int *cursor, string str) {
  stringstream ss;

  ss << str;

  string temp;
  int found;
  while (!ss.eof()) {

    ss >> temp;
    if (stringstream(temp) >> found) {
      cout << found << endl;
      GetHeaderOfEmailByUID(sslConnection, cursor, found);
    }
    temp = "";
  }
}

int SendAndReceiveImapMessage1(string command, SSL *sslConnection, int *cursor,
                               int silent) {
  if (silent == 0)
    cout << "C: " << command << endl;
  SSL_write(sslConnection, command.c_str(), command.length());
  string response = imap_recv(sslConnection, 100);
  int is_ok = check_ok(response);
  if (silent == 0)
    cout << "S: " << response << endl;
  if (is_ok) {
    GetIntsFromString(sslConnection, cursor, response.c_str());
    return 0;
  } else {
    return -1;
  }
}

void GetAllEmailsFromMailBox(SSL *sslConnection, int *cursor) {
  string message;
  string mailBoxName;
  printf("Enter mailbox name: ");
  cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  SelectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  message += "A" + to_string((*cursor)++) + " UID SEARCH ALL\r\n";
  if (SendAndReceiveImapMessage1(message, sslConnection, cursor, 0) == -1)
    printf("Fetch failed\n");
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