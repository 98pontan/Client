#pragma once
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "GameProtocol.h"
#include <string>

#define SCK_version 2 0x0202

class Client {
private:

	//Last player coordinates
	Coordinate position;
	//default header for messages
	MsgHead thisHeader;
	//Socket
	SOCKET sock;
	//Boolean for stoping the listening loop
	bool stopListening;
	//Boolean to know if a player has a position
	bool setPosition;
	//sends a move event
	void move(Coordinate xy, Coordinate lastMove);
	//sends a leave event
	void leave();
public:
	void serverConnector(std::string ipAdress, int portNum);
	~Client();
	void listener();

};

class sendGame {
private:
	// UDP socket
	SOCKET guiSocket;
	// Server 
	sockaddr_in6 server;
public:
	sendGame();
	~sendGame();
	void repaint(unsigned int id, Coordinate pos);
};