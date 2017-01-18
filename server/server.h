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
	HANDLE hThread;
	bool ifReady;

public:
	SERVER(): hThread(NULL), ifReady(false){}
	~SERVER();
	bool initServer(int portNum, char* ipAddress);
	bool sendMessage(const char* message);
	bool getState();
};
