CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lboost_locale -lboost_thread

all: runClient
	g++ -o bin/TFTPclient bin/Packet.o bin/PacketMessageEncoderDecoder.o bin/connectionHandler.o bin/echoClient.o $(LDFLAGS)

runClient: bin/Packet.o bin/PacketMessageEncoderDecoder.o bin/connectionHandler.o bin/echoClient.o
	
bin/Packet.o: src/Packet.cpp
	g++ $(CFLAGS) -o bin/Packet.o src/Packet.cpp

bin/PacketMessageEncoderDecoder.o: src/PacketMessageEncoderDecoder.cpp
	g++ $(CFLAGS) -o bin/PacketMessageEncoderDecoder.o src/PacketMessageEncoderDecoder.cpp
	
bin/connectionHandler.o: src/connectionHandler.cpp
	g++ $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp
	
bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp
	
.PHONY: clean
clean:
	rm -f bin/*