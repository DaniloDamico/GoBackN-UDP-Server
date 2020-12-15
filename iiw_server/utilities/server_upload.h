int serverUpload(int sockfd, int connectedSocket, struct sockaddr_in connectedSocketAddress,
                 struct sockaddr_in clientAddress, int fileDescriptor) {
    int currentTimeout;

    if (numberOfPackets == 0) {
        return 1;
    }

    struct goBackNPacket packet;
    socklen_t receiveAddressLength = sizeof(connectedSocketAddress);
    int lastPacketReceived = -1;

    while (1) {
        memset(&packet, 0, sizeof(packet));

        // pongo alarmTrigger = 1 in modo che la prima volta che si entra nel ciclo while venga inviato l'ack del comando
        alarmTrigger = 1;

        // attende finchè non arriva un pacchetto
        while (recvfrom(connectedSocket, &packet, sizeof(packet), MSG_DONTWAIT,
                        (struct sockaddr *) &connectedSocketAddress, &receiveAddressLength) <= 0) {
            if (alarmTrigger) {
                alarmTrigger = 0;

                sendCommandACK(connectedSocketAddress, clientAddress, sockfd);
                // timeout per assicurarmi che il command ack sia arrivato
                currentTimeout = setTimeout();
                alarm(currentTimeout);
            }
        }

        alarm(0); // è arrivato almeno un messaggio: questo significa che l'ack del comando è arrivato a destinazione

        if (ntohl(packet.sequenceNumber) == (lastPacketReceived + 1)) {

            lastPacketReceived++;
            //printf("Dati ricevuti:%s\n", packet.data);
            write(fileDescriptor, packet.data, ntohl(packet.dataLength));
        }

        // ACK
        memset(&packet, 0, sizeof(packet));
        packet.type = htonl(0);
        packet.sequenceNumber = htonl(lastPacketReceived);

        if (ntohl(packet.sequenceNumber) == (numberOfPackets - 1)) {
            // ACK Ultimo pacchetto
            printf("L'upload è stato completato correttamente\n");

            closeServerUploadConnection(sockfd, connectedSocket, packet, connectedSocketAddress, clientAddress);
            return 1;
        }
        // introduciamo una perdita pacchetti
        if (calculateLoss()) {
            sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) &clientAddress, sizeof(clientAddress));
        } else {
            printf("Pacchetto scartato.\n");
        }
    }
}