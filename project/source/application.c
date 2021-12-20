#include "../headers/application.h"
#include "../headers/interface.h"
#include "../headers/auxiliary.h"
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

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

    if (status == TRANSMITTER) {
        if (transmitFile(fd, argv[3], pathLength) < 0) {
            fprintf(stderr, "transmitFile(): Error when transmitting the file\n");
            return -1;
        }
    } else {
        if (receiveFile(fd)) {
            fprintf(stderr, "receiveFile(): Error when receiving file\n");
            return -1;
        }
    }

    if (llclose(fd) < 0) {
        fprintf(stderr, "llclose(): Error when closing the serial port\n");
        return -1;
    }

    return 0;
}

int transmitFile(int fd, char *filePath, int pathLength) {
    
    // Opening file
    int file = open(filePath, O_RDONLY);
    if (file < 0) return -1;

    // Getting struct with file attributes
    struct stat st;
    if (fstat(file, &st) < 0) ABORT(file);

    // Getting file name from file path
    char fileName[MAX_PATH_SIZE], data[MAX_DATA_SIZE], packet[MAX_PACKET_SIZE];
    int fileNameLength = pathToFilename(fileName, filePath, pathLength);

    // Creating and transmitting start control packet
    int packetLength = createControlPacket(packet, C_START, st.st_size, fileName, fileNameLength);
    if (llwrite(fd, packet, packetLength) < 0) ABORT(file);
    
    printf("> Transmitted START packet\n");
    printControlPacket(packet);

    // Calculating number of packets to be sent
    int numberPackets = st.st_size / MAX_DATA_SIZE;
    if (st.st_size % MAX_PACKET_SIZE > 0) ++numberPackets;

    for (int i = 0, dataLength; i < numberPackets; ++i) {
        if ((dataLength = read(file, data, MAX_DATA_SIZE)) <= 0) ABORT(file);   // If read fails or reaches EOF
        packetLength = createDataPacket(packet, i, data, dataLength);
        if (llwrite(fd, packet, packetLength) < 0) ABORT(file);

        printf("> Transmitted DATA packet [%d/%d]\n", i+1, numberPackets);
    }

    // Creating and transmitting start control packet
    packetLength = createControlPacket(packet, C_END, st.st_size, fileName, fileNameLength);
    if (llwrite(fd, packet, packetLength) < 0) ABORT(file);
    printf("> Transmitted END packet\n");
    printControlPacket(packet);

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

    printf("> Received START packet\n");
    printControlPacket(packet);

    // Opening file
    int file = open(fileName, O_WRONLY | O_TRUNC | O_CREAT, 0777);
    if (file < 0) return -1;

    // Calculating number of packets to be read
    int numberPackets = fileSize / MAX_DATA_SIZE;
    if (fileSize % MAX_PACKET_SIZE > 0) ++numberPackets;
    
    for (int i = 0; i < numberPackets; ++i) {

        if ((packetLength = llread(fd, packet)) < 0) ABORT(file);

        if (packet[0] == C_DATA) {
            if ((unsigned char) packet[1] != i % 256) ABORT(file);
            if ((unsigned char) packet[2] * 256 + (unsigned char) packet[3] != packetLength - 4) ABORT(file);
            if (write(file, packet + 4, packetLength - 4) < 0) ABORT(file);
        } else if (packet[0] == C_END) ABORT(file);       // If C_END occurs sooner than expected
        
        printf("> Received DATA packet [%d/%d]\n", i+1, numberPackets);
    }
    
    // Reading end control packet
    if ((packetLength = llread(fd, packet)) < 0) ABORT(file);
    if (packet[0] != C_END) ABORT(file);
    
    // Parse data from control packet
    int verificationFileSize;
    char verificationFileName[MAX_PATH_SIZE];
    if ((verificationFileSize = parseControlPacket(packet, packetLength, verificationFileName)) < 0) ABORT(file);

    // Verifying if end control packet information is equal to start control packet information
    if (fileSize != verificationFileSize || strcmp(fileName, verificationFileName) != 0) ABORT(file);

    printf("> Received END packet\n");
    printControlPacket(packet);

    // Closing file
    if (close(file) < 0) return -1;

    return 0;
}

