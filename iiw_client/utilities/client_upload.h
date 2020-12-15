int
clientUpload(int fileDescriptor, int sockfd, struct sockaddr_in connectedSocketAddress /*indirizzo cui inviare dati*/,
             struct sockaddr_in address /* indirizzo su cui ricevere dati*/) {
    struct goBackNPacket packet;
    socklen_t addressLength = sizeof(address);
    int lastPacketReceived = -1;
    ssize_t bytesRead;
    int sequenceNumberReceived, currentTimeout;

    alarmTrigger = 1;
    sequenceBase = 0;

    while ((lastPacketReceived < (numberOfPackets - 1)) && (attempt < MAX_ATTEMPTS)) {

        // invio dati
        sendPackets:

        if (alarmTrigger) {
            alarmTrigger = 0;
            // invia pacchetti fino a riempire la finestra
            for (int i = 0; i < windowSize; i++) {
                if (sequenceBase + i <
                    numberOfPackets) { // controllo di non andare oltre il numero previsto di pacchetti
                    memset(&packet, 0, sizeof(packet));

                    packet.type = htonl(1); // Dati
                    packet.sequenceNumber = htonl(sequenceBase + i);
                    lseek(fileDescriptor, (sequenceBase + i) * MAXLINE, SEEK_SET);
                    bytesRead = read(fileDescriptor, packet.data, MAXLINE);
                    packet.dataLength = htonl(bytesRead);
                    // introduciamo una perdita pacchetti
                    if (calculateLoss()) {
                        printf("Invio il pacchetto %d di %d.\n", sequenceBase + i + 1, numberOfPackets);
                        sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) &connectedSocketAddress,
                               sizeof(connectedSocketAddress));
                    } else {
                        printf("Pacchetto scartato.\n");
                    }
                }
            }
        }

        // ricezione ACK
        waitACK:

        currentTimeout = setTimeout();
        alarm(currentTimeout);

        memset(&packet, 0, sizeof(packet));
        while (recvfrom(sockfd, &packet, sizeof(packet), MSG_DONTWAIT, (struct sockaddr *) &address, &addressLength) <=
               0) { // attesa risposta
            if (alarmTrigger) { // alarmTrigger è 1 se è scaduto il timeout
                alarm(0);
                if (attempt < MAX_ATTEMPTS) {
                    sequenceBase = lastPacketReceived + 1; // inizio a inviare dall'ultimo pacchetto ricevuto
                    goto sendPackets;
                } else {
                    printf("La connessione con il server è instabile. L'operazione è stata annullata.\n");
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

        } else { goto waitACK; }

    }
    // ho ricevuto l'ack dell'ultimo pacchetto

    memset(&packet, 0, sizeof(packet));
    packet.type = htonl(0);
    packet.sequenceNumber = htonl(numberOfPackets);

    // la trasmissione è stata completata
    closeClientUploadConnection(sockfd, packet, connectedSocketAddress);
    return 1;
}
