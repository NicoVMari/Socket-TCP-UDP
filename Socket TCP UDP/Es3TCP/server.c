#include <stdio.h>      
#include <sys/types.h>
#include <sys/socket.h>   
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

const int MAX_LENGTH = 512;

int main(int argc, char *argv[]) {

    int simpleSocket = 0;
    uint16_t simplePort = 0;
    int returnStatus = 0;
    struct sockaddr_in simpleServer;

    if (2 != argc) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (simpleSocket == -1) {
        fprintf(stderr, "Could not create a socket!\n");
        exit(1);
    } else {
        fprintf(stderr, "Socket created!\n");
    }

    simplePort = (uint16_t)atoi(argv[1]);

    memset(&simpleServer, '\0', sizeof(simpleServer));
    simpleServer.sin_family = AF_INET;
    simpleServer.sin_addr.s_addr = htonl(INADDR_ANY);
    simpleServer.sin_port = htons(simplePort);

    returnStatus = bind(simpleSocket, (struct sockaddr *)&simpleServer, sizeof(simpleServer));
    if (returnStatus == 0) {
        fprintf(stderr, "Bind completed!\n-----------------------------\n");
    } else {
        fprintf(stderr, "Could not bind to address!\n");
        close(simpleSocket);
        exit(1);
    }

    returnStatus = listen(simpleSocket, 5);
    if (returnStatus == -1) {
        fprintf(stderr, "Cannot listen on socket!\n");
        close(simpleSocket);
        exit(1);
    }

    while (1) {
        struct sockaddr_in clientName = { 0 };
        int simpleChildSocket = 0;
        socklen_t clientNameLength = sizeof(clientName);

        simpleChildSocket = accept(simpleSocket, (struct sockaddr *)&clientName, &clientNameLength);
        if (simpleChildSocket == -1) {
            fprintf(stderr, "Cannot accept connections!\n");
            close(simpleSocket);
            exit(1);
        } else {
            fprintf(stderr, "Connection accepted!\n");
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientName.sin_addr), clientIP, INET_ADDRSTRLEN);

        char welcomeMessage[MAX_LENGTH];
        snprintf(welcomeMessage, sizeof(welcomeMessage), "Hello student with IP: %s\n", clientIP);

        write(simpleChildSocket, welcomeMessage, strlen(welcomeMessage));
        
        char buffer[MAX_LENGTH];
        ssize_t bytesRead = read(simpleChildSocket, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            int interactions = atoi(buffer);

            for (int i = 0; i < interactions; i++) {
                // Read each message from the client
                bytesRead = read(simpleChildSocket, buffer, sizeof(buffer) - 1);
                if (bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                } else {
                    break;
                }
            }

            write(simpleChildSocket, "ack", strlen("ack"));
        }

        close(simpleChildSocket);
        fprintf(stderr, "Connection closed!\n-----------------------------\n");
    }

    close(simpleSocket);
    return 0;
}
