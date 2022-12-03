#pragma once
#pragma comment(lib, "ws2_32")
#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

using int32 = __int32;
using namespace std;

class Socket
{
private:
	SOCKET			clientSocket;
	SOCKADDR_IN		clientAddr;
	int				clientSize;

public:
	Socket()
	{
		::memset(&clientSocket, 0, sizeof(clientSocket));
		::memset(&clientAddr, 0, sizeof(clientAddr));
		clientSize = sizeof(clientAddr);
	}
	Socket(SOCKET socket, SOCKADDR_IN addr)
	{
		::memset(&clientSocket, 0, sizeof(clientSocket));
		::memset(&clientAddr, 0, sizeof(clientAddr));

		clientSocket = socket;
		clientAddr = addr;
		clientSize = sizeof(clientAddr);
	}
	~Socket()
	{
		::memset(&clientSocket, 0, sizeof(clientSocket));
		::memset(&clientAddr, 0, sizeof(clientAddr));
		clientSize = -1;
	}
	void	SetClientSocket(SOCKET socket)   { clientSocket = socket; }
	void	SetClientAddr(SOCKADDR_IN addr)	 { clientAddr = addr; }
	void	SetClinetSize(int size)			 { clientSize = size; }

public:
	SOCKADDR_IN&	GetClientAddr()			{ return clientAddr; }
	SOCKET&			GetClientSocket()		{ return clientSocket; }
	int&			GetClientSize()			{ return clientSize; }

};

