#pragma comment(lib, "ws2_32")

#include <iostream>
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
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(65456);
	::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	cout << "> echo-client is activated" << endl;

	while (true)
	{
		char sendMsg[100];
		cout << "> ";
		cin >> sendMsg;
		int32 resultCode = ::send(clientSocket, sendMsg, sizeof(sendMsg), 0);
		char recvData[1000];
		int32 recvLen = ::recv(clientSocket, recvData, sizeof(recvData), 0);
		cout << "> received: " << recvData << endl;
		if (!strncmp(sendMsg, "quit", sizeof("quit"))) break;
	}

	cout << "> echo-client is de-activated" << endl;

	::closesocket(clientSocket);
	::WSACleanup();
	system("pause");

	return 0;
}
