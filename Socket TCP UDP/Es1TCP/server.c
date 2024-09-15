#include <stdio.h>        // Libreria standard per l'I/O
#include <sys/types.h>    // Definizioni di tipi di dati usati in system calls
#include <sys/socket.h>   // Definizioni per le operazioni sui socket
#include <netdb.h>        // Definizioni per la risoluzione di indirizzi di rete
#include <stdlib.h>       // Libreria standard per funzioni generiche (es. malloc, atoi, exit)
#include <string.h>       // Libreria per operazioni sulle stringhe (es. memset, strlen, strcmp)
#include <unistd.h>       // Definizioni di costanti e tipi di dati usati in POSIX (es. read, write, close)
#include <arpa/inet.h>    // Definizioni per la conversione degli indirizzi IP

const int MAX_LENGTH = 512; // Definizione della lunghezza massima del buffer

int main(int argc, char *argv[]) {
    int simpleSocket = 0; // Descrittore del socket
    uint16_t simplePort = 0; // Porta su cui ascoltare
    int returnStatus = 0; // Stato del ritorno delle funzioni di sistema
    struct sockaddr_in simpleServer; // Struttura per l'indirizzo del server

    // Controlla che il programma sia invocato con i giusti argomenti
    if (2 != argc) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // Crea un socket TCP
    simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (simpleSocket == -1) {
        fprintf(stderr, "Could not create a socket!\n");
        exit(1);
    } else {
        fprintf(stderr, "Socket created!\n");
    }

    // Converte la porta passata come argomento in un intero
    simplePort = (uint16_t)atoi(argv[1]);

    // Inizializza la struttura sockaddr_in con i dettagli del server
    memset(&simpleServer, '\0', sizeof(simpleServer));
    simpleServer.sin_family = AF_INET; // Famiglia di indirizzi (IPv4)
    simpleServer.sin_addr.s_addr = htonl(INADDR_ANY); // Accetta connessioni da qualsiasi indirizzo IP
    simpleServer.sin_port = htons(simplePort); // Porta del server (convertita in ordine dei byte di rete)

    // Associa il socket all'indirizzo e alla porta specificati
    returnStatus = bind(simpleSocket, (struct sockaddr *)&simpleServer, sizeof(simpleServer));
    if (returnStatus == 0) {
        fprintf(stderr, "Bind completed!\n-----------------------------\n");
    } else {
        fprintf(stderr, "Could not bind to address!\n");
        close(simpleSocket);
        exit(1);
    }

    // Mette il socket in ascolto di connessioni
    returnStatus = listen(simpleSocket, 5);
    if (returnStatus == -1) {
        fprintf(stderr, "Cannot listen on socket!\n");
        close(simpleSocket);
        exit(1);
    }

    while (1) {
        struct sockaddr_in clientName = { 0 }; // Struttura per l'indirizzo del client
        int simpleChildSocket = 0; // Descrittore del socket per la connessione accettata
        socklen_t clientNameLength = sizeof(clientName);

        // Accetta una connessione in ingresso
        simpleChildSocket = accept(simpleSocket, (struct sockaddr *)&clientName, &clientNameLength);
        if (simpleChildSocket == -1) {
            fprintf(stderr, "Cannot accept connections!\n");
            close(simpleSocket);
            exit(1);
        } else {
            fprintf(stderr, "Connections accept!\n");
        }

        // Converte l'indirizzo IP del client in una stringa leggibile
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientName.sin_addr), clientIP, INET_ADDRSTRLEN);

        // Invia un messaggio di benvenuto al client
        char welcomeMessage[MAX_LENGTH];
        snprintf(welcomeMessage, sizeof(welcomeMessage), "Hello student with IP: %s\n", clientIP);
        write(simpleChildSocket, welcomeMessage, strlen(welcomeMessage));

        // Ciclo per la comunicazione con il client
        while (1) {
            char buffer[MAX_LENGTH]; // Buffer per la comunicazione
            ssize_t bytesRead = read(simpleChildSocket, buffer, sizeof(buffer) - 1); // Legge dati dal client

            if (bytesRead > 0 && bytesRead < MAX_LENGTH - 1) {
                buffer[bytesRead] = '\0'; // Aggiunge il terminatore di stringa

                // Se il client invia "0", termina la comunicazione
                if (strncmp(buffer, "0", 1) == 0) {
                    break;
                }

                // Stampa i dati ricevuti dal client
                fprintf(stderr, "Dati ricevuti: %s", buffer);

                // Invia una risposta al client
                write(simpleChildSocket, "Ho ricevuto i dati", strlen("Ho ricevuto i dati"));
            } else {
                break;
            }
        }

        // Chiude il socket della connessione accettata
        close(simpleChildSocket);
        fprintf(stderr, "Connections close!\n-----------------------------\n");
    }

    // Chiude il socket principale
    close(simpleSocket);
    return 0;
}