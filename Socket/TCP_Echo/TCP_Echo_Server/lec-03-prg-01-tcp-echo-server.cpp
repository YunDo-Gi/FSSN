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

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);	
	serverAddr.sin_port = ::htons(65456);
	::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	::listen(listenSocket, SO_MAX_CONN);

	bool isActive = true;

	while (isActive)
	{
		SOCKADDR_IN clientAddr;
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 addLen = sizeof(clientAddr);
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addLen);

		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		cout << "> echo-server is activated" << endl;
		cout << "> client connected by IP address " << ipAddress << " with Port number " << ::ntohs(serverAddr.sin_port) << endl;

		while (isActive)
		{
			char recvData[1000];
			int32 recvLen = ::recv(clientSocket, recvData, sizeof(recvData), 0);
			cout << "> echoed: " << recvData << endl;
			int32 res = ::send(clientSocket, recvData, recvLen, 0);
			if (!strncmp(recvData, "quit", sizeof("quit"))) isActive = false;
		}
	}

	cout << "> echo-server is de-activated" << endl;

	::closesocket(listenSocket);
	::WSACleanup();
	system("pause");

	return 0;
}