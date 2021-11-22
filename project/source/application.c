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

    char fileName[100], data[MAX_DATA_SIZE], packet[MAX_PACKET_SIZE];                                   // TODO: Change fileName size so that it can't be overflowed in pathToFilename's strcpy()
    int fileNameLength = pathToFilename(fileName, filePath, pathLength);

    // Creating and transmitting start control packet
    int packetLength = createControlPacket(packet, C_START, st.st_size, fileName, fileNameLength);        // TODO: Change filePath to the name of the file (create a function that reveives a path and returns the file name perhaps?)
    if (llwrite(fd, packet, packetLength) < 0) return -1;

    int dataLength, n = 0;
    while ((dataLength = read(fd, data, MAX_DATA_SIZE)) != 0) {
        packetLength = createDataPacket(packet, n++, data, dataLength);
        if (llwrite(fd, packet, packetLength) < 0) return -1;
    }

    // Creating and transmitting start control packet
    packetLength = createControlPacket(packet, C_END, st.st_size, fileName, fileNameLength);
    if (llwrite(fd, packet, packetLength) < 0) return -1;

    // Closing file
    if (close(fd) < 0) return -1;

    return 0;
}

int receiveFile(int fd) {
    int packet[MAX_PACKET_SIZE];
    int packetLength;
    
    // Reading start control packet
    if ((packetLength = llread(fd, packet)) < 0) return -1;
    if (packet[0] != C_START) return -1;

    int size;
    char fileName[100];
    for (int i = 1; i < packetLength;) {
        if (packet[i] == T_FILE_SIZE) {
            size = packet[i+2];
            i += 4;
        } else if (packet[i] == T_FILE_NAME) {
            
        }
    }
    
}

int llopen(int port, ApplicationStatus status) {
    return status == TRANSMITTER ? connectTransmitter(port) : connectReceiver(port);
}

int llwrite(int fd, char *buffer, int length) {
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