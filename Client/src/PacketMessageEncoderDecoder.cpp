#include "../include/PacketMessageEncoderDecoder.h"
#include "../include/Packet.h"

//Constructor
PacketMessageEncoderDecoder::PacketMessageEncoderDecoder() :_bytes(1024), _len(0), _opcode(1){}

//Decoder
Packet* PacketMessageEncoderDecoder::decodeNextByte(char nextByte){
    if(_len <= 2){
        _bytes[_len++] = nextByte;
        if(_len == 2) {
            char opcodeBytes[2];
            opcodeBytes[0]=_bytes[0];
            opcodeBytes[1]=_bytes[1];
            _opcode = bytesToShort(opcodeBytes);
        }
    }
    else if(_opcode == 5){ //ERROR
        if(_len <= 4)
            _bytes[_len++] = nextByte;
        else
            return checkForZeroByte(nextByte);
    }
    else if(_opcode == 9){ //BCAST
        if(_len <= 3)
            _bytes[_len++]=nextByte;
        else
            return checkForZeroByte(nextByte);
    }
    else if(_opcode == 3) { //DATA
        _bytes[_len++] = nextByte;
        if (_len >= 6) {
            char packetSizeBytes[2];
            packetSizeBytes[0] = _bytes[2];
            packetSizeBytes[1] = _bytes[3];
            if (bytesToShort(packetSizeBytes) + 6 == _len)
                return createPacket();
        }
    }
    else if(_opcode == 4) { //ACK
        _bytes[_len++] = nextByte;
        if (_len == 4)
            return createPacket();
    }
    return 0; //not a line yet
}

//Encoder
vector<char> PacketMessageEncoderDecoder::encode(Packet* message){
    short opcode = (*message).getOpcode();
    vector<char> opcodeBytesToReturn(2);
    char opcodeBytes[2];
    shortToBytes(opcode, opcodeBytes);
    opcodeBytesToReturn[0]=opcodeBytes[0];
    opcodeBytesToReturn[1]=opcodeBytes[1];
    if((opcode==1)||(opcode==2)||(opcode==7)||(opcode==8)){
        vector<char> nameBytes = convertStringToCharArray(message->getString());
        vector<char> messageBytes(nameBytes.size()+3);
        messageBytes[0]=opcodeBytes[0];
        messageBytes[1]=opcodeBytes[1];
        messageBytes[messageBytes.size()-1]=0; // the delimiter
        for(unsigned int i=2;i<messageBytes.size()-1;i++){
            messageBytes[i]=nameBytes[i-2];
        }
        return messageBytes;
    }
    else if(opcode==4){ //ACK
        char blockBytes[2];
        ACK *ackPointer= dynamic_cast<ACK*>(message);
        shortToBytes(ackPointer->getBlock(), blockBytes);
        vector<char> messageBytes(4);
        messageBytes[0]=opcodeBytes[0];
        messageBytes[1]=opcodeBytes[1];
        messageBytes[2]=blockBytes[0];
        messageBytes[3]=blockBytes[1];
        return messageBytes;
    }
    else if(opcode==5){ //ERROR
        char errorCodeBytes[2];
        ERROR *errorPointer= dynamic_cast<ERROR*>(message);
        shortToBytes(errorPointer->getErrorCode(), errorCodeBytes);
        vector<char> nameBytes = convertStringToCharArray(message->getString());
        vector<char> messageBytes(nameBytes.size() + 5);
        messageBytes[0]=opcodeBytes[0];
        messageBytes[1]=opcodeBytes[1];
        messageBytes[2]=errorCodeBytes[0];
        messageBytes[3]=errorCodeBytes[1];
        messageBytes[messageBytes.size()-1]=0; // the delimiter
        for(unsigned int i=4;i<messageBytes.size()-1;i++){
            messageBytes[i]=nameBytes[i-4];
        }
        return messageBytes;
    }
    else if(opcode==3){ //DATA
        DATA *dataPointer= dynamic_cast<DATA*>(message);
        vector<char> dataBytes = (dataPointer->getData());
        char blockNumberBytes[2];
        shortToBytes(dataPointer->getBlockNumber(), blockNumberBytes);
        char packetSizeBytes[2];
        shortToBytes(dataPointer->getPacketSize(), packetSizeBytes);
        vector<char> messageBytes(dataBytes.size() + 6);
        messageBytes[0]=opcodeBytes[0];
        messageBytes[1]=opcodeBytes[1];
        messageBytes[2]=packetSizeBytes[0];
        messageBytes[3]=packetSizeBytes[1];
        messageBytes[4]=blockNumberBytes[0];
        messageBytes[5]=blockNumberBytes[1];
        for(unsigned int i=6;i<messageBytes.size();i++){
            messageBytes[i]=dataBytes[i-6];
        }
        return messageBytes;
    }
    return opcodeBytesToReturn;
}

