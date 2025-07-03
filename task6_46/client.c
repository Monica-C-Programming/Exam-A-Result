#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tea.h"

#define BUFFER_SIZE 2048  // Buffer

void saveData(const char *filename, char *data, size_t size) { //Methode to create the file
    FILE *file = fopen(filename, "wb");
    if (file) {
        fwrite(data, 1, size, file);
        fclose(file);
    } else {
        perror("Error saving data");
    }
}

int main(int argc, char *argv[]) { //Main method, also defines values we will use
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytesReceived;
     
    const char *server_ip = "127.0.0.1"; // Default IP
    int port = 80;                       // Default port, yes 99% of the time this port will be denied, so its only for "show"
    
    for (int i = 1; i < argc; i++) { //Method to allow -server and -port in inputline, but still allows you to connect if you forget -server or -port
        if (strcmp(argv[i], "-server") == 0 && i + 1 < argc) {
            server_ip = argv[i + 1]; // Make sure to prioritize ip from userinput and not default
            i++;
        } else if (strcmp(argv[i], "-port") == 0 && i + 1 < argc) {
            int tempPort = atoi(argv[i + 1]);
            if (tempPort > 0 && tempPort <= 65535) {
                port = tempPort; //Makes sure to prioritize userinput for port number
            } else {
                fprintf(stderr, "Invalid portnumber. Using default port at 80.\n"); //Prints error if portnumber is not valid
            }
            i++;
        }
    }

    //Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr)); //Configurate server adress
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sockfd);
        return 1;
    }
    //Bind the sock to the adress
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    printf("Connected to the server at %s:%d\n", server_ip, port); //Prints out if connection is successfull

    // Function to receive the data from EWA
    bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0) {
        perror("Error receiving data");
        close(sockfd);
        return 1;
    }

    // Check Content-Length so that we only save the encrypted data in the file and know how much data to save
    char *contentLengthHeader = strstr(buffer, "Content-Length:");
    int contentLength = 0;

    if (contentLengthHeader) {
        sscanf(contentLengthHeader, "Content-Length: %d", &contentLength);
        printf("Content-Length: %d\n", contentLength); // Prints how many bytes the data we need is and use it for saving data
    } else {
        fprintf(stderr, "Content-Length header not found.\n"); //Prints error if the line "content_lenght" is not found
        close(sockfd);
        return 1;
    }

    // Find out where the data we need starts
    char *dataStart = strstr(buffer, "\r\n\r\n");
    if (!dataStart) {
        fprintf(stderr, "Something went wrong...\n"); //Prints error
        close(sockfd);
        return 1;
    }

    dataStart += 4;  // Skip the HTTP standard \r\n\r\n
    size_t headerDataSize = bytesReceived - (dataStart - buffer);

    // Allocate memory and save the data
    char *allData = malloc(contentLength);
    if (!allData) {
        perror("malloc failed"); //Prints an error if data allocation fails
        close(sockfd);
        return 1;
    }

    memcpy(allData, dataStart, headerDataSize);
    size_t totalReceived = headerDataSize;

    //Checks to see if all the data is received based on the content length and if not it continues to receive data
    while (totalReceived < contentLength) {
        ssize_t chunk = recv(sockfd, allData + totalReceived, contentLength - totalReceived, 0);
        if (chunk <= 0) {
            perror("recv failed during content reading"); //Prints error if something goes wrong and frees resources
            free(allData);
            close(sockfd);
            return 1;
        }
        totalReceived += chunk;
    }

    printf("Total encrypted data received: %zu bytes\n", totalReceived); //Prints out how much data received

    saveData("Saved_data.bin", allData, totalReceived); //Saves data to Saved_data.bin
    printf("Data saved to 'Saved_data.bin'.\n"); //Prints if the data was saved successfully
    teaDecrypt("Saved_data.bin", "decrypted_output.txt"); //Function to decrypt the received data

    //Frees all resources
    free(allData);
    close(sockfd);
    return 0;
}
