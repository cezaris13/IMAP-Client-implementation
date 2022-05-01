#ifndef MY_FUNC2_H
#define MY_FUNC2_H
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <bits/stdc++.h>
#define MAX_EMAIL_ADDRESS_LENGTH 320
#define MAX_MAILBOX_NAME_SIZE 20
using namespace std;
int SendAndReceiveImapMessage(string command, SSL *sslConnection, int silent);
void CheckConnectionStatus(SSL *sslConnection, int *cursor);
void LoginUser(SSL *sslConnection, int *cursor);
void LoginUserHardcoded(SSL *sslConnection, int *cursor);
void LogoutUser(SSL *sslConnection, int *cursor);
void SelectMailboxByNameProvided(SSL *sslConnection, int *cursor,
                                 string mailBoxName);
void SelectMailboxByName(SSL *sslConnection, int *cursor);
void GetMailBoxes(SSL *sslConnection, int *cursor);
void CreateMailBox(SSL *sslConnection, int *cursor);
void DeleteMailBox(SSL *sslConnection, int *cursor);
void RenameMailBox(SSL *sslConnection, int *cursor);
void noop(SSL *sslConnection, int *cursor);
void GetEmailCountForMailBox(SSL *sslConnection, int *cursor);
void DeleteEmailFromMailBox(SSL *sslConnection, int *cursor);
void MoveEmailFromOneMailBoxToAnother(SSL *sslConnection, int *cursor);
void GetMailByUID(SSL *sslConnection, int *cursor);
void Search(SSL *sslConnection, int *cursor);
void GetHeaderOfEmailByUID(SSL *sslConnection, int *cursor, long uid);
void GetIntsFromString(SSL *sslConnection, int *cursor, string str);
int SendAndReceiveImapMessage1(string command, SSL *sslConnection, int *cursor,
                               int silent);
void GetAllEmailsFromMailBox(SSL *sslConnection, int *cursor);
#endif
