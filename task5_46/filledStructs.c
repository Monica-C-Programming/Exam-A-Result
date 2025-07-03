#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>

#include "ewpdef.h"  

//Sourcecode to fill all the structures

void buildHeader(struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER *header, int size) {  //Build the header used across the functions
    memcpy(header->acMagicNumber, EWA_EXAM25_TASK5_PROTOCOL_MAGIC, 3);
    sprintf(header->acDataSize, "%04d", size);
    header->acDelimeter[0] = '|';
}

void serverAccept(int client_fd, const char *ip, const char *userID) { //Function for server accept
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT response = {0};

    //Build the header
    buildHeader(&response.stHead, sizeof(response) - sizeof(response.stHead)); 

    //Set the status response
    memcpy(response.acStatusCode, "220", sizeof(response.acStatusCode)); 
    response.acHardSpace[0] = ' ';  

    //Using timer for timestamp in the respons
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%d %b %Y, %H:%M:%S", t);

    //Formatting the message
    sprintf(response.acFormattedString, "%s SMTP %s %s", ip, userID, timestamp); 
    response.acHardZero[0] = '\0';

    //Define the size of the response
    char buffer[64] = {0};
    memcpy(buffer, &response, sizeof(response));

    //Send the response
    ssize_t bytesSent = send(client_fd, buffer, 64, 0);
    if (bytesSent < 0) {
        perror("send");
    }
}



void clientHelo(int client_fd, const char *username, const char *client_ip) { //Function for clienthelo
    struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTHELO request = {0};

    //Build the header
    buildHeader(&request.stHead, sizeof(request) - sizeof(request.stHead)); 

    //Set the response
    strncpy(request.acCommand, "HELO", sizeof(request.acCommand) - 1); 
    request.acHardSpace[0] = ' ';  

    //Format the response
    snprintf(request.acFormattedString, sizeof(request.acFormattedString), "%s.%s", username, client_ip); 
    request.acHardZero[0] = 0; 

    //Send response
    ssize_t bytesSent = send(client_fd, &request, sizeof(request), 0); 
    if (bytesSent < 0) {
        perror("send");
    }
}

void serverHelo(int client_fd, const char *client_ip, const char *username) { //Function for serverHelo
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERHELO response = {0};

    //Build header 
    buildHeader(&response.stHead, sizeof(response) - sizeof(response.stHead)); 

    //Set the status code
    strncpy(response.acStatusCode, "250", sizeof(response.acStatusCode)); 

    //Hardspace
    response.acHardSpace[0] = ' ';

    //Set the formatted response
    snprintf(response.acFormattedString, sizeof(response.acFormattedString),
             "%s Hello %s", client_ip, username); 
    response.acHardZero[0] = 0; 

    //Send the response
    ssize_t bytesSent = send(client_fd, &response, sizeof(response), 0); 
    if (bytesSent < 0) {
        perror("send"); 
    }
}

void serverReply(int client_fd, const char *status_code, const char *message) { //Function for serverreply
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY reply = {0};

    //Build header
    buildHeader(&reply.stHead, sizeof(reply) - sizeof(reply.stHead)); 

    //Set the status code
    strncpy(reply.acStatusCode, status_code, sizeof(reply.acStatusCode)); 
    reply.acHardSpace[0] = ' ';

    //Format the message
    snprintf(reply.acFormattedString, sizeof(reply.acFormattedString), "%s", message); 
    reply.acHardZero[0] = '\0'; 

    //Send the message
    ssize_t sendSize = send(client_fd, &reply, sizeof(reply), 0); 
    if (sendSize < 0) {
        perror("serverReply: send failed"); 
    }
}

void statusCode(int sockFd, const char *status_code) { //Function for the statuscodes
    if (strcmp(status_code, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK) == 0) { //Status ready 
        serverReply(sockFd, status_code, "Sender address ok | Ready for message");
    } else if (strcmp(status_code, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_READY) == 0) { //Status ready
        serverReply(sockFd, status_code, "Ready for message");
    } else if (strcmp(status_code, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_CLOSED) == 0) { //Status close
        serverReply(sockFd, status_code, "Closing connection");
        close(sockFd);
    } else {
        serverReply(sockFd, "500", "Unknown status code"); // Status unknown
    }
}


void mailFromData(int sockNewFd) { //Function for mailfromData
    struct EWA_EXAM25_TASK5_PROTOCOL_MAILFROM mailFrom;

    memset(&mailFrom, 0, sizeof(mailFrom)); //Makes sure the memory starts from zero

    //Build header
    buildHeader(&mailFrom.stHead, sizeof(mailFrom) - sizeof(mailFrom.stHead)); 

    //Reads the data received and returns and prints an error if it fails
    ssize_t recvSize = recv(sockNewFd, &mailFrom, sizeof(mailFrom), 0);
    if (recvSize < 0) { 
        perror("Something went wrong in receiving data in mailFromData");
        close(sockNewFd);
        return;
    }
    //Prints out a message if everything is ok
    printf("Received mail from: %s\n", mailFrom.acFormattedString); 
}

