#ifndef CLIENT_PACKETMESSAGEENCODERDECODER_H
#define CLIENT_PACKETMESSAGEENCODERDECODER_H

#include "../include/Packet.h"

class PacketMessageEncoderDecoder{
private:
    vector<char> _bytes;
    int _len;
    short _opcode;

    Packet* checkForZeroByte(char nextByte);
    Packet* createPacket();
    Packet* activeDATA();
    Packet* activeBCAST();
    Packet* activeACK();
    Packet* activeERROR();

    vector<char> convertStringToCharArray(string str);
    string convertBytesToString(int startPoint);
    void shortToBytes(short num, char* bytesArr);
    short bytesToShort(char* bytesArr);
public:
    PacketMessageEncoderDecoder();
    Packet* decodeNextByte(char nextByte);
    vector<char> encode(Packet* message);
};

#endif //CLIENT_PACKETMESSAGEENCODERDECODER_H