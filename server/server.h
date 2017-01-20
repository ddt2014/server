#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define BUFFER_LENGTH 2048

class SERVER {
private:
	WSADATA     WSA;
	SOCKET      severScoket, clientScoket;
	struct      sockaddr_in  LocalAddr, clientAddr;
	bool ifReady;
	int cameraID;

public:
	SERVER(): ifReady(false), cameraID(-1){}
	~SERVER();
	bool initServer(const int portNum, const char* ipAddress, const int ID);
	bool sendMessage(const char* message);
	bool getState();
	int getID();
	static DWORD WINAPI recieveThread(void* server);
};
