int serverDownload(int fd, int sockfd, struct sockaddr_in address /*indirizzo cui inviare dati*/, int connectedSocket,
                   struct sockaddr_in connectedSocketAddress /* indirizzo su cui ricevere dati*/) {
    struct goBackNPacket packet;
    socklen_t addressLength = sizeof(connectedSocketAddress);
    int lastPacketReceived = -1;
    ssize_t bytesRead;
    int sequenceNumberReceived;
    int currentTimeout;

    attempt = 0;

    sendCommandACK(connectedSocketAddress, address, sockfd);

    alarmTrigger = 1;
    sequenceBase = 0;

    while ((lastPacketReceived < (numberOfPackets - 1)) && (attempt < MAX_ATTEMPTS)) {

        // invio dati
        send_packets:

        if (alarmTrigger) {
            alarmTrigger = 0;
            // invia pacchetti fino a riempire la finestra
            for (int i = 0; i < windowSize; i++) {
                if (sequenceBase + i <
                    numberOfPackets) { // controllo di non andare oltre il numero previsto di pacchetti
                    memset(&packet, 0, sizeof(packet));

                    packet.type = htonl(1); // Dati
                    packet.sequenceNumber = htonl(sequenceBase + i);
                    lseek(fd, (sequenceBase + i) * MAXLINE, SEEK_SET);
                    bytesRead = read(fd, packet.data, MAXLINE);
                    packet.dataLength = htonl(bytesRead);

                    // introduciamo una perdita pacchetti
                    if (calculateLoss()) {
                        sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) &address, sizeof(address));
                    } else {
                        printf("Pacchetto %d di %d scartato.\n", sequenceBase + i + 1, numberOfPackets);
                    }
                }
            }
        }

        // ricezione ACK
        waitACK:

        currentTimeout = setTimeout();
        alarm(currentTimeout);

        memset(&packet, 0, sizeof(packet));
        while (recvfrom(connectedSocket, &packet, sizeof(packet), MSG_DONTWAIT,
                        (struct sockaddr *) &connectedSocketAddress, &addressLength) <= 0) { // attesa risposta
            if (alarmTrigger) { // alarmTrigger è 1 se è scaduto il timeout
                alarm(0);
                if (attempt < MAX_ATTEMPTS) {
                    sequenceBase = lastPacketReceived + 1; // inizio a inviare dall'ultimo pacchetto ricevuto

                    // se non è mai arrivato un ack reinviamo la conferma del comando:
                    if (lastPacketReceived == -1)
                        sendCommandACK(connectedSocketAddress, address, sockfd);

                    goto send_packets;
                } else {
                    printf("La connessione con il client è instabile. L'operazione è stata annullata.\n");
                    return -1;
                }
            }
        }

        sequenceNumberReceived = ntohl(packet.sequenceNumber);

        //ACK di risposta ricevuto
        if (ntohl(packet.type) == 0 && sequenceNumberReceived > lastPacketReceived) {
            alarm(0);
            attempt = 0;
            lastPacketReceived = ntohl(packet.sequenceNumber);

            if ((lastPacketReceived + 1) ==
                (sequenceBase + windowSize)) { // tutti i pacchetti inviati sono stati confermati
                sequenceBase = lastPacketReceived +
                               1; // Dobbiamo iniziare ad inviare dal pacchetto successivo all'ultimo ricevuto
            }

            alarmTrigger = 1;

        } else { // abbiamo ricevuto qualcosa ma non è un ack successivo
            goto waitACK;
        }
    }

    // messaggio di conferma di fine comunicazione
    memset(&packet, 0, sizeof(packet));
    packet.type = htonl(0);
    packet.sequenceNumber = htonl(numberOfPackets);

    closeServerDownloadConnection(sockfd, packet, connectedSocketAddress, address);
    return 1;
}
