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
    ssize_t returnStatus = 0;
    struct sockaddr_in simpleServer;

    if (3 != argc) {
        fprintf(stderr, "Usage: %s <server> <port>\n", argv[0]);
        exit(1);
    }

    simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (simpleSocket == -1) {
        fprintf(stderr, "Could not create a socket!\n");
        exit(1);
    } else {
        fprintf(stderr, "Socket created!\n");
    }

    simplePort = (uint16_t)atoi(argv[2]);

    memset(&simpleServer, '\0', sizeof(simpleServer));
    simpleServer.sin_family = AF_INET;
    simpleServer.sin_addr.s_addr = inet_addr(argv[1]);
    simpleServer.sin_port = htons(simplePort);

    returnStatus = connect(simpleSocket, (struct sockaddr *)&simpleServer, sizeof(simpleServer));
    if (returnStatus == 0) {
        fprintf(stderr, "Connect successful!\n");
    } else {
        fprintf(stderr, "Could not connect to address!\n");
        close(simpleSocket);
        exit(1);
    }

    char buffer[MAX_LENGTH];
    ssize_t status;

    status = read(simpleSocket, buffer, sizeof(buffer) - 1);
    if (status > 0) {
        buffer[status] = '\0';
        printf("%ld: %s", status, buffer);

        int interactions = 0;
        printf("Inserisci numero iterazioni: ");
        scanf("%d", &interactions);
        getchar();

        snprintf(buffer, sizeof(buffer), "%d", interactions);
        write(simpleSocket, buffer, strlen(buffer));

        for (int i = 0; i < interactions; i++) {
            printf("Inserisci messaggio: ");
            fgets(buffer, sizeof(buffer), stdin);

            write(simpleSocket, buffer, strlen(buffer));
        }

        write(simpleSocket, "bye",strlen("bye"));
        memset(buffer, 0, sizeof(buffer));
        status = read(simpleSocket, buffer, sizeof(buffer) - 1);

        if (status > 0) {
            buffer[status] = '\0';
            printf("Risposta server: %s\n", buffer);
        } else {
            fprintf(stderr, "Errore lettura dati dal server\n");
        }

    } else {
        fprintf(stderr, "Return Status = %ld \n", status);
    }

    close(simpleSocket);
    return 0;
}
