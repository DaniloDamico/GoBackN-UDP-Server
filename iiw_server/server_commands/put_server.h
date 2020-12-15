void putServer(char *directory, char *filename, int sockfd, struct sockaddr_in clientAddress) {
    int fileDescriptor;
    char error[] = "la creazione del file richiesto è fallita. Trasferimento annullato.";

    chdir(directory);
    if ((fileDescriptor = open(filename, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) == -1) {
        printf("%s.\n", error);
        sendLastPacket(error, sockfd, clientAddress);
        return;
    }

    // creo il socket connesso e salvo l'indirizzo della porta su cui è in ascolto per comunicarlo al client
    int connectedSocket = createConnectionSocket();
    struct sockaddr_in connectedSocketAddress = extractConnectionSocketAddress(connectedSocket);

    if (serverUpload(sockfd, connectedSocket, connectedSocketAddress, clientAddress, fileDescriptor) == 1)
        printf("upload completato.\n");
}
