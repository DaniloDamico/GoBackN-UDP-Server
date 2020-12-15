#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <time.h>

// global macro
#define MAXLINE 1024
#define MAX_ATTEMPTS 16
#define LAST_TIMEOUT 10

// variabili globali
int windowSize;
int lossRate;
int timeout;
int serverPort = 8080;
int adaptiveTimeout = 0; // 1: timeout adattivo, 0: timeout fisso

int attempt = 0;
int alarmTrigger;
int sequenceBase;
int numberOfPackets = -1; // in modo che sia impossibile e fallisca il controllo sul completamento della trasmissione se non arrivano dati

// pacchetto utilizzato per stabilire la connessione
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

// pacchetto utilizzato per trasmettere dati
struct goBackNPacket {
    int type; //-1: Errore
    // 0: ACK
    // 1: Dati
    // 2: Comando & ACK Comando

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

void alarmHandler(int signo) {
    attempt++;
    alarmTrigger = 1;
}

int howManyPackets(int fileDescriptor) {
    int fileSize = lseek(fileDescriptor, 0, SEEK_END);
    int packets = fileSize / MAXLINE;
    if (fileSize % MAXLINE)
        packets++;
    return packets;
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
        return 0; // il mittente perde il pacchetto
    else
        return 1; // il mittente invia il pacchetto
}

struct sockaddr_in
calculateConnectedSocketAddress(struct sockaddr_in serverAddress, struct connectionPacket connectionData) {
    struct sockaddr_in connectedSocketAddress;

    memset((void *) &connectedSocketAddress, 0, sizeof(connectedSocketAddress));
    memcpy(&connectedSocketAddress, &serverAddress, sizeof(serverAddress));
    connectedSocketAddress.sin_port = connectionData.serverPort; // porta indicata

    return connectedSocketAddress;
}

void setTimeoutHandler() { // imposta segnale per il timeout

    struct sigaction act;
    act.sa_handler = alarmHandler;
    sigfillset(&act.sa_mask);
    sigaction(SIGALRM, &act, 0);
}

void closeClientDownloadConnection(int sockfd, struct goBackNPacket packet, struct sockaddr_in connectedSocketAddress,
                                   struct sockaddr_in address) {
    struct goBackNPacket shutdownPacket;
    socklen_t receiveAddressLength = sizeof(address);
    alarmTrigger = 0;
    if (fork() == 0) { // un processo figlio si occupa di inviare l'ultimo ack
        alarm(LAST_TIMEOUT);

        while (!alarmTrigger) {
            // inviamo l'ultimo ack finchè non arriva un messaggio di conferma
            sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) &connectedSocketAddress,
                   sizeof(connectedSocketAddress));
            if (recvfrom(sockfd, &shutdownPacket, sizeof(shutdownPacket), MSG_DONTWAIT, (struct sockaddr *) &address,
                         &receiveAddressLength) > 0) {
                if (ntohl(shutdownPacket.type) == 0 && ntohl(shutdownPacket.sequenceNumber) == numberOfPackets) {
                    close(sockfd);
                    exit(EXIT_SUCCESS);
                }
            }
        }
        close(sockfd);
        exit(EXIT_SUCCESS);
    }
}

void closeClientUploadConnection(int sockfd, struct goBackNPacket packet, struct sockaddr_in connectedSocketAddress) {
    if (fork() == 0) {
        alarmTrigger = 0;
        alarm(LAST_TIMEOUT);

        while (!alarmTrigger) {
            sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) &connectedSocketAddress,
                   sizeof(connectedSocketAddress));
        }
        close(sockfd);
        exit(EXIT_SUCCESS);
    }
}

int createSocket() {
    int sockfd;

    // crea il socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Creazione del socket fallita.");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}