﻿#pragma comment(lib, "ws2_32")
#include <iostream>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

using int32 = __int32;
using namespace std;

void HandleError(const char* cause);

char recvData[1024];
bool isActive = true;

int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData)) return 0;
	SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(65456);
	if (::bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		HandleError("Bind");
		return 0;
	}

	cout << "> echo-server is activated" << endl;

	while (true)
	{
		SOCKADDR_IN clientAddr;
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addrLen = sizeof(clientAddr);
		int32 recvLen = ::recvfrom(serverSocket, recvData, sizeof(recvData), 0, (SOCKADDR*)&clientAddr, &addrLen);
		if (recvLen <= 0)
		{
			HandleError("RecvFrom");
			return 0;
		}
		int32 errorCode = ::sendto(serverSocket, recvData, recvLen, 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
		if (errorCode == SOCKET_ERROR)
		{
			HandleError("SendTo");
			return 0;
		}
		cout << "> echoed: " << recvData << endl;
	}

	cout << "> echo-server is de-activated" << endl;

	::closesocket(serverSocket);
	::WSACleanup();
	system("pause");

	return 0;
}

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}
