#include "Client.h"

sendGame::sendGame() {
	WSADATA wsdata;
	int wsCheck;
	WORD ver;
	int portNum = 3000;

	wsCheck = WSAStartup(MAKEWORD(2, 2), &wsdata);


	if (wsCheck != 0) {
		std::cerr << "cant initialize" << std::endl;
		return;
	}

	server.sin6_family = AF_INET6;
	server.sin6_port = htons(portNum);
	server.sin6_flowinfo = 0;
	server.sin6_scope_id = 0;

	const char* ipv6Addr = "::1";

	inet_pton(AF_INET6, ipv6Addr, &server.sin6_addr);
	guiSocket = socket(AF_INET6, SOCK_DGRAM, 0);
}

/* 
sendGame::sendGame() {

	WSADATA wsdata;
	int wsCheck;
	WORD ver;
	int portNum;

	guiSocket = socket(AF_INET, SOCK_DGRAM, 0);
	
	ver = MAKEWORD(2, 2);
	wsCheck = WSAStartup(ver, &wsdata);
	portNum = 3000;

	if (wsCheck != 0) {
		std::cerr << "cant initialize" << std::endl;
		return;
	}
	

	if (guiSocket == INVALID_SOCKET) {
		std::cerr << "cant create socket" << std::endl;
		return;
	}
	std::cout << "Send game initialised" << std::endl;
	// Bind ip address and port
	server.sin_family = AF_INET;
	server.sin_port = htons(portNum);
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

}
*/

sendGame::~sendGame() {
	WSACleanup();
	closesocket(guiSocket);
}

void sendGame::repaint(unsigned int id, Coordinate pos)
{	
	int x = pos.x;
	int y = pos.y;
	int color = id;
	int sendOK;

	std::string message = std::to_string(x) + ":" + std::to_string(y) + ":" + std::to_string(color);
	
	sendOK = sendto(guiSocket, message.c_str(), message.size(), 0, (sockaddr*)&server, sizeof(server));

	if (sendOK == SOCKET_ERROR) {
		std::cout << message << std::endl;
		std::cout << "SOCKET ERROR! " << WSAGetLastError() << std::endl;
	}
}


