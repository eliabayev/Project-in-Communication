#include "../include/connectionHandler.h"
#include "../include/PacketMessageEncoderDecoder.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

ConnectionHandler::ConnectionHandler(string host, short port): _encDec(), host_(host), port_(port), io_service_(), socket_(io_service_),
                                                               _data(), _blockNumber(1), _dataSize(0), _fileName(""), _lastRequest(""), _open(true){}

//raminder to check if _data should be initiallized with number

ConnectionHandler::~ConnectionHandler() {
    close();
}

Packet* ConnectionHandler::process(Packet* message){
    Packet* replyMessage = 0;
    short opcode = (*message).getOpcode();
    if(opcode<0 || opcode>10)
        replyMessage = new ERROR((short)5, (short)4, "Illigal TFTP opration");
    else{
        if(opcode==3) { //DATA
            DATA *dataPointer= dynamic_cast<DATA*>(message);
            if(dataPointer->getData().size() == 512){
                _dataSize+=(int)(dataPointer->getPacketSize());
                vector<char> dataBlock = dataPointer->getData();
                _data.push_back(dataBlock);
                _blockNumber++;
                replyMessage = new ACK((short)4, ((DATA&)(*message)).getBlockNumber());
            }
            else {
                _dataSize+=(int)(dataPointer->getPacketSize());
                vector<char> dataBlock = dataPointer->getData();
                _data.push_back(dataBlock);
                vector<char> charBuffer;
                for(vector<char> currentByteData : _data)
                    for(char currentChar : currentByteData)
                        charBuffer.push_back(currentChar);
                if(_lastRequest.compare("RRQ")==0) {
                    writeToFile(charBuffer.data(), charBuffer.size());
                    replyMessage = new ACK((short) 4, dataPointer->getBlockNumber());
                }
                else if(_lastRequest.compare("DIRQ")==0) {
                    string allFiles = "";
                    for(unsigned int i=0;i<charBuffer.size();i++){
                        if(charBuffer[i]!='\0')
                            allFiles = allFiles + charBuffer[i];
                        else if(i!=charBuffer.size()){
                            cout << allFiles << endl;
                            allFiles = "";
                        }
                        else{
                            cout << allFiles;
                            allFiles = "";
                        }
                    }
                }
                _lastRequest="";
                _fileName="";
                _data.clear();
                _dataSize = 0;
                _blockNumber = 1;
            }
        }

        else
        if(opcode==4) { //ACK
            ACK *ackPointer= dynamic_cast<ACK*>(message);
            if(ackPointer->getBlock()!=0) {
                cout << "ACK " << ackPointer->getBlock() << endl;
                if (!_data.empty())
                    replyMessage = createDataPacket();
                else {
                    cout << "WRQ " << _fileName << " complete" << endl;
                    _blockNumber = 1;
                }
            }
            else if(_lastRequest.compare("WRQ")==0){
                    cout << "ACK 0" << endl;
                    vector<char> inputBytesVector = readFromFile();
                    AddByteArraysToData(inputBytesVector);
                    replyMessage = createDataPacket();
                }
            else if(_lastRequest.compare("DISC")==0){
                cout << "ACK 0" << endl;
                _open = false;
            }
            else{
                cout << "ACK 0" << endl;
            }
        }

        else if(opcode==9) { //BCAST
            BCAST *bcastPointer= dynamic_cast<BCAST*>(message);
            if((short)bcastPointer->getDeletedOrAdded()==1)
                cout << "BCAST " << "add " << _fileName << endl;
            else
                cout << "BCAST " << "del " << _fileName << endl;
        }

        else if(opcode==5) { //ERROR
            ERROR *errorPointer= dynamic_cast<ERROR*>(message);
            cout << "Error " << errorPointer->getErrorCode() << endl;
            _fileName="";
            _data.clear();
            _dataSize = 0;
            _blockNumber = 1;
        }
    }
    return replyMessage;
}

vector<char> ConnectionHandler::readFromFile(){
        std::vector<char> vec;
        std::ifstream file(_fileName);
        file.seekg(0, std::ios_base::end);
        std::streampos fileSize = file.tellg();
        vec.resize(fileSize);

        file.seekg(0, std::ios_base::beg);
        file.read(&vec[0], fileSize);
        return vec;
}

