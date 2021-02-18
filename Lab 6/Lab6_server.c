/*
* Name: Ethan Paek
* Date: 5/5/2020
* Title: Lab 6 - Stop and Wait for an Unreliable Channel, with Loss
* Description: This program runs the server program for an rdt3.0 receiver
*/

//Explain what each library does in the context of this lab. Do we need all of them?
#include <arpa/inet.h> // necessary for sockaddr_in
#include <stdio.h> // necessary for perror
#include <stdlib.h> // necessary for EXIT_FAILURE
#include <string.h> // necessary for strlen
#include <sys/socket.h> // necessary for sockaddr and socklen_t
#include <time.h> // necessary for rand
#include <unistd.h> // necessary for write and close
#include <fcntl.h> // necessary for O_RDWR
#include "lab6.h"

//add the missing functions

//******STUDENT WORK******
int getChecksum(Packet packet) {
    packet.header.cksum = 0;

    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;

    while (ptr < end) {
        checksum ^= *ptr++;
    }

    return checksum;
}

//******STUDENT WORK******
void logPacket(Packet packet) {
    printf("Packet { header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
           packet.header.seq_ack,
           packet.header.len,
           packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

void ServerSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    // Simulating a chance that ACK gets lost
    if (rand() % PLOSTMSG == 0) {
        //******STUDENT WORK******
        printf("Packet lost\n");
        return;
    }
    //prepare and send the ACK
    Packet packet;
    //******STUDENT WORK******
    packet.header.seq_ack = seqnum;
    packet.header.len = sizeof(packet.data);
    packet.header.cksum = getChecksum(packet);
    sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);

    printf("Sent ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
}

Packet ServerReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) {
                     
    Packet packet;

    while (1) {
        //Receive a packet from the client
        //******STUDENT WORK******
        recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);

        // validate the length of the packet
        //******STUDENT WORK******
        if(packet.header.len != strlen(packet.data)){
            printf("Packet lengths mismatch\n");
            break;
        }

        // log what was received
        printf("Received: ");
        logPacket(packet);

        //verify the checksum and the sequence number
        if (packet.header.cksum != getChecksum(packet)) {
            printf("Bad checksum, expected %d\n", getChecksum(packet));
            //******STUDENT WORK******
            ServerSend(sockfd, address, *addrlen, !seqnum);
        }
        else if (packet.header.seq_ack != seqnum) {
            printf("Bad seqnum, expected %d\n", seqnum);
            //******STUDENT WORK******
            ServerSend(sockfd, address, *addrlen, !seqnum);
        }
        else {
            printf("Good packet\n");
            //******STUDENT WORK******
            ServerSend(sockfd, address, *addrlen, seqnum);
            break;
        }
    }
    printf("\n");
    return packet;
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        return EXIT_FAILURE;
    }

    puts("Waiting for client...");

    // seed the RNG
    srand((unsigned)time(NULL));

    // create a socket
    //******STUDENT WORK******
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    // initialize the server address structure
    //******STUDENT WORK******
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket
    //******STUDENT WORK******
    if ((bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr))) < 0){
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    // Please answer the following questions:
        // [Q1] Briefly explain what is "AF_INET" used for.
        // AF_INET is an address family used to designate the type of addresses that a socked can communicate with.

        // [Q2] Briefly explain what is "SOCK_DGRAM" used for.
        // "SOCK_DGRAM" is used to send packets with their own addresses unreliably; it's a datagram protocol.
        // It is somewhat opposite of SOCK_STREAM.

        // [Q3] Briefly explain what is "htons" used for.
        // "htons" is used to change a number from host-byte order to network-byte order in so it can be used in TCP networks

        // [Q4] Briefly explain why bind is required on the server and not on the client.
        // Bind is required for the server because it allows the server to have a certain port number; we need something to "plug into".
        // However, it is not required for the client since the local address and port of a client socket is unimportant.

    // open file
    int fp = open(argv[2], O_RDWR);
    if(fp < 0){
        perror("fopen");
        return EXIT_FAILURE;
    }

    // get file contents from client and save it to the file
    int seqnum = 0;
    Packet packet;
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);
    do {
        //******STUDENT WORK******
        packet = ServerReceive(sockfd, (struct sockaddr *)&clientaddr, &clientaddr_len, seqnum);
        write(fp, packet.data, strlen(packet.data));
        seqnum = (seqnum + 1) % 2;
    } while (packet.header.len != 0);

    //cleanup
    //******STUDENT WORK******
    close(fp);
    close(sockfd);

    return 0;
}
