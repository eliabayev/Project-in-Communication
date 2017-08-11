#include "../include/Packet.h"
#include "../include/connectionHandler.h"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/locale.hpp>
#include <stdlib.h>

using namespace boost;

/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/

void readFromServer(ConnectionHandler& handler) {
    bool open = true;
    while (1) {
        open = handler.getOpen();
        if(!open)
            break;
        char byteBuffer[1];
        handler.getBytes(byteBuffer,1);
        char nextByte=byteBuffer[0];
        Packet *firstP = handler.getEncDec().decodeNextByte(nextByte);
        if (firstP != nullptr) {
            Packet *secondP = handler.process(firstP);
            delete(firstP);
            if (secondP != nullptr) {
                vector<char> message = handler.getEncDec().encode(secondP);
                delete(secondP);
                char finalArray[message.size()];
                handler.convertVectorToCharArray(message, finalArray);
                if (!handler.sendBytes(finalArray, message.size())) {
                    std::cout << "Disconnected. Exiting...\n" << std::endl;
                    handler.setOpen();
                }
            }
        }
    }
}

void readFromKeyBoard(ConnectionHandler& handler){
    bool open = true;
    while(1){
        open = handler.getOpen();
        if(!open)
            break;
        string userInput;
        getline(cin, userInput, '\n');
        Packet* returnedPacket = handler.stringToPacket(userInput);
        if(returnedPacket==nullptr)
            cout << "Request is illegal" <<endl;
        else {
            if (!handler.sendPacket(returnedPacket)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                handler.setOpen();
            }
            delete(returnedPacket);
        }
        if(userInput.compare("DISC")==0)
            break;
    }
}

int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);

    ConnectionHandler connectionHandler(host, port);

    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

    boost::thread t1(boost::bind(readFromServer, boost::ref(connectionHandler)));
    readFromKeyBoard(connectionHandler);

    t1.join();
    return 0;
}