bool ConnectionHandler::fileExist(){
    ifstream inFile(_fileName);
    return inFile.good();
}

void ConnectionHandler::writeToFile(char finalCharBuffer[], int size){
        string filename = _fileName;
        ofstream fout;
        fout.open(filename);
        for (int i = 0; i < size; i++)
            fout << finalCharBuffer[i];
        cout << "RRQ " << _fileName << " complete" << endl;
        fout.close();
}

void ConnectionHandler::AddByteArraysToData(vector<char> array){
    int available = array.size();
    int pos = 0;
    for(unsigned int i=0;i<(array.size()/512)+1;i++){
        if(available>=512){
            vector<char> dataBlock(array.begin() + pos, array.begin() + pos + 512);
            _data.push_back(dataBlock);
            pos=pos+512;
            available=available-512;
        }
        else {
            vector<char> dataBlock(array.begin() + pos, array.begin() + array.size());
            _data.push_back(dataBlock);
        }
    }
}

Packet* ConnectionHandler::createDataPacket(){
    vector<char> firstData = (_data.front());
    _data.erase(_data.begin());
    Packet* dataBlock = new DATA((short)3, (short)firstData.size(), _blockNumber, firstData); //@pointer
    _blockNumber++;
    return dataBlock;
}

Packet* ConnectionHandler::stringToPacket(string userInput){
    int i = userInput.find_first_of(" ");
    string stringUntilSpace = userInput.substr(0, i);
    string stringAfterSpace = userInput.substr(i+1);
    _lastRequest=stringUntilSpace;
    if(stringUntilSpace.compare("RRQ")==0){
        if((userInput.length() == stringUntilSpace.length())||(userInput.length() == stringUntilSpace.length()+1))
            return new RRQ((short)1, " " );
        else {
            _fileName = stringAfterSpace;
            return new RRQ((short) 1, stringAfterSpace);
        }
    }
    else if(stringUntilSpace.compare("WRQ")==0){
        _fileName=stringAfterSpace;
        if(fileExist())
            return new WRQ((short)2, stringAfterSpace );
    }
    else if(stringUntilSpace.compare("DIRQ")==0){
        return new DIRQ((short)6);
    }
    else if(stringUntilSpace.compare("LOGRQ")==0){
        if((userInput.length() == stringUntilSpace.length())||(userInput.length() == stringUntilSpace.length()+1))
            return new LOGRQ((short)7, " " );
        else
            return new LOGRQ((short)7, stringAfterSpace );
    }
    else if(stringUntilSpace.compare("DELRQ")==0){
        if((userInput.length() == stringUntilSpace.length())||(userInput.length() == stringUntilSpace.length()+1))
            return new DELRQ((short)8, " " );
        else {
            _fileName = stringAfterSpace;
            return new DELRQ((short) 8, stringAfterSpace);
        }
    }
    else if(stringUntilSpace.compare("DISC")==0){
        return new DISC((short)10);
    }
    return 0;
}

bool ConnectionHandler::connect() {
    try {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
        boost::system::error_code error;
        socket_.connect(endpoint, error);
        if (error)
            throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
            tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);
        }
        if(error)
            throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
            tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
        if(error)
            throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, '\n');
}

bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
        do{
            getBytes(&ch, 1);
            frame.append(1, ch);
        }while (delimiter != ch);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
    bool result=sendBytes(frame.c_str(),frame.length());
    if(!result) return false;
    return sendBytes(&delimiter,1);
}

// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}

bool ConnectionHandler::sendPacket(Packet* intputPacket){
    vector<char> intputPacketBytes = _encDec.encode(intputPacket);
    return sendBytes(intputPacketBytes.data(), intputPacketBytes.size());
}

void ConnectionHandler::convertVectorToCharArray(vector<char> vector, char something[]){
    for(unsigned int i=0;i<vector.size();i++) 
        something[i] = vector[i];
}

PacketMessageEncoderDecoder& ConnectionHandler::getEncDec(){
    return _encDec;
}

bool ConnectionHandler::getOpen(){
    return _open;
}

void ConnectionHandler::setOpen(){
    _open = false;
}