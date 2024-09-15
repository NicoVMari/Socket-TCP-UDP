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
    ssize_t returnStatus = 0; // Stato del ritorno delle funzioni di sistema
    struct sockaddr_in simpleServer; // Struttura per l'indirizzo del server

    // Controlla che il programma sia invocato con i giusti argomenti
    if (3 != argc) {
        fprintf(stderr, "Usage: %s <server> <port>\n", argv[0]);
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
    simplePort = (uint16_t)atoi(argv[2]);

    // Inizializza la struttura sockaddr_in con i dettagli del server
    memset(&simpleServer, '\0', sizeof(simpleServer));
    simpleServer.sin_family = AF_INET; // Famiglia di indirizzi (IPv4)
    simpleServer.sin_addr.s_addr = inet_addr(argv[1]); // Indirizzo IP del server
    simpleServer.sin_port = htons(simplePort); // Porta del server (convertita in ordine dei byte di rete)

    // Tenta di connettersi al server
    returnStatus = connect(simpleSocket, (struct sockaddr *)&simpleServer, sizeof(simpleServer));
    if (returnStatus == 0) {
        fprintf(stderr, "Connect successful!\n");
    } else {
        fprintf(stderr, "Could not connect to address!\n");
        close(simpleSocket);
        exit(1);
    }

    char buffer[MAX_LENGTH]; // Buffer per la comunicazione
    ssize_t status;

    // Legge dati dal server
    status = read(simpleSocket, buffer, sizeof(buffer));
    buffer[status] = '\0'; // Aggiunge il terminatore di stringa

    // Se i dati sono stati letti correttamente
    if (status > 0) {
        printf("%ld: %s", status, buffer);

        while (1) {
            // Chiede all'utente di inserire un messaggio per il server
            printf("Inserisci un messaggio per il server (0 per terminare): ");
            fgets(buffer, sizeof(buffer), stdin);

            // Se l'utente inserisce "0", termina il programma
            if (strncmp(buffer, "0", 1) == 0) {
                close(simpleSocket);
                exit(0);
            }

            // Invia il messaggio al server
            write(simpleSocket, buffer, strlen(buffer));

            // Ripulisce il buffer e legge la risposta del server
            memset(buffer, 0, sizeof(buffer));
            status = read(simpleSocket, buffer, sizeof(buffer));

            // Se i dati sono stati letti correttamente
            if (status > 0) {
                sscanf(buffer, "OK DATA");
                printf("Dati ricevuti dal server.\n");
            } else {
                fprintf(stderr, "Errore nella ricezione dei dati dal server.\n");
                break;
            }
        }
    } else {
        fprintf(stderr, "Return Status = %ld \n", status);
    }

    // Chiude il socket
    close(simpleSocket);
    return 0;
}
