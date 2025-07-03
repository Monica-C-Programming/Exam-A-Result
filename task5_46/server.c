#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "ewpdef.h"
#include "filledStructs.h"


//sourcecode for connection and arguments

// Method to check if the filename is acceptable before we send it to be used
int validFilename(const char *filename) {
    if (filename == NULL || strlen(filename) == 0) {
        return 0;
    }
    for (int i = 0; filename[i] != '\0'; i++) {
        if (filename[i] == '/' || filename[i] == '\\') {
            return 0;
        }
    }
    return 1;
}
    
int main(int argc, char *argv[]) {
    struct sockaddr_in saAddr = {0};
    struct sockaddr_in saConClient = {0};

    int sockFd, sockNewFd;
    int iPort = 8080; // Default port
    char userID[100] = "123"; // Default user ID
    socklen_t addrLen = sizeof(saConClient);

    // Handle command-line arguments, inspired by the hint in exam preparations by using argv and argc for the flags
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-port") == 0 && i + 1 < argc) {
            int tempPort = atoi(argv[i + 1]);
            if (tempPort > 0 && tempPort <= 65535) {
                iPort = tempPort;
                i++;
            } else {
                fprintf(stderr, "Invalid port, using default 8080.\n"); //If no port is given, we use the default one
            }
        } else if (strcmp(argv[i], "-id") == 0 && i + 1 < argc) {
            strncpy(userID, argv[i + 1], sizeof(userID) - 1);
            userID[sizeof(userID) - 1] = '\0';
            i++;
        }
    }

    printf("Using port: %d\n", iPort); //Prints what port we use
    printf("User ID: %s\n", userID); //Prints the userID

    // Create the socket for our server
    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("socket");
        return 1;
    }

    // Configure the server address
    saAddr.sin_family = AF_INET;
    saAddr.sin_port = htons(iPort);
    saAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to the given address
    if (bind(sockFd, (struct sockaddr*)&saAddr, sizeof(saAddr)) < 0) {
        perror("bind");
        return 1;
    }

    // Start listening
    if (listen(sockFd, 5) < 0) {
        perror("I listen now");
        return 1;
    }

    printf("Server is waiting for connection on port %d...\n", iPort);

    // Accept incoming connection
    sockNewFd = accept(sockFd, (struct sockaddr*)&saConClient, &addrLen);
    if (sockNewFd < 0) {
        perror("I did NOT accept your connection");
        return 1;
    }

    printf("Connection established from: %s\n", inet_ntoa(saConClient.sin_addr));

    char client_ip[INET_ADDRSTRLEN]; //Collects the IP adress
    inet_ntop(AF_INET, &saConClient.sin_addr, client_ip, sizeof(client_ip)); //Converts the ip adress
    printf("Connection from: %s\n", client_ip); //Prints out a message if connection succeeded
    serverAccept(sockNewFd, client_ip, userID);

    struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTHELO helo; // Receive and respond for hello

    //Make sure we start at 0
    memset(&helo, 0, sizeof(helo));

    //Receive information
    recv(sockNewFd, &helo, sizeof(helo), 0);
    char username[64] = {0}; //Received username

    //Format response
    strncpy(username, helo.acFormattedString, sizeof(username) - 1);

    //Send response
    printf("Received HELO from user: %s\n", username);
    serverHelo(sockNewFd, client_ip, username);



    mailFromData(sockNewFd); // Calling MAIL FROM and servercode reply
    statusCode(sockNewFd, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK);

    rcptTo(sockNewFd); // Calling RCPT TO and serverreply code
    statusCode(sockNewFd, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK);

    char dataCommandBuffer[512] = {0}; //Using for filename
    recv(sockNewFd, dataCommandBuffer, sizeof(dataCommandBuffer) - 1, 0);
    
    char filename[256] = {0}; //Receive filename from client
    char *dataPrefix = strstr(dataCommandBuffer, "DATA ");

    //Validate the name and pass it on if it is good or print an error if not
    if (dataPrefix && sscanf(dataPrefix, "DATA %255s", filename) == 1 && validFilename(filename)) {
        printf("Received DATA command for file: %s\n", filename);
        statusCode(sockNewFd, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_READY);
        clientDataFile(sockNewFd, filename, userID);
    } else {
        fprintf(stderr, "Invalid DATA command or filename\n");
    }
    
    // Handle QUIT command and close the connection
    void closeCommand(int sockNewFd, const char *userID); 


    close(sockNewFd);
    close(sockFd);
    return 0;
}