vector<char> PacketMessageEncoderDecoder::convertStringToCharArray(string str){
    vector<char> vec(str.size());
    for(unsigned int i=0;i<str.size();i++)
        vec[i]=str[i];
    return vec;
}

Packet* PacketMessageEncoderDecoder::checkForZeroByte(char nextByte){
    if(nextByte!='\0')
        _bytes[_len++]=nextByte;
    else
        return createPacket();
    return 0;
}

Packet* PacketMessageEncoderDecoder::createPacket(){
    Packet *result = nullptr;
    if(_opcode == 3) result = activeDATA();
    else if(_opcode == 4) result = activeACK();
    else if(_opcode == 5) result = activeERROR();
    else if(_opcode == 9) result = activeBCAST();
    return result;
}

//DATA(3)
Packet* PacketMessageEncoderDecoder::activeDATA(){
    char packetSizeBytes[2];
    packetSizeBytes[0] = _bytes[2];
    packetSizeBytes[1] = _bytes[3];
    short packetSize = bytesToShort(packetSizeBytes);
    char blockNumberBytes[2];
    blockNumberBytes[0] = _bytes[4];
    blockNumberBytes[1] = _bytes[5];
    short blockNumber  = bytesToShort(blockNumberBytes);
    vector<char> dataBytes(_len-6);
    for(int i=6; i<_len; i++)
        dataBytes[i-6] = _bytes[i];
    _len = 0;
    _bytes.clear();
    return new DATA((short)3, blockNumber, packetSize, dataBytes);
}

//ACK(4)
Packet* PacketMessageEncoderDecoder::activeACK(){
    char blockNumberBytes[2];
    blockNumberBytes[0] = _bytes[2];
    blockNumberBytes[1] = _bytes[3];
    short blockNumber = bytesToShort(blockNumberBytes);
    _len = 0;
    _bytes.clear();
    return new ACK((short)4, blockNumber);
}

//ERROR(5)
Packet* PacketMessageEncoderDecoder::activeERROR(){
    char errorCodeBytes[2];
    errorCodeBytes[0] = _bytes[2];
    errorCodeBytes[1] = _bytes[3];
    short errorCode = bytesToShort(errorCodeBytes);
    string errorName = convertBytesToString(4);
    _len=0;
    _bytes.clear();
    return new ERROR((short)5, errorCode, errorName);
}

//BCAST(9)
Packet* PacketMessageEncoderDecoder::activeBCAST(){
    char deletedOrAdded = _bytes[2];
    string name = convertBytesToString(3);
    _len = 0;
    _bytes.clear();
    return new BCAST((short)9, deletedOrAdded, name);
}

string PacketMessageEncoderDecoder::convertBytesToString(int startPoint){
    string str = "";
    for(int i=startPoint;i<_len;i++)
        str=str+_bytes[i];
    return str;
}

short PacketMessageEncoderDecoder::bytesToShort(char* bytesArr) {
    short result = (short)((bytesArr[0] & 0xff) << 8);
    result += (short)(bytesArr[1] & 0xff);
    return result;
}

void PacketMessageEncoderDecoder::shortToBytes(short num, char* bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}