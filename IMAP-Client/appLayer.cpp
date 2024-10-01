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

std::string getMailboxName() {
    std::string mailBoxName;
    std::cout << "Enter mailbox name: ";
    std::cin >> mailBoxName;
    if (mailBoxName.length() > MAX_MAILBOX_NAME_SIZE) {
        std::cerr << "Mailbox name is too long" << std::endl;
        return "";
    }
    return mailBoxName;
}

// Helper function to get UID from the user
std::string getUID() {
    std::string uid;
    std::cout << "Enter UID: ";
    std::cin >> uid;
    return uid;
}

// Helper function to fetch a specific part of an email by UID
std::string fetchEmailPart(SSL *sslConnection, int *cursor, const std::string &uid, std::string part) {
    std::string message = std::format("A{} UID FETCH {} (BODY.PEEK[{}])\r\n", (*cursor)++, uid, part);
    Data response = sendAndReceiveImapMessage(message, sslConnection, 1);
    return response.message;
}

// Helper function to extract plain text from the email body
std::string extractPlainText(const std::string &message) {
    size_t found = message.find("text/plain; charset=\"UTF-8\"");
    if (found == std::string::npos) return "";

    std::string t = message.substr(found + strlen("text/plain; charset=\"UTF-8\""));
    found = t.find("--");
    if (found != std::string::npos)
        return t.substr(0, found);
    return "";
}

// Helper function to decode base64 encoded message body
std::string decodeBase64Message(const std::string &message) {
    std::string base64Part = message.substr(message.find("}") + 3, message.find(")") - message.find("}") - 3);
    base64Part.erase(std::remove(base64Part.begin(), base64Part.end(), '\r'), base64Part.end());
    base64Part.erase(std::remove(base64Part.begin(), base64Part.end(), '\n'), base64Part.end());
    if (!base64Part.empty()) {
        base64Part[base64Part.size() - 1] = 0;
    }
    return base64Decode(base64Part);
}

// Helper function to extract the filename from an email body
std::string extractFilename(const std::string &message) {
    size_t found = message.find("filename");
    if (found == std::string::npos) return "";

    std::string filename = message.substr(found);
    size_t firstQuote = filename.find("\"");
    size_t secondQuote = filename.find("\"", firstQuote + 1);
    if (firstQuote != std::string::npos && secondQuote != std::string::npos) {
        return filename.substr(firstQuote + 1, secondQuote - firstQuote - 1);
    }
    return "";
}

// Main function to fetch an email by UID and write its content to a file
void getMailByUID(SSL *sslConnection, int *cursor) {
    std::string mailBoxName = getMailboxName();
    if (mailBoxName.empty()) return;

    std::string uid = getUID();
    selectMailboxByNameProvided(sslConnection, cursor, mailBoxName);

    // Fetch plain text part
    std::string message = fetchEmailPart(sslConnection, cursor, uid, "1");
    std::string text = extractPlainText(message);
    std::cout << "Text: " << text << std::endl;

    // Fetch and decode message part 2 (base64 encoded)
    std::string encodedMessage = fetchEmailPart(sslConnection, cursor, uid, "2");
    std::string decodedMessage = decodeBase64Message(encodedMessage);

    // Fetch attachment part
    std::string resultMessage = fetchEmailPart(sslConnection, cursor, uid, "TEXT");
    std::string filename = extractFilename(resultMessage);

    if (filename.empty()) {
        std::cerr << "Fetch failed: No filename found" << std::endl;
        return;
    }

    // Write content to file
    std::ofstream file(filename);
    if (file.is_open()) {
        file << decodedMessage;
        file.close();
        showFileContents(filename);
    } else {
        std::cerr << "Failed to open file: " << filename << std::endl;
    }
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