#ifndef AUXILIARY_H
#define AUXILIARY_H

int pathToFilename(char *filename, char *path, int pathLength);

int createControlPacket(char *buffer, char control, int size, char *fileName, char fileNameSize);

int createDataPacket(char *buffer, char sequence, char *data, int dataLength);

#endif // AUXILIARY_H