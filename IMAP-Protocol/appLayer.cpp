#include <openssl/err.h>
#include <openssl/ssl.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <print>
#include <format>
#include "sockets.h"
#include "appLayerUtils.cpp"
#define MESSAGE_LENGTH 320
#define MAX_MAILBOX_NAME_SIZE 20

void checkConnectionStatus(SSL *sslConnection, int *cursor) {
  std::string message = std::format("A{} CAPABILITY\r\n",(*cursor)++);
  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
    printf("Connection lost\n");
}

void loginUser(SSL *sslConnection, int *cursor) {
  std::string userName, password;
  printf("Enter your email address: ");
  std::cin >> userName;
  printf("Enter your password: ");
  std::cin >> password;
  std::string message = std::format("A{} LOGIN \"{}\" \"{}\"\r\n", (*cursor)++,userName,password);
  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
    printf("Login failed\n");
}

void loginUserHardcoded(SSL *sslConnection, int *cursor) {
  std::unordered_map env = loadEnv(".env");
  
  if (env.find("EMAIL") == env.end() || env.find("PASSWORD") == env.end()) {
    std::print("Failed to load enviroment variables.");
    return;
  }

  std::string message = std::format("A{} LOGIN \"{}\" \"{}\"\r\n", (*cursor)++, env["EMAIL"], env["PASSWORD"]);

  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
    printf("Login failed\n");
}

void logoutUser(SSL *sslConnection, int *cursor) {
  std::string message = "A" + std::to_string((*cursor)++) + " LOGOUT\r\n";
  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
    printf("Logout failed\n");
}

void selectMailboxByNameProvided(SSL *sslConnection, int *cursor,
                                 std::string mailBoxName) {
  std::string message = std::format("A{} SELECT \"{}\"\r\n", (*cursor)++, mailBoxName);

  if (!sendAndReceiveImapMessage(message, sslConnection, 1).statusCode)
    printf("Select failed\n");
}

void selectMailboxByName(SSL *sslConnection, int *cursor) {
  std::string mailBoxName;
  printf("Enter mailbox name: ");
  std::cin >> mailBoxName;
  if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE)
    printf("Mailbox name is too long\n");
  std::string message = std::format("A{} SELECT \"{}\"\r\n", (*cursor)++, mailBoxName);

  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
    printf("Select failed\n");
}

void getMailboxes(SSL *sslConnection, int *cursor) {
  std::string message = std::format("A{} LIST \"\" \"*\"\r\n", (*cursor)++);

  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
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
  std::string message = std::format("A{} CREATE \"{}\"\r\n", (*cursor)++, mailBoxName);

  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
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
  std::string message = std::format("A{} DELETE \"{}\"\r\n", (*cursor)++, mailBoxName);
  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
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
  std::string message = std::format("A{} RENAME \"{}\" \"{}\"\r\n", (*cursor)++, mailBoxName, newMailBoxName);

  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
    printf("Rename failed\n");
}

void noop(SSL *sslConnection, int *cursor) {
  std::string message = std::format("A{} NOOP\r\n", (*cursor)++);
  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
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
  std::string message = std::format("A{} EXAMINE \"{}\"\r\n", (*cursor)++, mailBoxName);

  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
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

  std::string message = std::format("A{} UID STORE {} +FLAGS (\\Deleted)\r\n", (*cursor)++, uid);
  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode ) {
    printf("Store failed\n");
    return;
  }
  message = std::format("A{} UID EXPUNGE {}\r\n", (*cursor)++, uid);
  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
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
  std::string message = std::format("A{} UID COPY {} {}\r\n", (*cursor)++, uid, newMailBoxName);
  if (!sendAndReceiveImapMessage(message, sslConnection, 1).statusCode) {
    printf("Copy failed\n");
    return;
  }
  message = std::format("A{} UID STORE {} +FLAGS (\\Deleted)\r\n", (*cursor)++, uid);

  if (!sendAndReceiveImapMessage(message, sslConnection, 1).statusCode) {
    printf("Store failed\n");
    return;
  }
  message = std::format("A{} UID EXPUNGE {}\r\n", (*cursor)++, uid);
  if (!sendAndReceiveImapMessage(message, sslConnection, 1).statusCode)
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

  std::string message = std::format("A{} search", (*cursor)++);
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
  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
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
  std::string message = std::format("A{} UID FETCH {} (BODY.PEEK[1])\r\n", (*cursor)++, uid);

  Data response = sendAndReceiveImapMessage(message, sslConnection, 1);
  size_t found = response.message.find("text/plain; charset=\"UTF-8\"");
  std::string t = response.message.substr(found + strlen("text/plain; charset=\"UTF-8\""));

  found = t.find("--");
  std::string text = t.substr(0, found);
  std::print("text: {}\n",text);

  message = std::format("A{} UID FETCH {} (BODY.PEEK[2])\r\n", (*cursor)++, uid);
  response = sendAndReceiveImapMessage(message, sslConnection, 1);
  message = response.message;
  std::string mess = message.substr(message.find("}") + 3, message.find(")") - message.find("}") - 3);
  mess.erase(remove(mess.begin(), mess.end(), '\r'));
  mess.erase(remove(mess.begin(), mess.end(), '\n'));
  mess[mess.size() - 1] = 0;
  response.message = base64Decode(mess);
  message = std::format("A{} UID FETCH {} (BODY.PEEK[TEXT])\r\n", (*cursor)++, uid);

  Data result = sendAndReceiveImapMessage(message, sslConnection, 1);
  found = result.message.find("filename");
  std::string filename = result.message.substr(found);
  filename = filename.substr(filename.find("\"") + 1, filename.find("\"", filename.find("\"") + 1) - filename.find("\"") - 1);
  if (!response.statusCode || !result.statusCode) {
    printf("Fetch failed\n");
    return;
  }

  std::ofstream file;
  file.open(filename);
  file << response.message;
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
  std::string message = std::format("A{} UID search ALL\r\n", (*cursor)++);
  if (!sendAndReceiveImapMessage(message, sslConnection, 0).statusCode )
    printf("Fetch failed\n");
}