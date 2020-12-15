void putClient(int listeningSocket, struct sockaddr_in serverAddress, char *directory, char *filename) {
    char command[MAXLINE] = {0};

    char error[] = "non è stato possibile aprire il file specificato";
    char errorExistance[] = "il file specificato non è disponibile nella cartella indicata";
    char errorAllowed[] = "l'accesso al file specificato non è consentito";

    chdir(directory);
    int fileDescriptor = open(filename, O_RDONLY);

    if (fileDescriptor == -1) {
        if (errno == ENOENT) {
            printf("%s\n", errorExistance);
        } else if (errno == EACCES) {
            printf("%s\n", errorAllowed);
        } else {
            printf("%s\n", error);
        }
        return;
    }

    strcpy(command, "put ");
    strcat(command, filename);

    numberOfPackets = howManyPackets(fileDescriptor); // calcolo il numero di pacchetti da trasmettere

    struct connectionPacket *connectionData = sendCommand(listeningSocket, serverAddress, command);

    if (connectionData == NULL)
        return;

    struct sockaddr_in connectedSocketAddress = calculateConnectedSocketAddress(serverAddress, *connectionData);

    if (clientUpload(fileDescriptor, listeningSocket, connectedSocketAddress /*indirizzo cui inviare dati*/,
                     serverAddress /* indirizzo su cui ricevere dati*/) == 1) {
        printf("Upload completato con successo.\n");
    } else {
        printf("Non è stato possibile completare la richiesta.\n");
    }
}
