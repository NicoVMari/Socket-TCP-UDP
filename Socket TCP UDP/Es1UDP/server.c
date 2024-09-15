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
    uint16_t simplePort = 0; // Porta su cui connettersi
    int returnStatus = 0; // Stato del ritorno delle funzioni di sistema
    struct sockaddr_in simpleServer, clientName; // Strutture per l'indirizzo del server e del client

    // Controlla che il programma sia invocato con il giusto numero di argomenti
    if (2 != argc) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // Crea un socket UDP
    simpleSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
    simpleServer.sin_addr.s_addr = htonl(INADDR_ANY); // Accetta connessioni da qualsiasi indirizzo
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

    int flag = 0; // Flag per inviare il messaggio di benvenuto una sola volta

    // Ciclo principale per ricevere messaggi dai client
    while (1) {
        char buffer[MAX_LENGTH]; // Buffer per la comunicazione
        socklen_t clientNameLength = sizeof(clientName); // Lunghezza della struttura del client
        ssize_t bytesRead = recvfrom(simpleSocket, buffer, MAX_LENGTH - 1, 0, (struct sockaddr *)&clientName, &clientNameLength); // Riceve un messaggio dal client

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0'; // Aggiunge il terminatore di stringa

            if(flag == 0){
                // Ottiene l'indirizzo IP del client
                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(clientName.sin_addr), clientIP, INET_ADDRSTRLEN);

                // Prepara e invia il messaggio di benvenuto al client
                char welcomeMessage[MAX_LENGTH];
                snprintf(welcomeMessage, sizeof(welcomeMessage), "Hello student with IP: %s\n", clientIP);
                sendto(simpleSocket, welcomeMessage, strlen(welcomeMessage), 0, (struct sockaddr *)&clientName, clientNameLength);

                flag = 1; // Imposta il flag per evitare di inviare nuovamente il messaggio di benvenuto
            }

            // Se il client invia "0", esce dal ciclo
            if (strncmp(buffer, "0", 1) == 0) {
                break;
            }

            fprintf(stderr, "Dati ricevuti: %s", buffer); // Stampa i dati ricevuti
            sendto(simpleSocket, "Ho ricevuto i dati", strlen("Ho ricevuto i dati"), 0, (struct sockaddr *)&clientName, clientNameLength); // Invia conferma di ricezione al client
        }
    }

    // Chiude il socket
    close(simpleSocket);
    return 0;
}
