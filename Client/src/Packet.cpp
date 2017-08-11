#include "../include/Packet.h"

using namespace std;

// Packet
Packet::Packet(short opcode) : _opcode(opcode){ }

Packet::~Packet() { }

short Packet::getOpcode(){
    return _opcode;
}

// RRQ
RRQ::RRQ(short opcode, string fileName) : Packet(opcode), _fileName(fileName){ }

RRQ::~RRQ() { }

string RRQ::getString(){
    return _fileName;
}

// WRQ
WRQ::WRQ(short opcode, string fileName) : Packet(opcode), _fileName(fileName){ }

WRQ::~WRQ() { }

string WRQ::getString(){
    return _fileName;
}

// DATA
DATA::DATA(short opcode ,short packetSize, short blockNumber, vector<char> data) : Packet(opcode), _packetSize(packetSize), _blockNumber(blockNumber), _data(data){ }

DATA::~DATA() { }

short DATA::getBlockNumber(){
    return _blockNumber;
}

vector<char> DATA::getData(){
    return _data;
}

short DATA::getPacketSize(){
    return _packetSize;
}

string DATA::getString(){
    return "DATA";
}

//ACK
ACK::ACK(short opcode, short block) : Packet(opcode), _block(block){ }

ACK::~ACK() { }

short ACK::getBlock(){
    return _block;
}

string ACK::getString(){
    return "ACK";
}

// ERROR
ERROR::ERROR(short opcode, short errorCode, string errorMessage) : Packet(opcode), _errorCode(errorCode), _errorMessage(errorMessage){ }

ERROR::~ERROR() { }

short ERROR::getErrorCode(){
    return _errorCode;
}

string ERROR::getString(){
    return _errorMessage;
}

// DIRQ
DIRQ::DIRQ(short opcode) : Packet(opcode){}

DIRQ::~DIRQ() { }

string DIRQ::getString(){
    return "DIRQ";
}

// LOGRQ
LOGRQ::LOGRQ(short opcode, string userName) : Packet(opcode), _userName(userName){ }

LOGRQ::~LOGRQ() { }

string LOGRQ::getString(){
    return _userName;
}

// DELRQ
DELRQ::DELRQ(short opcode, string fileName) : Packet(opcode), _fileName(fileName){ }

DELRQ::~DELRQ() { }

string DELRQ::getString(){
    return _fileName;
}

BCAST::BCAST(short opcode, char deletedOrAdded, string fileName) : Packet(opcode), _deletedOrAdded(deletedOrAdded), _fileName(fileName){ }

BCAST::~BCAST() { }

string BCAST::getString(){
    return _fileName;
}

char BCAST::getDeletedOrAdded(){
    return _deletedOrAdded;
}

DISC::DISC(short opcode) : Packet(opcode){ }

DISC::~DISC() { }

string DISC::getString(){
    return "DISC";
}