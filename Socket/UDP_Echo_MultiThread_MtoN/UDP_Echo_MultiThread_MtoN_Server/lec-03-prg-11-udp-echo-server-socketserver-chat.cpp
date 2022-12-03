#include <iostream>
#include <cstring>
#include <unordered_map>
#include "Socket.h"

void HandleError(const char* cause);

unordered_map<USHORT, Socket> socketMap;
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

	socketMap.clear();
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
		if (!strncmp(recvData, "quit", sizeof("quit")))
		{
			if (socketMap.find(clientAddr.sin_port) != socketMap.end())
			{
				socketMap.erase(clientAddr.sin_port);
				continue;
			}
			continue;
		}
		else if (!strncmp(recvData, "#REG", sizeof("#REG")))
		{
			if (socketMap.find(clientAddr.sin_port) == socketMap.end())
			{
				socketMap[clientAddr.sin_port] = Socket(serverSocket, clientAddr);
				char ipAddress[16];
				::inet_ntop(AF_INET, &socketMap[clientAddr.sin_port].GetClientAddr().sin_addr, ipAddress, sizeof(ipAddress));
				cout << "> client registered ('" << ipAddress << "', " << ::ntohs(serverAddr.sin_port) << ")" << endl;
				continue;
			}
		}
		else if (!strncmp(recvData, "#DEREG", sizeof("#DEREG")))
		{
			if (socketMap.find(clientAddr.sin_port) != socketMap.end())
			{
				char ipAddress[16];
				::inet_ntop(AF_INET, &socketMap[clientAddr.sin_port].GetClientAddr().sin_addr, ipAddress, sizeof(ipAddress));
				cout << "> client de-registered ('" << ipAddress << "', " << ::ntohs(serverAddr.sin_port) << ")" << endl;
				socketMap.erase(clientAddr.sin_port);
				continue;
			}
		}

		if (socketMap.begin() == socketMap.end())
		{
			cout << "> no clients to echo" << endl;
			continue;
		}

		for (auto iter = socketMap.begin(); iter != socketMap.end(); ++iter)
		{
			int number = 0;
			int32 errorCode = ::sendto(iter->second.GetClientSocket(), recvData, recvLen, 0,
				(SOCKADDR*)&iter->second.GetClientAddr(), sizeof(iter->second.GetClientAddr()));
			if (errorCode == SOCKET_ERROR)
			{
				HandleError("SendTo");
				return 0;
			}
		}
		cout << "> received ( " << recvData << " ) and echoed to " << socketMap.size() << " clients" << endl;
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
