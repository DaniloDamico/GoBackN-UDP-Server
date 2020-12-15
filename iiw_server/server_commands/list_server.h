void listServer(char *directory, int sockfd, struct sockaddr_in address) {
    static char buffer[MAXLINE] = {0};
    size_t readLength;
    char error[] = "errore nella lettura dei file disponibili.";

    chdir(directory);

    // apro un terminale in cui eseguire il comando ls e scrivendo l'output in filesList
    // popen crea una pipe, pertanto dovrò copiarne i contenuti in un file temporaneo se voglio usare fseek()
    FILE *filesList = popen("ls", "r");

    if (filesList == NULL) {
        fprintf(stderr, "%s\n", error);
        sendLastPacket(error, sockfd, address);
        return;
    }

    // copio filesList in un file temporaneo
    char template[] = "/tmp/list_server_temp_file_XXXXXX";
    int fileDescriptor = mkstemp(template);

    while ((readLength = fread(buffer, 1, MAXLINE, filesList)) != 0) {
        write(fileDescriptor, buffer, readLength);
        memset(buffer, 0x0, MAXLINE);
    }

    // calcolo numberOfPackets
    numberOfPackets = howManyPackets(fileDescriptor);

    // creo il socket connesso e salvo l'indirizzo della porta su cui è in ascolto per comunicarlo al client
    int connectedSocket = createConnectionSocket();
    struct sockaddr_in connectedSocketAddress = extractConnectionSocketAddress(connectedSocket);

    // trasferimento dati
    serverDownload(fileDescriptor, sockfd, address /*indirizzo cui inviare dati*/, connectedSocket,
                   connectedSocketAddress /* indirizzo su cui ricevere dati*/);
}