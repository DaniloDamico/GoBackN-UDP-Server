void listClient(int listeningSocket, struct sockaddr_in serverAddress) {
    char buffer[MAXLINE] = {0};
    char list[] = "list ";
    struct connectionPacket *connectionData = sendCommand(listeningSocket, serverAddress, list);

    if (connectionData == NULL)
        return;

    //creo un file temporaneo in cui verrà scritto il contenuto ricevuto
    char template[] = "/tmp/list_client_temp_file_XXXXXX";
    int fileDescriptor = mkstemp(template);

    struct sockaddr_in connectedSocketAddress = calculateConnectedSocketAddress(serverAddress, *connectionData);
    numberOfPackets = ntohl(connectionData->numberOfPackets);

    if (clientDownload(listeningSocket, serverAddress/*indirizzo su cui ricevo pacchetti*/,
                       connectedSocketAddress/*indirizzo cui inviare ACK*/, fileDescriptor)) {

        // stampiamo il contenuto del file temporaneo
        lseek(fileDescriptor, 0, SEEK_SET);

        printf("Elenco dei file disponibili sul server:\n");
        while (read(fileDescriptor, buffer, MAXLINE)) {
            printf("%s", buffer);
            memset(buffer, 0x0, MAXLINE);
        }
    } else {
        printf("non è stato possibile completare la richiesta.\n");
    }

    close(fileDescriptor);
}