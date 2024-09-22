#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <bits/stdc++.h>
#include <ctype.h>
#include "sockets.h"
#define MESSAGE_LENGTH 320
using namespace std;

struct Data {
  int statusCode;
  string message;
};


const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "abcdefghijklmnopqrstuvwxyz"
                            "0123456789+/";

bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

void ShowFileContents(string fileName) {
  if (fileName.substr(fileName.length() - 4, 4) != ".txt")
    return;
  ifstream file;
  file.open(fileName);
  string line;
  while (getline(file, line)) {
    cout << line << endl;
  }
  file.close();
}

string base64_decode(string const &encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  string ret;

  while (in_len-- && (encoded_string[in_] != '=') &&
         is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_];
    in_++;
    if (i == 4) {
      for (i = 0; i < 4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] =
          (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] =
          ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++)
      char_array_4[j] = 0;

    for (j = 0; j < 4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] =
        ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++)
      ret += char_array_3[j];
  }

  return ret;
}
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

Data SendAndReceiveImapMessage3(string command, SSL *sslConnection,
                                int silent) {
  Data data;
  if (silent == 0)
    cout << "C: " << command << endl;
  SSL_write(sslConnection, command.c_str(), command.length());
  string response = imap_recv(sslConnection, 100);
  int is_ok = check_ok(response);
  if (silent == 0)
    cout << "S: " << response << endl;
  if (is_ok)
    data.statusCode = 0;
  else
    data.statusCode = -1;
  data.message = response;

  return data;
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
            " (BODY.PEEK[1])\r\n";
  Data res0 = SendAndReceiveImapMessage3(message, sslConnection, 1);
  size_t found = res0.message.find("text/plain; charset=\"UTF-8\"");
  string t =
      res0.message.substr(found + strlen("text/plain; charset=\"UTF-8\""));
  found = t.find("--");
  string text = t.substr(0, found);
  cout << "text: " << text << endl;
  message = "A" + to_string((*cursor)++) + " UID FETCH " + uid +
            " (BODY.PEEK[2])\r\n";
  Data ret = SendAndReceiveImapMessage3(message, sslConnection, 1);
  string response = ret.message;
  string mess = response.substr(response.find("}") + 3,
                                response.find(")") - response.find("}") - 3);
  mess.erase(remove(mess.begin(), mess.end(), '\r'));
  mess.erase(remove(mess.begin(), mess.end(), '\n'));
  mess[mess.size() - 1] = 0;
  ret.message = base64_decode(mess);
  message = "A" + to_string((*cursor)++) + " UID FETCH " + uid +
            " (BODY.PEEK[TEXT])\r\n";
  Data result = SendAndReceiveImapMessage3(message, sslConnection, 1);
  found = result.message.find("filename");
  string filename = result.message.substr(found);
  filename = filename.substr(filename.find("\"") + 1,
                             filename.find("\"", filename.find("\"") + 1) -
                                 filename.find("\"") - 1);
  if (ret.statusCode == -1 || result.statusCode == -1) {
    printf("Fetch failed\n");
    return;
  }

  ofstream file;
  file.open(filename);
  file << ret.message;
  file.close();
  ShowFileContents(filename);
}

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

