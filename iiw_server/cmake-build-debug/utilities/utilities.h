#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <signal.h>
#include <time.h>

//global macro
#define MAXLINE 1024
#define MAX_ATTEMPTS 16
#define LAST_TIMEOUT 10

// variabili di comunicazione
int numberOfPackets = 0;
int windowSize = 0;
int lossRate = 0;
int timeout = 0;
int serverPort = 8080;
int adaptiveTimeout = 0;

int attempt = 0;
int alarmTrigger = -1;
int sequenceBase = -1;

struct connectionPacket {
    int type; //-1: Errore
    // 0: Comando o ACK

    char data[MAXLINE];
    int serverPort; // porta su cui il server stabilisce la connessione
    int numberOfPackets;
    int windowSize;
    int lossRate;
    int timeout;
    int adaptiveTimeout;
};

struct goBackNPacket {
    int type; // 0: ACK
    // 1: Dati
    int sequenceNumber;
    int dataLength;
    char data[MAXLINE];
};

int checkDirectory(char *directory) {
    DIR *dir = opendir(directory);

    if (dir) {
        closedir(dir);
        return 0;
    } else {
        return 1;
    }
}

int howManyPackets(int fileDescriptor) {
    int fileSize = lseek(fileDescriptor, 0, SEEK_END);
    int packets = fileSize / MAXLINE;
    if (fileSize % MAXLINE)
        packets++;
    return packets;
}

void alarmHandler(int signo) {
    attempt++;
    alarmTrigger = 1;
}

void lastPacketAlarmHandler(int signo) {
    printf("Il messaggio di conferma di fine upload non è arrivato in tempo. La connessione relativa verrà chiusa.\n");
    exit(EXIT_SUCCESS);
}

int setTimeout() {
    int currentTimeout;

    if (adaptiveTimeout == 0)
        return timeout;
    else {
        if (attempt == 0) {
            currentTimeout = 1;
        } else {
            currentTimeout = 1 * attempt * 2;
        }
        return currentTimeout;
    }
}

int calculateLoss() {
    int randomPercentage = rand() % 100;
    if (lossRate > randomPercentage)
        return 0; // perdi il pacchetto
    else
        return 1; // invia il pacchetto
}

int createConnectionSocket() {
    int connectedSocket;
    struct sockaddr_in connectedSocketAddress;

    memset((void *) &connectedSocketAddress, 0, sizeof(connectedSocketAddress));
    connectedSocketAddress.sin_family = AF_INET;
    connectedSocketAddress.sin_addr.s_addr = htonl(
            INADDR_ANY); // il server accetta pacchetti su una qualunque delle sue interfacce di rete
    connectedSocketAddress.sin_port = htons(0); // nel bind verrà scelta una porta disponibile

    // crea un nuovo socket e ne fa il bind su di una porta disponibile.
    // Il nuovo socket verrà utilizzato per gestire la richiesta del client
    if ((connectedSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Errore nella creazione del socket per il trasfermiento");
        exit(EXIT_FAILURE);
    }

    // assegna l'indirizzo al socket
    if (bind(connectedSocket, (struct sockaddr *) &connectedSocketAddress, sizeof(connectedSocketAddress)) < 0) {
        perror("errore in bind");
        exit(EXIT_FAILURE);
    }

    return connectedSocket;
}

struct sockaddr_in extractConnectionSocketAddress(int connectedSocket) {
    struct sockaddr_in connectedSocketAddress;

    // indirizzo del socket a cui il client dovrà rivolgere la richiesta
    socklen_t connectedSocketAddressLength = sizeof(connectedSocketAddress);
    if (getsockname(connectedSocket, (struct sockaddr *) &connectedSocketAddress, &connectedSocketAddressLength) ==
        -1) {
        perror("Errore nell'ottenimento dell'indirizzo di comunicazione.");
        close(connectedSocket);
        exit(EXIT_FAILURE);
    }

    return connectedSocketAddress;
}

void setTimeoutHandler() { // imposta segnale per il timeout

    struct sigaction act;
    act.sa_handler = alarmHandler;
    sigfillset(&act.sa_mask);
    sigaction(SIGALRM, &act, 0);
}

void sendLastPacket(char *message, int sockfd, struct sockaddr_in clientAddress) {
    // invio pacchetto d'errore e termino la richiesta.

    struct connectionPacket packet;

    strcpy(packet.data, message);
    packet.type = htonl(-1); // Errore
    alarmTrigger = 0;
    alarm(LAST_TIMEOUT);
    while (1) {
        sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) &clientAddress, sizeof(clientAddress));
        if (alarmTrigger) {
            printf("Non è stato possibile risolvere la richiesta.\n");
            alarmTrigger = 0;
            return;
        }
    }
}

void closeServerDownloadConnection(int sockfd, struct goBackNPacket packet, struct sockaddr_in connectedSocketAddress,
                                   struct sockaddr_in address) {
    alarmTrigger = 0;
    alarm(LAST_TIMEOUT);

    while (1) {
        // inviamo l'ultimo ack finchè non arriva un messaggio di conferma
        sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) &connectedSocketAddress,
               sizeof(connectedSocketAddress));
        if (alarmTrigger) { // è scaduto il timeout
            alarmTrigger = 0;
            return;
        }
        sleep(1);
    }
}

void closeServerUploadConnection(int sockfd, int connectedSocket, struct goBackNPacket packet,
                                 struct sockaddr_in connectedSocketAddress, struct sockaddr_in address) {
    struct goBackNPacket shutdownPacket;
    socklen_t receiveAddressLength = sizeof(address);
    alarmTrigger = 0;
    alarm(LAST_TIMEOUT);

    while (1) {
        // inviamo l'ultimo ack finchè non arriva un messaggio di conferma o non scade il timeout
        sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) &connectedSocketAddress,
               sizeof(connectedSocketAddress));
        if (recvfrom(connectedSocket, &shutdownPacket, sizeof(shutdownPacket), MSG_DONTWAIT,
                     (struct sockaddr *) &address, &receiveAddressLength) > 0) {
            if (ntohl(shutdownPacket.type) == 0 && ntohl(shutdownPacket.sequenceNumber) == numberOfPackets) {
                return;
            }
        }

        if (alarmTrigger) { // è scaduto il timeout
            alarmTrigger = 0;
            return;
        }
        sleep(1);
    }
}