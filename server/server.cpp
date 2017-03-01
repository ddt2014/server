#include "server.h"

SERVER::SERVER() : ifReady(false), cameraID(-1), fileFlag(false) {
	image = new IMAGE();
}

DWORD WINAPI recieveThread(void* server) {
	SERVER* sv = (SERVER *)server;
	int RET = 0;
	char recvBuffer[BUFFER_LENGTH];

	//init recvBuffer 
	while (true) {
		memset(recvBuffer, 0x00, sizeof(recvBuffer));
		RET = recv(sv->getClientScoket(), recvBuffer, BUFFER_LENGTH, 0);
		if (RET == 0 || RET == SOCKET_ERROR) {
			cout << "message recieve failed" << endl;
			break;
		}
		cout << "message from camera " << sv->getID() + 1 << ": " << recvBuffer << endl;
		if (strcmp(recvBuffer, "starting transfer file") == 0) {
			memset(recvBuffer, 0x00, sizeof(recvBuffer));
			RET = recv(sv->getClientScoket(), recvBuffer, BUFFER_LENGTH, 0);
			if (RET == 0 || RET == SOCKET_ERROR) {
				cout << "file recieve failed" << endl;
				break;
			}
			string fileName(recvBuffer);
			fileName = "./" + to_string(sv->getID() + 1) + "/" + fileName;
			cout << "start transfer file : " << fileName << endl;
			FILE * fp = fopen(fileName.c_str(), "wb");
			if (NULL == fp) {
				cout << "open file faild " << fileName << endl;
				break;
			}
			while (true){
				memset(recvBuffer, 0x00, sizeof(recvBuffer));
				RET = recv(sv->getClientScoket(), recvBuffer, BUFFER_LENGTH, 0);
				if (RET == 0 || RET == SOCKET_ERROR) {
					cout << "file recieve failed" << endl;
					break;
				}
				//printf("%s", recvBuffer);
				//cout << endl << endl << endl;
				if (strcmp(recvBuffer, "transfer over") == 0)
					break;
				fwrite(recvBuffer, sizeof(char), RET, fp);
			}
			sv->setSaveFileName(fileName);
			cout << "transfer end" << endl;
			fclose(fp);
		}
	}
	return 0;
}


bool SERVER:: initServer(const int portNum, const char* ipAddress, const int ID) {
	cameraID = ID;
	int ret;

	//init windows socket
	if (WSAStartup(MAKEWORD(2, 2), &WSA) != 0) {
		cout << "init failed" << endl;
		return false;
	}
	//creat socket
	severScoket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (severScoket == INVALID_SOCKET){
		cout << "creat failed" << GetLastError() << endl;
		return false;
	}
	LocalAddr.sin_family = AF_INET;
	LocalAddr.sin_addr.s_addr = inet_addr(ipAddress);
	LocalAddr.sin_port = htons(portNum);
	memset(LocalAddr.sin_zero, 0x00, 8);
	//bind socket
	ret = bind(severScoket, (struct sockaddr*)&LocalAddr, sizeof(LocalAddr));
	if (ret != 0) {
		cout << "bind failed";
		return false;
	}
	ret = listen(severScoket, 5);
	if (ret != 0) {
		cout << "listen failed";
		return false;
	}

	ret = sizeof(clientAddr);
	clientScoket = accept(severScoket, (struct sockaddr*)&clientAddr, &ret);
	if (clientScoket == INVALID_SOCKET)	{
		cout << "accept failed";
		return false;
	}

	HANDLE hThread = NULL;
	hThread = CreateThread(NULL, 0, recieveThread, this, 0, NULL);
	if (hThread == NULL) {
		cout << "creat thread failed" << endl;
		return false;
	}

	ifReady = true;
	cout << "camera " << cameraID + 1<< " ready." << endl;
	return true;
}

bool SERVER:: sendMessage(const char* message) {
	int ret;
	ret = send(clientScoket, message, strlen(message), 0);
	if (ret == SOCKET_ERROR) {
		printf("Send Information Failed! Error:%d\n", GetLastError());
		return false;
	}
	return true;
}

SERVER:: ~SERVER() {
	closesocket(severScoket);
	closesocket(clientScoket);
	WSACleanup();
}

void SERVER ::setSaveFileName(string fn) {
	fileName = fn;
	fileFlag = true;
}

void SERVER::readSaveImage() {
	if (!fileName.empty()) {
		image->setFileName(fileName);
		image->fileConvert();
	}
}

void SERVER::clearFileState() {
	fileName.clear();
	image->clearState();
}

void SERVER::clearTransState() {
	fileFlag = false;
}