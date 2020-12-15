void getServer(char *directory, char *filename, int sockfd, struct sockaddr_in clientAddress) {
    int fileDescriptor;

    char error[] = "non è stato possibile aprire il file specificato";
    char errorExistance[] = "il file specificato non è disponibile sul server";
    char errorAllowed[] = "l'accesso al file richiesto non è consentito";

    chdir(directory);
    fileDescriptor = open(filename, O_RDONLY);

    if (fileDescriptor == -1) {
        if (errno == ENOENT) {
            printf("%s\n", errorExistance);
            sendLastPacket(errorExistance, sockfd, clientAddress);
            return;
        } else if (errno == EACCES) {
            printf("%s\n", errorAllowed);
            sendLastPacket(errorAllowed, sockfd, clientAddress);
            return;
        } else {
            printf("%s\n", error);
            sendLastPacket(error, sockfd, clientAddress);
            return;
        }
    } else {
        // calcolo numberOfPackets
        numberOfPackets = howManyPackets(fileDescriptor);

        // creo il socket connesso e salvo l'indirizzo della porta su cui è in ascolto per comunicarlo al client
        int connectedSocket = createConnectionSocket();
        struct sockaddr_in connectedSocketAddress = extractConnectionSocketAddress(connectedSocket);

        // trasferimento dati
        serverDownload(fileDescriptor, sockfd, clientAddress /*indirizzo cui inviare dati*/, connectedSocket,
                       connectedSocketAddress /* indirizzo su cui ricevere dati*/);
    }
}

