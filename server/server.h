#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <iostream>
#include <string>
#include "image.h"
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
	IMAGE image;
	string fileName;
	bool fileFlag;

public:
	SERVER(): ifReady(false), cameraID(-1), fileFlag(false){}
	~SERVER();
	bool initServer(const int portNum, const char* ipAddress, const int ID);
	bool sendMessage(const char* message);
	bool getState() { return ifReady; }
	int getID() { return cameraID; }
	bool getFileState() { return fileFlag; }
	SOCKET getClientScoket() { return clientScoket; }
	void setSaveFileName(string fn);
	void readSaveImage();
	IMAGE getImage() { return image; }
};
