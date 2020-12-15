void sendCommandACK(struct sockaddr_in connectedSocketAddress, struct sockaddr_in addr, int sockfd) {
    struct connectionPacket packet;

    memset(&packet, 0, sizeof(packet));
    packet.type = htonl(0);
    packet.numberOfPackets = htonl(numberOfPackets); // considerato dal client solo per le funzioni put e get
    packet.serverPort = connectedSocketAddress.sin_port;
    // introduciamo una perdita pacchetti
    if (calculateLoss()) {
        sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) &addr, sizeof(addr));
    } else {
        printf("Pacchetto scartato.\n");
    }
}
