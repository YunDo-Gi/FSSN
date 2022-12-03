#pragma comment(lib, "ws2_32")
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

int main()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData)) return 0;
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Socket" << " ErrorCode : " << errCode << endl;
		return 0;
	}
	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Non Blocking" << " ErrorCode : " << errCode << endl;
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
		cout << "Bind" << " ErrorCode : " << errCode << endl;
		return 0;
	}
	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Listen" << " ErrorCode : " << errCode << endl;
		return 0;
	}

	cout << "> echo-server is activated" << endl;

	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);

	while (true)
	{
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK) continue;
			int32 errCode = ::WSAGetLastError();
			cout << "Accept" << " ErrorCode : " << errCode << endl;
			break;
		}
		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
		cout << "> client connected by IP address " << ipAddress << " with Port number " << ::ntohs(serverAddr.sin_port) << endl;

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
				int32 errCode = ::WSAGetLastError();
				cout << "Receive" << " ErrorCode : " << errCode << endl;
				break;
			}

			while (true)
			{
				if (::send(clientSocket, recvData, recvLen, 0) == SOCKET_ERROR)
				{
					if (::WSAGetLastError() == WSAEWOULDBLOCK) continue;
					int32 errCode = ::WSAGetLastError();
					cout << "Send" << " ErrorCode : " << errCode << endl;
					break;
				}
				cout << "> echoed: " << recvData << endl;
				break;
			}
		}
	}

	cout << "> echo-server is de-activated" << endl;

	::closesocket(listenSocket);
	::WSACleanup();
	system("pause");

	return 0;
}
