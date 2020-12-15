struct connectionPacket *sendCommand(int listeningSocket, struct sockaddr_in serverAddress, char *command) {
    struct connectionPacket *packet = malloc(sizeof(struct connectionPacket));
    socklen_t addressLength = sizeof(serverAddress);
    int currentTimeout;

    sendCommand:

    // invio il comando list al server
    packet->type = htonl(0); // comando
    packet->windowSize = htonl(windowSize);
    packet->lossRate = htonl(lossRate);
    packet->timeout = htonl(timeout);
    packet->numberOfPackets = htonl(numberOfPackets); // lo considero solo per il comando put
    packet->adaptiveTimeout = htonl(adaptiveTimeout);
    strcpy(packet->data, command);
    // introduciamo una perdita pacchetti
    if (calculateLoss()) {
        sendto(listeningSocket, packet, sizeof(struct connectionPacket), 0, (struct sockaddr *) &serverAddress,
               sizeof(serverAddress));
    } else {
        printf("Pacchetto scartato.\n");
    }
    currentTimeout = setTimeout();
    alarm(currentTimeout);

    waitResponse:

    // aspetto risposta con nuova porta su cui inviare dati e altri metadati
    while (recvfrom(listeningSocket, packet, sizeof(struct connectionPacket), MSG_DONTWAIT,
                    (struct sockaddr *) &serverAddress, &addressLength) <= 0) {
        if (alarmTrigger) {
            alarm(0);
            alarmTrigger = 0;
            if (attempt < MAX_ATTEMPTS) {
                goto sendCommand;
            } else {
                printf("La connessione con il server è instabile. L'operazione put è stata annullata.\n");
                return NULL;
            }
        }
    }

    if (ntohl(packet->type) == -1) {
        alarm(0);
        printf("%s\n", packet->data);
        return NULL;
    }

    if (ntohl(packet->type != 0)) {
        goto waitResponse;
    }

    attempt = 0;
    return packet;
}
