void getClient(int listeningSocket, struct sockaddr_in serverAddress, char *clientDirectory, char *filename) {
    char command[MAXLINE] = {0x0};
    int fileDescriptor;

    strcpy(command, "get ");
    strcat(command, filename);

    if (filename == NULL) {
        perror("nome del file desiderato mancante.");
        return;

    } else {

        chdir(clientDirectory);
        if ((fileDescriptor = open(filename, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU)) == 0) {
            printf("la creazione di %s è fallita. Trasferimento annullato.\n", filename);
            return;
        }

        struct connectionPacket *connectionData = sendCommand(listeningSocket, serverAddress, command);

        if (connectionData == NULL)
            return;

        struct sockaddr_in connectedSocketAddress = calculateConnectedSocketAddress(serverAddress, *connectionData);

        numberOfPackets = ntohl(connectionData->numberOfPackets);

        if (clientDownload(listeningSocket, serverAddress/*indirizzo su cui ricevo pacchetti*/,
                           connectedSocketAddress/*indirizzo cui inviare ACK*/, fileDescriptor)) {

            printf("Download completato con successo.\n");
        } else {
            printf("non è stato possibile completare il download.\n");
        }

        free(connectionData);
        close(fileDescriptor);
    }
}
