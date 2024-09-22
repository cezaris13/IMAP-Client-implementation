#ifndef MY_FUNC2_H
#define MY_FUNC2_H
#include "appLayer.cpp"
void checkConnectionStatus(SSL *sslConnection, int *cursor);
void loginUser(SSL *sslConnection, int *cursor);
void loginUserHardcoded(SSL *sslConnection, int *cursor);
void logoutUser(SSL *sslConnection, int *cursor);
void selectMailboxByNameProvided(SSL *sslConnection, int *cursor,
                                 string mailBoxName);
void selectMailboxByName(SSL *sslConnection, int *cursor);
void getMailboxes(SSL *sslConnection, int *cursor);
void createMailBox(SSL *sslConnection, int *cursor);
void deleteMailbox(SSL *sslConnection, int *cursor);
void renameMailbox(SSL *sslConnection, int *cursor);
void noop(SSL *sslConnection, int *cursor);
void getEmailCountForMailbox(SSL *sslConnection, int *cursor);
void deleteEmailFromMailbox(SSL *sslConnection, int *cursor);
void moveEmailFromOneMailboxToAnother(SSL *sslConnection, int *cursor);
void getMailByUID(SSL *sslConnection, int *cursor);
void search(SSL *sslConnection, int *cursor);
void getAllEmailsFromMailbox(SSL *sslConnection, int *cursor);
#endif