void rcptTo(int sockNewFd) { //Function for rcpt to
    struct EWA_EXAM25_TASK5_PROTOCOL_RCPTTO rcptTo;
    
    //Makes sure we start at 0
    memset(&rcptTo, 0, sizeof(rcptTo));

    //Build the header
    buildHeader(&rcptTo.stHead, sizeof(rcptTo) - sizeof(rcptTo.stHead)); 

    //Reads the data received and returns and prints an error if it fails
    ssize_t recvSize = recv(sockNewFd, &rcptTo, sizeof(rcptTo), 0); 
    if (recvSize < 0) {
        perror("Something went wrong in receiving data in rcptTo");
        close(sockNewFd);
        return;
    } //Checks if we received any data
    if (recvSize == 0) { 
        printf("We didnt receive any data in rcptTo \n");
        close(sockNewFd);
        return;
    }
   //Prints if we receive data as planned
    printf("Received rcpt data TO: %s\n", rcptTo.acFormattedString); 
}

//Function for clientdatacmd
void dataCommand(int sockNewFd, char *filename, size_t filenameSize) { 
    struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATACMD dataCmd;

    //Makes sure we start at 0
    memset(&dataCmd, 0, sizeof(dataCmd));

    //Build the header
    buildHeader(&dataCmd.stHead, sizeof(dataCmd) - sizeof(dataCmd.stHead)); 

    //Reads the data received and returns and prints an error if it fails
    ssize_t recvSize = recv(sockNewFd, &dataCmd, sizeof(dataCmd), 0); 
    if (recvSize < 0) {
        perror("Something went wrong in receiving data command.");
        close(sockNewFd);
        return; //Returns
    }
    if (recvSize == 0) { //If we receive 0 data we assume the connection is closed
        printf("Connection might be closed.\n");
        close(sockNewFd);
        return;
    }
    //Prints if everything is good
    printf("Received DATA command for file: %s\n", dataCmd.acFormattedString);
}

//Function for clientdatafile
void clientDataFile(int sockNewFd, const char *filename, const char *userID) { 
    struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATAFILE dataFileHeader;

    //Makes sure we start at 0
    memset(&dataFileHeader, 0, sizeof(dataFileHeader));

    // receive header for file
    ssize_t recvSize = recv(sockNewFd, &dataFileHeader, sizeof(dataFileHeader.stHead), 0); 
    if (recvSize < 0) {
        perror("recv header for client data file");
        close(sockNewFd);
        return;
    }

    // reads the filesize filesize
    size_t dataSize = atoi(dataFileHeader.stHead.acDataSize); 
    if (dataSize <= 0 || dataSize > 9998) {
        printf("Invalid data size received: %zu\n", dataSize);
        close(sockNewFd);
        return;
    }

    // Allocate memory for the file
    char *fileContent = malloc(dataSize); 
    if (fileContent == NULL) {
        perror("Something went wrong in malloc.");
        close(sockNewFd);
        return;
    }

    // Receiving data for the file
    recvSize = recv(sockNewFd, fileContent, dataSize, 0); 
    if (recvSize < 0) {
        perror("Something went wrong in receiving data..");
        free(fileContent); //Closes and frees resources if it fails
        close(sockNewFd);
        return;
    }

    // Creates the file for data
    FILE *file = fopen(filename, "wb"); 
    if (file == NULL) {
        perror("Failed to open file for writing");
        free(fileContent);
        close(sockNewFd);
        return;
    }

    // Write data to out file
    size_t bytesWritten = fwrite(fileContent, 1, recvSize, file);
    if (bytesWritten != recvSize) {
        perror("Error writing file data");
    } else {
        printf("File received and saved successfully as '%s'.\n", filename);
    }

    // Free recourses
    free(fileContent);
    fclose(file);

    // Sends confirmation that file was received and saved without problem
    serverReply(sockNewFd, "250", "File received and saved successfully");
}

// Function for QUIT
void closeCommand(int sockFd) {
    struct EWA_EXAM25_TASK5_PROTOCOL_CLOSECOMMAND cmd;
    ssize_t recvSize;

    while (1) {
        memset(&cmd, 0, sizeof(cmd)); //Make sure we start at 0

        // Receive exactly one struct
        recvSize = recv(sockFd, &cmd, sizeof(cmd), 0);
        if (recvSize <= 0) {
            if (recvSize == 0) {
                printf("Client closed the connection.\n");
            } else {
                perror("Something went wrong..");
            }
            break;
        }

        // Check if commanc is DATA
        if (strncmp(cmd.acCommand, "DATA", 4) == 0) {
            // Save data received after DATA
            char filename[256];
            sscanf(cmd.acFormattedString, "%255s", filename);
            printf("Received DATA for file: %s\n", filename);

            clientDataFile(sockFd, filename, "userID");
        }
        else if (strncmp(cmd.acCommand, "QUIT", 4) == 0) { //If the quit command is received
            printf("Received QUIT command.\n");
        
            struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY serverReply = {0};
        
            // Bild header
            buildHeader(&serverReply.stHead, sizeof(serverReply) - sizeof(serverReply.stHead));

            //Set status code and
            strncpy(serverReply.acStatusCode, "221", sizeof(serverReply.acStatusCode));
            serverReply.acHardSpace[0] = ' ';

            //Format respond
            strncpy(serverReply.acFormattedString, "See ya later!", sizeof(serverReply.acFormattedString) - 1);
            serverReply.acHardZero[0] = '\0';
        
            // Send the whole structure
            ssize_t sendSize = send(sockFd, &serverReply, sizeof(serverReply), 0);
            if (sendSize < 0) {
                perror("Something went wrong in sending quit reply.. ");
            }
        
            printf("Sent 221 reply. Closing connection.\n");
            break;
        }
        close(sockFd);
    }
    close(sockFd);
    }
