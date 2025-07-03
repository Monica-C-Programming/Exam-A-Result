#ifndef FILLEDSTRUCTS_H
#define FILLEDSTRUCTS_H

void serverAccept(int client_fd, const char *ip, const char *userID);
void statusCode(int client_fd, const char *code);
void serverHelo(int client_fd, const char *client_ip, const char *username);
void mailFromData(int client_fd);
void rcptTo(int client_fd);
void clientDataFile(int client_fd, const char *filename, const char *userID);

#endif
