#include <openssl/err.h>
#include <openssl/ssl.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "sockets.h"
#include "appLayerUtils.cpp"
#define MESSAGE_LENGTH 320
#define MAX_MAILBOX_NAME_SIZE 20

void checkConnectionStatus(SSL *sslConnection, int *cursor) {
  std::string message = "A" + std::to_string((*cursor)++) + " CAPABILITY\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Connection lost\n");
}

void loginUser(SSL *sslConnection, int *cursor) {
  std::string userName, password;
  printf("Enter your email address: ");
  std::cin >> userName;
  printf("Enter your password: ");
  std::cin >> password;
  std::string message = "A" + std::to_string((*cursor)++) + " LOGIN \"" + userName + "\" \"" + password + "\"\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Login failed\n");
}

void loginUserHardcoded(SSL *sslConnection, int *cursor) {
  std::string message = "A" + std::to_string((*cursor)++) + " LOGIN \"" + "kt.testimap2022@gmail.com" + "\" \"" + "Q!w2e3r4t5" + "\"\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Login failed\n");
}

void logoutUser(SSL *sslConnection, int *cursor) {
  std::string message = "A" + std::to_string((*cursor)++) + " LOGOUT\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Logout failed\n");
}

void selectMailboxByNameProvided(SSL *sslConnection, int *cursor,
                                 std::string mailBoxName) {
  std::string message = "A" + std::to_string((*cursor)++) + " SELECT \"" + mailBoxName + "\"\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 1).statusCode == -1)
    printf("Select failed\n");
}

void selectMailboxByName(SSL *sslConnection, int *cursor) {
  std::string mailBoxName;
  printf("Enter mailbox name: ");
  std::cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE)
    printf("Mailbox name is too long\n");
  std::string message = "A" + std::to_string((*cursor)++) + " SELECT \"" + mailBoxName + "\"\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Select failed\n");
}

void getMailboxes(SSL *sslConnection, int *cursor) {
  std::string message = "A" + std::to_string((*cursor)++) + " LIST \"\" \"*\"\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("List failed\n");
}

void createMailBox(SSL *sslConnection, int *cursor) {
  std::string mailBoxName;
  printf("Enter mailbox name: ");
  std::cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  std::string message = "A" + std::to_string((*cursor)++) + " CREATE \"" + mailBoxName + "\"\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Create failed\n");
}

void deleteMailbox(SSL *sslConnection, int *cursor) {
  std::string mailBoxName;
  printf("Enter mailbox name: ");
  std::cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  std::string message = "A" + std::to_string((*cursor)++) + " DELETE \"" + mailBoxName + "\"\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Delete failed\n");
}

void renameMailbox(SSL *sslConnection, int *cursor) {
  std::string mailBoxName;
  std::string newMailBoxName;
  printf("Enter mailbox name: ");
  std::cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  printf("Enter new mailbox name: ");
  std::cin >> newMailBoxName;
  if (newMailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  std::string message = "A" + std::to_string((*cursor)++) + " RENAME \"" + mailBoxName + "\" \"" + newMailBoxName + "\"\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Rename failed\n");
}

void noop(SSL *sslConnection, int *cursor) {
  std::string message = "A" + std::to_string((*cursor)++) + " NOOP\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Noop failed\n");
}

void getEmailCountForMailbox(SSL *sslConnection, int *cursor) {
  std::string mailBoxName;
  printf("Enter mailbox name: ");
  std::cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  std::string message = "A" + std::to_string((*cursor)++) + " EXAMINE \"" + mailBoxName + "\"\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Status failed\n");
}

void deleteEmailFromMailbox(SSL *sslConnection, int *cursor) {
  std::string mailBoxName;
  printf("Enter mailbox name: ");
  std::cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  selectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  std::string uid;
  printf("Enter uid: ");
  std::cin >> uid;

  std::string message = "A" + std::to_string((*cursor)++) + " UID STORE " + uid + " +FLAGS (\\Deleted)\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1) {
    printf("Store failed\n");
    return;
  }
  message = "A" + std::to_string((*cursor)++) + " UID EXPUNGE " + uid + "\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Expunge failed\n");
}

void moveEmailFromOneMailboxToAnother(SSL *sslConnection, int *cursor) {
  std::string mailBoxName;
  std::string newMailBoxName;
  std::string uid;
  printf("Enter mailbox name: ");
  std::cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  printf("Enter new mailbox name: ");
  std::cin >> newMailBoxName;
  if (newMailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }

  printf("Enter uid of email: ");
  std::cin >> uid;
  selectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  std::string message = "A" + std::to_string((*cursor)++) + " UID COPY " + uid + " " + newMailBoxName + "\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 1).statusCode == -1) {
    printf("Copy failed\n");
    return;
  }
  message = "A" + std::to_string((*cursor)++) + " UID STORE " + uid + " +FLAGS (\\Deleted)\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 1).statusCode == -1) {
    printf("Store failed\n");
    return;
  }
  message = "A" + std::to_string((*cursor)++) + " UID EXPUNGE " + uid + "\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 1).statusCode == -1)
    printf("Expunge failed\n");
}

