#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <termios.h>

#define BAUDRATE B38400

#define FLAG 0b01111110
#define ESCAPE 0b01111101
#define FLAG_STUFFING 0x5E
#define ESCAPE_STUFFING 0x5D

// Campo de Endereço
#define A_EMITTER_RECEIVER 0x03  // Comandos enviados pelo Emissor e Respostas enviadas pelo Receptor
#define A_RECEIVER_EMITTER 0x01  // Comandos enviados pelo Receptor e Respostas enviadas pelo Emissor

// Campo de Controlo
#define C_SET                       0b00000011  // Set up
#define C_DISC                      0b00001011  // Disconnect
#define C_UA                        0b00000111  // Unnumbered acknowledgment
#define C_I(S)                      S << 7      // Recieving information
#define C_RR(R)                     0b0000101 | (R << 7)  // Receiver ready
#define C_REJ(R)                    0b0000001 | (R << 7)  // Reject
#define IS_SU_CONTROL_BYTE(byte)    byte == C_SET || byte == C_DISC || byte == C_UA || (byte & 0b1111111) == C_RR(0) || (byte & 0b1111111) == C_REJ(0)
#define IS_I_CONTROL_BYTE(byte)     (byte & 0b10111111) == C_I(0)
#define BCC(B1, B2)                 B1 ^ B2

#define MAX_FRAME_SIZE 512

typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_RCV, BCC1_RCV, STOP, ERROR} FrameState;

int getConfig(int fd, struct termios *config);
int loadConfig(int fd, struct termios *config);
void configNonCanonical(struct termios *config);
int openSerial(int port);
int closeSerial(int fd);

int receiveFrame(int fd, int timer, char *frame);
FrameState FrameStateMachine(FrameState currentState, char *frame, int length);

char calculateBCC(char *data, int length);
int destuffing(char *data, int length);
int stuffing(char *data, int length);

#endif // PROTOCOL_H