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
#include <string.h>

static ApplicationStatus applicationStatus;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [0 - Transmitter OR 1 - Receiver] [Port Number] [Filename IF Transmitter]\n", argv[0]);
        return -1;
    }

    ApplicationStatus status = atoi(argv[1]);

    if ((status != TRANSMITTER && status != RECEIVER) || (status == TRANSMITTER && argc != 4) || (status == RECEIVER && argc != 3)) {
        fprintf(stderr, "Usage: %s [0 - Transmitter OR 1 - Receiver] [Port Number] [Filename IF Transmitter]\n", argv[0]);
        return -1;
    }

    int port = atoi(argv[2]);

    int pathLength;
    if ((status == TRANSMITTER) && ((pathLength = strlen(argv[3])) > MAX_PATH_SIZE - 1)) {
        fprintf(stderr, "Path length to large\n");
        return -1;
    }

    int fd;
    if ((fd = llopen(port, status)) < 0) {
        fprintf(stderr, "llopen(): Error when opening the serial port\n");
        return -1;
    }

    if (applicationStatus == TRANSMITTER) {
        if (transmitFile(fd, argv[3], pathLength) < 0) {
            fprintf(stderr, "transmitFile(): Error when transmitting the file\n");
            return -1;
        }
    } else {
        if (receiveFile(fd)) {
            fprintf(stderr, "recieveFile(): Error when receiving file\n");
            return -1;
        }
    }

    if (llclose(fd) < 0) {
        fprintf(stderr, "llclose(): Error when closing the serial port\n");
        return -1;
    }

    return 0;
}

int llopen(int port, ApplicationStatus status) {
    applicationStatus = status;
    return applicationStatus == TRANSMITTER ? connectTransmitter(port) : connectReceiver(port);
}

int llwrite(int fd, char *buffer, int length) {
    if (applicationStatus == RECEIVER) return -1;
    return transmitPacket(fd, 3, 3, buffer, length);
}

int llread(int fd, char *buffer) {
    if (applicationStatus == TRANSMITTER) return -1;
    return recievePacket(fd, 3, 20, buffer);
}

int llclose(int fd) {
    return applicationStatus == TRANSMITTER ? disconnectTransmitter(fd) : disconnectReceiver(fd);
}

int transmitFile(int fd, char *filePath, int pathLength) {
    
    // Opening file
    int file = open(filePath, O_RDONLY);
    if (file < 0) return -1;

    struct stat st;
    if (fstat(file, &st) < 0) return -1;

    char fileName[MAX_PATH_SIZE], data[MAX_DATA_SIZE], packet[MAX_PACKET_SIZE];                                   // TODO: Change fileName size so that it can't be overflowed in pathToFilename's strcpy()
    int fileNameLength = pathToFilename(fileName, filePath, pathLength);

    // Creating and transmitting start control packet
    int packetLength = createControlPacket(packet, C_START, st.st_size, fileName, fileNameLength);        // TODO: Change filePath to the name of the file (create a function that reveives a path and returns the file name perhaps?)
    if (llwrite(fd, packet, packetLength) < 0) return -1;

    int dataLength, n = 0;
    while ((dataLength = read(file, data, MAX_DATA_SIZE)) != 0) {
        packetLength = createDataPacket(packet, n++, data, dataLength);
        if (llwrite(fd, packet, packetLength) < 0) return -1;
    }

    // Creating and transmitting start control packet
    packetLength = createControlPacket(packet, C_END, st.st_size, fileName, fileNameLength);
    if (llwrite(fd, packet, packetLength) < 0) return -1;

    // Closing file
    if (close(file) < 0) return -1;

    return 0;
}

int receiveFile(int fd) {
    char packet[MAX_PACKET_SIZE];
    int packetLength;
    
    // Reading start control packet
    if ((packetLength = llread(fd, packet)) < 0) return -1;
    if (packet[0] != C_START) return -1;

    // Parse data from control packet
    int fileSize;
    char fileName[MAX_PATH_SIZE];
    if ((fileSize = parseControlPacket(packet, packetLength, fileName)) < 0) return -1;

    // Opening file
    int file = open(fileName, O_WRONLY | O_TRUNC | O_CREAT, 0777);
    if (file < 0) return -1;

    int k, stop = 0;
    while (!stop) {
        if ((packetLength = llread(fd, packet)) < 0) return -1;
        if (packet[0] == C_DATA) {
            // TODO: DO SMTHING WITH N
            // TODO: ASSERT K == LENGTH - 4
            if (write(file, packet + 4, packetLength - 4) < 0) return -1;
        } else if (packet[0] == C_END) {
            stop = 1;
        } else {
            return -1;
        }
    }

    // Closing file
    if (close(file) < 0) return -1;

    return 0;
}