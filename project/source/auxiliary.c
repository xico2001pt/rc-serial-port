#include "../headers/auxiliary.h"
#include "../headers/protocol.h"
#include <string.h>

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
    // TODO: N GOSTO DISTO xD
    buffer[0] = control;
    buffer[1] = 0;
    buffer[2] = sizeof(int);
    buffer[3] = size;
    buffer[7] = 1;
    buffer[8] = fileNameSize;
    strncpy(buffer + 9, fileName, fileNameSize);
    return 9 + fileNameSize;
}

int createDataPacket(char *buffer, char sequence, char *data, int dataLength) {
    buffer[0] = 1;
    buffer[1] = sequence;
    buffer[2] = (char) (dataLength / 256);
    buffer[3] = (char) (dataLength % 256);
    strncpy(buffer + 4, data, dataLength);
    return 4 + dataLength;
}