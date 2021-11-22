#include "../headers/application.h"
#include "../headers/transmitter.h"
#include "../headers/receiver.h"
#include "../headers/auxiliary.h"
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [0-Transmitter OR 1-Receiver] [Port Number]\n", argv[0]);
        return -1;
    }

    ApplicationStatus status = atoi(argv[1]);       // TODO: Change atoi() to strtoi() to check for errors
    int port = atoi(argv[2]);

    int fd;
    if ((fd = llopen(port, status)) < 0) return -1;

    //if ()

    if (llclose(fd, status) < 0) return -1;

    return 0;
}

int transmitFile(int fd, char *filePath, int pathLength) {
    
    // Opening file
    int file = open(filePath, O_RDONLY);
    if (file < 0) return -1;

    struct stat st;
    if (fstat(file, &st) < 0) return -1;

    char data[MAX_DATA_SIZE], packet[MAX_PACKET_SIZE];

    char fileName[100];
    int fileNameLength = pathToFilename(fileName, filePath, pathLength);

    // Creating and transmitting start control packet
    int packetSize = createControlPacket(packet, C_START, st.st_size, fileName, fileNameLength);        // TODO: Change filePath to the name of the file (create a function that reveives a path and returns the file name perhaps?)
    if (llwrite(fd, packet, packetSize) < 0) return -1;

    int dataLength, n = 0;
    while ((dataLength = read(fd, data, MAX_DATA_SIZE)) != 0) {
        packetSize = createDataPacket(packet, n++, data, dataLength);
        if (llwrite(fd, packet, packetSize) < 0) return -1;
    }

    // Creating and transmitting start control packet
    packetSize = createControlPacket(packet, C_END, st.st_size, fileName, fileNameLength);
    if (llwrite(fd, packet, packetSize) < 0) return -1;

    // Closing file

    return 0;
}

int receiveFile(int fd) {

}

int llopen(int port, ApplicationStatus status) {
    return status == TRANSMITTER ? connectTransmitter(port) : connectReceiver(port);
}

int llwrite(int fd, char *buffer, int length) {
/*
    // Create start control packet
    char packet[MAX_PACKET_SIZE], *ptr = buffer;
    int packetSize = createControlPacket(packet, C_START, length, "pinguim.gif", 12), n = 0, dataLength;

    // Transmit packet
    if (transmitPacket(fd, packet, packetSize) < 0) return -1;

    while (length > 0) {
        dataLength = length >= MAX_DATA_SIZE ? MAX_DATA_SIZE : length;
        packetSize = createDataPacket(packet, n, ptr, dataLength);
        if (transmitPacket(fd, packet, packetSize) < 0) return -1;
        length -= dataLength;
        ptr += dataLength;
    }

    packetSize = createControlPacket(packet, C_START, length, "pinguim.gif", 12);
    
*/
    // para cada fragmento do buffer
        // createData
        //transmit
        // se erro, -1
        //length -= 508
        // ptr += 508
    
    // mandar end control packet
    // se erro, -1


    return transmitPacket(fd, buffer, length);
}

int llread(int fd, char *buffer) {

    // read start control packet
    // se erro, -1

    //int length;
    //char *ptr
    // para cada fragmento do buffer
        // if c == 3, verifica e acaba
        // ptr += k
        // update length

    // return length
    return recievePacket(fd, buffer);
}

int llclose(int fd, ApplicationStatus status) {
    return status == TRANSMITTER ? disconnectTransmitter(fd) : disconnectReceiver(fd);
}