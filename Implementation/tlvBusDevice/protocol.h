#ifndef PROTOCOL_H
#define PROTOCOL_H

#define FRAMESIZE 4 // Bytes Fixed for experiments in simplicity
struct sFrame{
    char cStart; // Expect :
    char cID;  // 'P' for Pump and 'L' for Lamp. Use hex numbers for your implementation
    char cSize; // Total size of transmission
    char cValue; //'0' for OFF and '1' for ON
};


#endif
