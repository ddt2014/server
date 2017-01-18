#include "server.h"

DWORD WINAPI ClientThread(LPVOID ipParameter)
{
	SOCKET ClientScoket = (SOCKET)ipParameter;
	int RET = 0;
	char RecvBuffer[BUFFER_LENGTH];
	//init recvBuffer 
	while (true) {
		memset(RecvBuffer, 0x00, sizeof(RecvBuffer));
		RET = recv(ClientScoket, RecvBuffer, BUFFER_LENGTH, 0);
		if (RET == 0 || RET == SOCKET_ERROR)
		{
			cout << "failed,exit" << endl;
			break;
		}
		cout << "接受的消息为" << RecvBuffer << endl;
	}
	return 0;
}

bool SERVER::initServer(int portNum, char* ipAddress) {
	int ret;

	//init windows socket
	if (WSAStartup(MAKEWORD(2, 2), &WSA) != 0) {
		cout << "init failed" << endl;
		return false;
	}
	//creat socket
	severScoket = socket(AF_INET, SOCK_STREAM, 0);
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

	hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)clientScoket, 0, NULL);
	if (hThread == NULL)	{
		cout << "creat thread failed" << endl;
		return false;
	}
	ifReady = true;
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
	CloseHandle(hThread);
	closesocket(severScoket);
	closesocket(clientScoket);
	WSACleanup();
}

bool SERVER::getState() {
	return ifReady;
}
