#define FLAG 0b01111110

// Campo de Endereço
#define A_EMITTER_RECEIVER 0x03  // Comandos enviados pelo Emissor e Respostas enviadas pelo Receptor
#define A_RECEIVER_EMITTER 0x01  // Comandos enviados pelo Receptor e Respostas enviadas pelo Emissor

// Campo de Controlo
#define C_SET       0b00000011  // Set up
#define C_DISC      0b00001011  // Disconnect
#define C_UA        0b00000111  // Unnumbered acknowledgment
#define C_RR(R)     0b0000101 | (R << 7)  // Receiver ready
#define C_REJ(R)    0b0000001 | (R << 7)  // Reject

#define BCC(A, C)   A ^ C