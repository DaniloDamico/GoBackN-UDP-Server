#include "utilities/utilities.h"
#include "utilities/server_command_ack.h"
#include "utilities/server_download.h"
#include "utilities/server_upload.h"
#include "server_commands/get_server.h"
#include "server_commands/put_server.h"
#include "server_commands/list_server.h"

int main(int argc, char *argv[]) {
    int listeningSocket;
    size_t commandLength;
    socklen_t addressLength;
    struct sockaddr_in serverAddress;
    char buffer[MAXLINE], *command, *filename;
    static const char space[] = " ";
    struct connectionPacket packet;

    // random seed
    srand(time(NULL));

    // controlla la validità degli argomenti immessi
    if (argc != 2) {
        fprintf(stderr, "utilizzo: client <fullpath cartella con contenuto del server>\n");
        exit(EXIT_FAILURE);
    }

    // controlla che la cartella per i file del server immessa esista
    char *serverDirectory = argv[1];
    if (checkDirectory(serverDirectory) != 0) {
        printf("La cartella scelta non esiste o non è possibile accedervi. Il server verrà terminato.\n");
        exit(EXIT_FAILURE);
    }

    // crea un socket UDP
    if ((listeningSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("errore in socket");
        exit(EXIT_FAILURE);
    }

    memset((void *) &serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(
            INADDR_ANY); // il server accetta pacchetti su una qualunque delle sue interfacce di rete
    serverAddress.sin_port = htons(serverPort); // numero di porta del server per prime comunicazioni
    addressLength = sizeof(serverAddress);

    // assegna l'indirizzo al socket
    if (bind(listeningSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("errore in bind");
        exit(EXIT_FAILURE);
    }

    printf("Server avviato.\n");

    while (1) {
        memset(&packet, 0, sizeof(packet));
        memset(buffer, 0, MAXLINE);

        // legge dal socket il pacchetto di risposta e l'indirizzo del mittente
        if (recvfrom(listeningSocket, &packet, sizeof(packet), 0, (struct sockaddr *) &serverAddress, &addressLength) <
            0) {
            perror("errore in recvfrom");
            exit(EXIT_FAILURE);
        }

        // controlliamo che il pacchetto sia del tipo giusto
        if (ntohl(packet.type) == 0) {
            // un processo figlio esegue la richiesta
            if (fork() == 0) {
                memcpy(buffer, packet.data, MAXLINE);

                // imposto la funzione per il timeout
                setTimeoutHandler();

                // comando ricevuto
                command = strtok(buffer, space); // nome del comando
                commandLength = strlen(command);
                filename = &buffer[commandLength + 1]; // nome del file

                // impostiamo le variabili di comunicazione scelte dal client
                numberOfPackets = ntohl(packet.numberOfPackets);
                windowSize = ntohl(packet.windowSize);
                lossRate = ntohl(packet.lossRate);
                timeout = ntohl(packet.timeout);
                adaptiveTimeout = ntohl(packet.adaptiveTimeout);

                // imposto le variabili globali sui valori di inizio comando
                attempt = 0;
                alarmTrigger = 0;
                sequenceBase = 0;

                if (strcmp(command, "list") == 0) {
                    printf("comando list richiesto.\n");
                    listServer(serverDirectory, listeningSocket, serverAddress);

                } else if (strcmp(command, "get") == 0) {
                    printf("comando get richiesto.\n");
                    getServer(serverDirectory, filename, listeningSocket, serverAddress);

                } else if (strcmp(command, "put") == 0) {
                    printf("comando put richiesto.\n");
                    putServer(serverDirectory, filename, listeningSocket, serverAddress);

                } else {
                    printf("Comando non valido.\n");
                }
                printf("operazione completata. Il processo dedicato verrà chiuso.\n");
                exit(EXIT_SUCCESS);
            }
        }
    }
}
