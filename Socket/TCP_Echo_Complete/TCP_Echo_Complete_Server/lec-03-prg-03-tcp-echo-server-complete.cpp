#pragma comment(lib, "ws2_32")
#include <iostream>
#include <cstring>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

using int32 = __int32;
using namespace std;

const int SO_MAX_CONN = 13;

int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData)) return 0;
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode : " << errCode << endl;
		return 0;
	}
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);	
	serverAddr.sin_port = ::htons(65456);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Bind ErrorCode : " << errCode << endl;
		return 0;
	}

	if (::listen(listenSocket, SO_MAX_CONN) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Listen ErrorCode : " << errCode << endl;
		return 0;
	}

	bool IsActive = true;

	while (IsActive)
	{
		SOCKADDR_IN clientAddr;
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addLen = sizeof(clientAddr);

		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addLen);
		if (clientSocket == INVALID_SOCKET)
		{
			int32 errCode = ::WSAGetLastError();
			cout << "Accept ErrorCode : " << errCode << endl;
			return 0;
		}

		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		cout << "> echo-server is activated" << endl;
		cout << "> client connected by IP address " << ipAddress << " with Port number " << ::ntohs(serverAddr.sin_port) << endl;

		while (IsActive)
		{
			char recvData[1000];
			int32 recvLen = ::recv(clientSocket, recvData, sizeof(recvData), 0);
			if (recvLen <= 0)
			{
				int32 errCode = ::WSAGetLastError();
				cout << "Receive ErrorCode : " << errCode << endl;
				return 0;
			}
			cout << "> echoed: " << recvData << endl;
			int32 res = ::send(clientSocket, recvData, recvLen, 0);
			if (res == SOCKET_ERROR)
			{
				INT32 errCode = ::WSAGetLastError();
				cout << "Send ErrorCode : " << errCode << endl;
				return 0;
			}
			if (!strncmp(recvData, "quit", sizeof("quit"))) IsActive = false;
		}
	}

	cout << "> echo-server is de-activated" << endl;

	::closesocket(listenSocket);
	::WSACleanup();
	system("pause");

	return 0;
}