void search(SSL *sslConnection, int *cursor) {
  std::string mailBoxName;
  std::string from;
  std::string to;
  std::string subject;
  std::string text;
  std::string notText;
  std::string since;
  std::string before;
  printf("Enter mailbox name: ");
  scanf("%s", mailBoxName);
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  getline(std::cin, from);
  printf("Enter from: ");
  getline(std::cin, from);
  printf("Enter to: ");
  getline(std::cin, to);
  printf("Enter subject: ");
  getline(std::cin, subject);
  printf("Enter text: ");
  getline(std::cin, text);
  printf("Enter notText: ");
  getline(std::cin, notText);
  printf("Enter since: ");
  getline(std::cin, since);
  printf("Enter before: ");
  getline(std::cin, before);
  from[from.size()] = 0;
  to[to.size()] = 0;
  subject[subject.size()] = 0;
  text[text.size()] = 0;
  notText[notText.size()] = 0;
  since[since.size()] = 0;
  before[before.size()] = 0;

  std::string message = "A" + std::to_string((*cursor)++) + " search";
  if (from.length() != 0)
    message += " FROM \"" + from + "\"";
  
  if (to.length() != 0)
    message += " TO \"" + to + "\"";
  
  if (subject.length() != 0)
    message += " SUBJECT \"" + subject + "\"";
  
  if (text.length() != 0)
    message += " TEXT \"" + text + "\"";
  
  if (notText.length() != 0)
    message += " NOT TEXT \"" + notText + "\"";
  
  if (since.length() != 0)
    message += " SINCE \"" + since + "\"";
  
  if (before.length() != 0)
    message += " BEFORE \"" + before + "\"";
  
  selectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  message += "\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("search failed\n");
}

void getMailByUID(SSL *sslConnection, int *cursor) {
  std::string mailBoxName;
  std::string uid;
  printf("Enter mailbox name: ");
  std::cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  printf("Enter UID: ");
  std::cin >> uid;
  selectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  std::string message = "A" + std::to_string((*cursor)++) + " UID FETCH " + uid + " (BODY.PEEK[1])\r\n";
  Data res0 = sendAndReceiveImapMessage(message, sslConnection, 1);
  size_t found = res0.message.find("text/plain; charset=\"UTF-8\"");
  std::string t = res0.message.substr(found + strlen("text/plain; charset=\"UTF-8\""));
  found = t.find("--");
  std::string text = t.substr(0, found);
  std::cout << "text: " << text << std::endl;
  message = "A" + std::to_string((*cursor)++) + " UID FETCH " + uid + " (BODY.PEEK[2])\r\n";
  Data ret = sendAndReceiveImapMessage(message, sslConnection, 1);
  std::string response = ret.message;
  std::string mess = response.substr(response.find("}") + 3, response.find(")") - response.find("}") - 3);
  mess.erase(remove(mess.begin(), mess.end(), '\r'));
  mess.erase(remove(mess.begin(), mess.end(), '\n'));
  mess[mess.size() - 1] = 0;
  ret.message = base64Decode(mess);
  message = "A" + std::to_string((*cursor)++) + " UID FETCH " + uid + " (BODY.PEEK[TEXT])\r\n";
  Data result = sendAndReceiveImapMessage(message, sslConnection, 1);
  found = result.message.find("filename");
  std::string filename = result.message.substr(found);
  filename = filename.substr(filename.find("\"") + 1, filename.find("\"", filename.find("\"") + 1) - filename.find("\"") - 1);
  if (ret.statusCode == -1 || result.statusCode == -1) {
    printf("Fetch failed\n");
    return;
  }

  std::ofstream file;
  file.open(filename);
  file << ret.message;
  file.close();
  showFileContents(filename);
}

void getAllEmailsFromMailbox(SSL *sslConnection, int *cursor) {
  std::string mailBoxName;
  printf("Enter mailbox name: ");
  std::cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
    printf("Mailbox name is too long\n");
    return;
  }
  selectMailboxByNameProvided(sslConnection, cursor, mailBoxName);
  std::string message = "A" + std::to_string((*cursor)++) + " UID search ALL\r\n";
  if (sendAndReceiveImapMessage(message, sslConnection, 0).statusCode == -1)
    printf("Fetch failed\n");
}