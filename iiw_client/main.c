#include <sys/wait.h>
#include "utilities/utilities.h"
#include "utilities/client_upload.h"
#include "utilities/client_download.h"
#include "utilities/send_command.h"
#include "client_commands/list_client.h"
#include "client_commands/get_client.h"
#include "client_commands/put_client.h"
#include "client_commands/help_client.h"
#include "client_commands/settings_client.h"

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serverAddress;
    char *filename = malloc(MAXLINE);
    char *command = malloc(MAXLINE);
    char *buffer = malloc(MAXLINE);
    size_t commandLength;
    static const char separators[] = " \n";

    if (argc != 6) { /* controlla numero degli argomenti */
        printf("utilizzo: client <indirizzo IP server> <fullpath cartella con contenuto del server> <dimensione finestra di spedizione> <probabilità perdita messaggi> <timeout>\n");
        exit(EXIT_FAILURE);
    }

    char *clientDirectory = argv[2];
    if (checkDirectory(clientDirectory) != 0) {
        printf("La cartella scelta non esiste o non è possibile accedervi. Il client verrà terminato.\n");
        exit(EXIT_FAILURE);
    }

    windowSize = (int) strtol(argv[3], NULL, 10);
    if (((windowSize == LONG_MIN | windowSize == LONG_MAX) && errno == ERANGE) | (errno == EINVAL) | (windowSize < 1)) {
        printf("La dimensione della finestra di spedizione deve essere un intero positivo. Il client verrà terminato\n");
        exit(EXIT_FAILURE);
    }

    lossRate = (int) strtol(argv[4], NULL, 10);
    if (((lossRate == LONG_MIN | lossRate == LONG_MAX) && errno == ERANGE) | (errno == EINVAL) | (lossRate < 0) |
        (lossRate > 100)) {
        printf("La probabilità di perdita pacchetto deve essere un intero compreso tra 0 e 100. Il client verrà terminato\n");
        exit(EXIT_FAILURE);
    }

    timeout = (int) strtol(argv[5], NULL, 10);
    if (((timeout == LONG_MIN | timeout == LONG_MAX) && errno == ERANGE) | (errno == EINVAL)) {
        printf("Il timeout deve essere un numero intero. Il client verrà terminato\n");
        exit(EXIT_FAILURE);
    }

    // random seed
    srand(time(NULL));

    // imposta timeout
    setTimeoutHandler();

    memset((void *) &serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET; // assegna il tipo di indirizzo
    serverAddress.sin_port = htons(serverPort); // assegna la porta del server

    // assegna l'indirizzo del server prendendolo dalla riga di comando. L'indirizzo è
    // una stringa da convertire in intero
    if (inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) <= 0) {
        perror("Conversione dell'indirizzo del server fallita.");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Inserisci un comando o digita 'help' per la lista dei comandi disponibili.\n");
        memset(buffer, 0, MAXLINE);
        memset(filename, 0, MAXLINE);
        memset(command, 0, MAXLINE);

        //fgets consuma l'intero input digitato dall'utente
        fflush(stdout);
        while (fgets(buffer, MAXLINE, stdin) == NULL);

        //rimuovo il carattere '\n' dalla fine della stringa
        buffer[strlen(buffer) - 1] = '\0';

        // comando da eseguire
        strcpy(command, strtok(buffer, separators));
        commandLength = strlen(command);

        // nome del file interessato
        strcpy(filename, &buffer[commandLength + 1]);


        // imposto le variabili globali sui valori di inizio comando
        attempt = 0;
        alarmTrigger = 0;
        sequenceBase = 0;

        if (strcmp(command, "list") == 0) {
            sockfd = createSocket();
            listClient(sockfd, serverAddress);

        } else if (strcmp(command, "get") == 0) {
            sockfd = createSocket();
            getClient(sockfd, serverAddress, clientDirectory, filename);

        } else if (strcmp(command, "put") == 0) {
            sockfd = createSocket();
            putClient(sockfd, serverAddress, clientDirectory, filename);

        } else if (strcmp(command, "help") == 0) {
            helpClient();

        } else if (strcmp(command, "settings") == 0) {
            settingsClient();

        } else if (strcmp(command, "shutdown") == 0) {
            printf("Il client sta per essere terminato.\n");
            free(command);
            free(filename);
            free(buffer);
            exit(EXIT_SUCCESS);
        } else {
            printf("Comando non valido.\n");
        }
    }
}
