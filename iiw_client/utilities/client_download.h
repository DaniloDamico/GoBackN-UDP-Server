int clientDownload(int sockfd, struct sockaddr_in address/*indirizzo su cui ricevo pacchetti*/,
                   struct sockaddr_in connectedSocketAddress/*indirizzo cui inviare ACK*/, int fileDescriptor) {

    if (numberOfPackets == 0) {
        return 1;
    }

    struct goBackNPacket packet;

    socklen_t receiveAddressLength = sizeof(address);
    int lastPacketReceived = -1;

    while (1) {
        memset(&packet, 0, sizeof(packet));
        // attende finchè non arriva un pacchetto
        recvfrom(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) &address, &receiveAddressLength);
        printf("ricevuto.\n");
        if (ntohl(packet.sequenceNumber) == (lastPacketReceived + 1)) {
            lastPacketReceived++;
            write(fileDescriptor, packet.data, ntohl(packet.dataLength));
        }

        // ACK
        memset(&packet, 0, sizeof(packet));
        packet.type = htonl(0);
        packet.sequenceNumber = htonl(lastPacketReceived);

        if (ntohl(packet.sequenceNumber) == (numberOfPackets - 1)) { // ho ricevuto l'ultimo pacchetto
            printf("Ho ricevuto l'ultimo pacchetto.\n");
            closeClientDownloadConnection(sockfd, packet, connectedSocketAddress, address);
            return 1;
        }
        if (calculateLoss()) {
            printf("Invio conferma del pacchetto %d di %d.\n", lastPacketReceived + 1, numberOfPackets);
            sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) &connectedSocketAddress,
                   sizeof(connectedSocketAddress));
        } else {
            printf("Pacchetto scartato.\n");
        }

    }
}
