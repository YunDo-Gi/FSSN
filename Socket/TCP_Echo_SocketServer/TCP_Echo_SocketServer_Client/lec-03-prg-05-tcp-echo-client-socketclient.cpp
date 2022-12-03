#pragma comment(lib, "ws2_32")
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

using int32 = __int32;
using namespace std;

int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData)) return 0;
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) return 0;
	u_long on = 1;
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET) return 0;
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(65456);

	while (true)
	{
		if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK) continue;
			if (::WSAGetLastError() == WSAEISCONN) break;
			int32 errCode = ::WSAGetLastError();
			cout << "Connect" << " ErrorCode : " << errCode << endl;
			break;
		}
	}

	cout << "> echo-client is activated" << endl;

	char sendMsg[1000];
	while (true)
	{
		cout << "> "; cin >> sendMsg;

		if (::send(clientSocket, sendMsg, sizeof(sendMsg), 0) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK) continue;
			int32 errCode = ::WSAGetLastError();
			cout << "Send" << " ErrorCode : " << errCode << endl;
			break;
		}

		if (!strcmp(sendMsg, "quit")) break;

		while (true)
		{
			char recvData[1000];
			int32 recvLen = ::recv(clientSocket, recvData, sizeof(recvData), 0);
			if (recvLen == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSAEWOULDBLOCK) continue;
				int32 errCode = ::WSAGetLastError();
				cout << "Receive" << " ErrorCode : " << errCode << endl;
				break;
			}
			else if (recvLen == 0)
			{
				break;
			}
			cout << "> received: " << recvData << endl;
			break;
		}
	}

	::closesocket(clientSocket);
	::WSACleanup();
	system("pause");

	return 0;
}