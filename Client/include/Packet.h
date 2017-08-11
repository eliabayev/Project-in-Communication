#ifndef CLIENT_PACKET_H
#define CLIENT_PACKET_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

//Packet
class Packet {
private:
    short _opcode;
public:
    Packet(short opcode);
    short getOpcode();
    virtual ~Packet();
    virtual string getString() = 0;
};

// RRQ
class RRQ : public Packet{
private:
    string _fileName; //with '\0' at the end
public:
    RRQ(short opcode, string fileName);
    virtual ~RRQ();
    virtual string getString() override;
};

// WRQ
class WRQ : public Packet{
private:
    string _fileName; //with '\0' at the end
public:
    WRQ(short opcode, string fileName);
    virtual ~WRQ();
    virtual string getString() override;
};

// DATA
class DATA : public Packet{
private:
    short _packetSize;
    short _blockNumber;
    vector<char> _data;
public:
    DATA(short opcode ,short packetSize, short blockNumber, vector<char> data);
    virtual ~DATA();
    short getBlockNumber();
    vector<char> getData();
    short getPacketSize();
    virtual string getString() override;
};

// ACK
class ACK : public Packet{
private:
    short _block;
public:
    ACK(short opcode, short block);
    virtual ~ACK();
    short getBlock();
    virtual string getString() override;
};

// ERROR
class ERROR : public Packet{
private:
    short _errorCode;
    string _errorMessage;
public:
    ERROR(short opcode, short errorCode, string errorMessage);
    virtual ~ERROR();
    short getErrorCode();
    virtual string getString() override;
};

// DIRQ
class DIRQ : public Packet{
private:
public:
    DIRQ(short opcode);
    virtual ~DIRQ();
    virtual string getString() override;
};

// LOGRQ
class LOGRQ : public Packet{
private:
    string _userName;
public:
    LOGRQ(short opcode, string userName);
    virtual ~LOGRQ();
    virtual string getString() override;
};

// DELRQ
class DELRQ : public Packet{
private:
    string _fileName;
public:
    DELRQ(short opcode, string fileName);
    virtual ~DELRQ();
    virtual string getString() override;
};

// BCAST
class BCAST : public Packet{
private:
    char _deletedOrAdded;
    string _fileName;
public:
    BCAST(short opcode, char deletedOrAdded, string fileName);
    virtual ~BCAST();
    virtual string getString() override;
    char getDeletedOrAdded();
};

// DISC
class DISC : public Packet{
private:
public:
    DISC(short opcode);
    virtual ~DISC();
    virtual string getString() override;
};

#endif //CLIENT_PACKET_H
