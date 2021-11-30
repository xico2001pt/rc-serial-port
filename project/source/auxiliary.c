#include "../headers/auxiliary.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int pathToFilename(char *filename, char *path, int pathLength) {
  int lastSlashIdx = -1, filenameSize = 0;
  for (int i = 0; i < pathLength; ++i) {
    ++filenameSize;
    if (path[i] == '/') {  // Assuming forward slash
      lastSlashIdx = i;
      filenameSize = 0;
    }
  }

  strncpy(filename, path + lastSlashIdx + 1, filenameSize);
  return filenameSize;
}

int createControlPacket(char *buffer, char control, int size, char *fileName, char fileNameSize) {
    char *ptr = buffer;
    *(ptr++) = control;
    *(ptr++) = 0;

    int length;
    char sizeStr[100];
    if ((length = sprintf(sizeStr, "%d", size)) < 0) return -1;

    *(ptr++) = length;
    memcpy(ptr, sizeStr, length);
    ptr += length;
    *(ptr++) = 1;
    *(ptr++) = fileNameSize;
    strncpy(ptr, fileName, fileNameSize);
    return  ptr - buffer + fileNameSize;
}

int createDataPacket(char *buffer, char sequence, char *data, int dataLength) {
    buffer[0] = 1;
    buffer[1] = sequence;
    buffer[2] = (char) (dataLength / 256);
    buffer[3] = (char) (dataLength % 256);
    memcpy(buffer + 4, data, dataLength);
    return 4 + dataLength;
}

int parseControlPacket(char *packet, int packetLength, char *fileName) {
  int size, fileSize;
  char buffer[10];
  for (int i = 1; i < packetLength; i += 2 + size) {
    size = packet[i+1];
    if (packet[i] == T_FILE_SIZE) {
        memcpy(buffer, packet + i + 2, size);
        fileSize = atoi(buffer);
    } else if (packet[i] == T_FILE_NAME) {
        strncpy(fileName, packet + i + 2, size);
    } else {
        return -1;
    }
  }
  return fileSize;
}

void printControlPacket(char *packet) {
  int numParameters = 2, size;
  char aux;
  ++packet;
  for (int p = 0; p < numParameters; ++p) {
    size = (unsigned int) packet[1];
    aux = packet[2 + size - 1];
    packet[2 + size - 1] = '\0';
    printf("     parameter T=%-3d L=%-3d V=%s%c\n", packet[0], size, packet + 2, aux);
    packet[2 + size - 1] = aux;
    packet += 2 + size;
  }
}