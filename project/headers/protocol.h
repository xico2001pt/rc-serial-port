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

/**
 * @brief Obtains the serial port config
 * 
 * @param fd        File descriptor of the serial port
 * @param config    Struct to be overriden by the current config of the serial port
 * @return int      Returns -1 if it fails, 0 if it successfuly gets the config
 */
int getConfig(int fd, struct termios *config);

/**
 * @brief Loads a serial port configuration
 * 
 * @param fd        File descriptor of the serial port
 * @param config    Struct with the config to be applied to the serial port
 * @return int      Returns -1 if it fails, 0 if it successfuly loads the config
 */
int loadConfig(int fd, struct termios *config);

/**
 * @brief Returns the configuration of the non canonical state of the serial port
 * 
 * @param config    Struct to be overiden by the non canonical configuration
 */
void configNonCanonical(struct termios *config);

/**
 * @brief Applies the non canonical configuration to the serial port, saves the old config, flushes the serial port file descriptor and returns the file descriptor
 * 
 * @param port  Number of port of the serial port
 * @return int  Returns serial port file descriptor upon success and -1 on failure
 */
int openSerial(int port);

/**
 * @brief Applies the old config to the serial port and closes the file descriptor
 * 
 * @param fd    File descriptor to be restored and closed
 * @return int  0 on success or -1 on error
 */
int closeSerial(int fd);

/**
 * @brief Attempts to write data in the given file descriptor, performing the stuffing operation before sending it
 * 
 * @param fd        File descriptor where data will be written
 * @param frame     Array that contains the data to be written
 * @param length    Length of the array
 * @return int      0 on success or -1 on error
 */
int transmitFrame(int fd, char *frame, int length);

/**
 * @brief Attempts to read data from the given file descriptor within the given time
 * 
 * @param fd        File descriptor where data will be read
 * @param timer     Number of seconds that the function waits to read data
 * @param frame     Array that contains the received frame
 * @return int      Length of the received frame or -1 on error
 */
int receiveFrame(int fd, int timer, char *frame);

/**
 * @brief Byte-by-byte state machine, useful to confirm information and supervisory/unnunbered frames format.
 In case of an information frame the buffer will be modified with a destuffed version of itself, and the length will be modified as well accordingly
 * 
 * @param currentState  Current state of the state machine
 * @param frame         Array that contains the received frame (dynamically updated inside this function)
 * @param length        Length of the received frame (dynamically updated inside this function)
 * @return FrameState   State of the state machine after the current byte is processed
 */
FrameState FrameStateMachine(FrameState currentState, char *frame, int *length);

/**
 * @brief Applies the XOR operation to all elements of the array
 * 
 * @param data      Charactares to be operated
 * @param length    Number of characters
 * @return char     Result of the XOR operation
 */
char calculateBCC(char *data, int length);

/**
 * @brief Converts the given data by translating ESCAPE+0x5D and ESCAPE+0x5E sequences to ESCAPE and FLAG respectively
 * 
 * @param data      Data to be modified
 * @param length    Length of the data
 * @return int      Length of the new data
 */
int destuffing(char *data, int length);

/**
 * @brief Converts the given data to not include any ESCAPE and FLAG bytes, by changing them to ESCAPE+0x5D and ESCAPE+0x5E respectively
 * 
 * @param data      Data to be modified
 * @param length    Length of the data
 * @return int      Length of the new data
 */
int stuffing(char *data, int length);

#endif // PROTOCOL_H
