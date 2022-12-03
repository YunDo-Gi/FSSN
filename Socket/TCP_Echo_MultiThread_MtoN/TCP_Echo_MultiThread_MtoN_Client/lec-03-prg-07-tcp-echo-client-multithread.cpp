#pragma comment(lib, "ws2_32")
#include <iostream>
#include <chrono>
#include <thread>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

using int32 = __int32;
using namespace std;

bool isActive = true;

void HandleError(const char* cause);
void SendData(SOCKET clientSocket);
void ReceiveData(SOCKET clientSocket);

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
			HandleError("Connect");
			break;
		}
	}

	cout << "> echo-client is activated" << endl;

	std::thread(SendData, clientSocket).detach();
	std::thread(ReceiveData, clientSocket).detach();

	while (isActive)
	{
	}

	cout << "> echo-client is de-activated" << endl;

	::closesocket(clientSocket);
	::WSACleanup();
	system("pause");

	return 0;
}

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

void SendData(SOCKET clientSocket)
{
	char sendMsg[1024];
	while (isActive)
	{
		cout << "> "; cin >> sendMsg;
		if (::send(clientSocket, sendMsg, sizeof(sendMsg), 0) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK) continue;
			HandleError("Send");
			break;
		}
		if (!strncmp(sendMsg, "quit", sizeof("quit"))) isActive = false;
	}
}

void ReceiveData(SOCKET clientSocket)
{
	char recvData[1024];
	while (isActive)
	{
		int32 recvLen = ::recv(clientSocket, recvData, sizeof(recvData), 0);
		if (recvLen == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK) continue;
			break;
		}
		else if (recvLen == 0) break;
		cout << "> received: " << recvData << endl;
	}
}