#include "Client.h"
#include <iostream>

int main() {
	Client client;
	client.serverConnector("130.240.74.25", 5400);
}

void Client::serverConnector(std::string ipAdress, int portNum) {

	WSADATA wsdata;
	int wsCheck;
	WORD ver;
	//int portNum = 54000;
	int connected;
	sockaddr_in addres;

	


	ver = MAKEWORD(2, 2);
	wsCheck = WSAStartup(ver, &wsdata);

	if (wsCheck != 0) {
		std::cerr << "cant initialize" << std::endl;
		return;
	}
	

	std::cout << portNum << std::endl;
	std::string strIP = ipAdress;

	addres.sin_family = AF_INET;
	addres.sin_port = htons(portNum);
	inet_pton(AF_INET, strIP.c_str(), &addres.sin_addr);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	connected = SOCKET_ERROR;

	connected = connect(sock, (sockaddr*)&addres, sizeof(addres));
	if (connected != SOCKET_ERROR) {
		std::cout << "Succesfull connection!" << std::endl;
	}
	else {
		std::cout << "Cant connect to server" << std::endl;
		return;
	}

	// Join message
	JoinMsg jMsg{
		MsgHead{sizeof jMsg, 0, 0, Join},
		Human,
		Cube,
		"HelloWorld"
	};
	MsgHead responseHead;
	send(sock, (char*)&jMsg, sizeof(jMsg), 0);
	
	char buffer[256] = { '\0*' };
	recv(sock, buffer, sizeof(buffer), 0);
	
	memcpy(&responseHead, buffer, sizeof(buffer));
	thisHeader.id = responseHead.id;

	std::cout << "Player Id: " << thisHeader.id <<  std::endl;
	stopListening = false;
	listener();
}

void Client::listener() {
	sendGame sGame = sendGame();
	fd_set readSet;
	timeval time;
	MsgHead msgHead;
	int validSockets;
	short pressedKey;


	while (true)
	{

		if (stopListening)
			return;

		time.tv_sec = 0;
		time.tv_usec = 0;

		FD_ZERO(&readSet);
		FD_SET(sock, &readSet);

		validSockets = select(0, &readSet, NULL, NULL, &time);

		char buffer[512] = { '\0' };

		if (validSockets == SOCKET_ERROR)
		{
			std::cout << "Error! Select failed " << WSAGetLastError() << std::endl;
			break;
		}

		else if (FD_ISSET(sock, &readSet))
		{
			recv(sock, buffer, sizeof(buffer), 0);
			memcpy(&msgHead, buffer, sizeof(msgHead));

			if (msgHead.type == Change)
			{
				ChangeMsg eventMessage;
				memcpy(&eventMessage, buffer, sizeof(eventMessage));

				switch (eventMessage.type)
				{
				case NewPlayer: {
					NewPlayerMsg npMsg;
					memcpy(&npMsg, buffer, sizeof(npMsg));
					std::cout << "New player: " << npMsg.name << "ID: " << msgHead.id << "Has joined!" << std::endl;
					break;
				}

				case NewPlayerPosition: {
					NewPlayerPositionMsg nppMsg;
					memcpy(&nppMsg, buffer, sizeof(nppMsg));
					std::cout << msgHead.id << " " << thisHeader.id << std::endl;
					if (msgHead.id == thisHeader.id)
					{
						position.x = nppMsg.pos.x;
						position.y = nppMsg.pos.y;
						std::cout << "New position X: " << position.x << " New position Y: " << position.y << std::endl;

					}
					else {
						//position.x = nppMsg.pos.x;
						//position.y = nppMsg.pos.y;
						std::cout << "ID: " << msgHead.id << " Position X: " << nppMsg.pos.x << " Position Y: " << nppMsg.pos.y << std::endl;
					}
					sGame.repaint(msgHead.id, nppMsg.pos);
					setPosition = true;
					break;
				}
				case PlayerLeave: {
					PlayerLeaveMsg plMsg;
					memcpy(&plMsg, buffer, sizeof(plMsg));
					std::cout << "ID: " << msgHead.id << " Has left the game" << std::endl;
					break;
				}

				}

			}

		}


		if (setPosition) {
			Coordinate lastMove = position;

			if (GetAsyncKeyState(VK_UP) & 1) {
				position.y--;
				move(position, lastMove);
			}


			else if (GetAsyncKeyState(VK_DOWN) & 1) {
				position.y++;
				move(position, lastMove);
			}


			else if (GetAsyncKeyState(VK_RIGHT) & 1) {
				position.x++;
				move(position, lastMove);
			}
			else if (GetAsyncKeyState(VK_LEFT) & 1)
			{
				position.x--;
				move(position, lastMove);
			}

			else if (GetAsyncKeyState('Q') & 1)
			{
				leave();
			}
		}

	}//while
}
  


void Client::move(Coordinate xy, Coordinate lastMove) {
	thisHeader.type = Event;

	EventMsg eveMsg;
	MoveEvent moveEvent;

	eveMsg.head = thisHeader;
	eveMsg.type = Move;
	

	moveEvent.event = eveMsg;
	moveEvent.pos = xy;
	moveEvent.dir.x = lastMove.x;
	moveEvent.dir.y = lastMove.y;

	std::cout << "sending move" << moveEvent.pos.x << moveEvent.pos.y << std::endl;
	send(sock, (char*)&moveEvent, sizeof(moveEvent), 0);
}

void Client::leave() {
	LeaveMsg lMsg;

	thisHeader.type = Leave;

	lMsg.head = thisHeader;

	send(sock, (char*)&lMsg, sizeof(lMsg), 0);


	stopListening = true;

	Client::~Client();

}

Client::~Client() {
	WSACleanup();
	closesocket(sock);
	return;
}

