#pragma once

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#endif // _WIN32

namespace panutils {

	const int E_SOCKET_ERROR = -1;
#ifdef _WIN32
	const int E_SOCKET_WOULDBLOCK = WSAEWOULDBLOCK;
	const int E_SOCKET_INTR = WSAEINTR;
	const int E_SOCKET_AGAIN = WSAEWOULDBLOCK;
	const int E_SOCKET_INPROGRESS = WSAEINPROGRESS;
#else
	const int E_SOCKET_WOULDBLOCK = EWOULDBLOCK;
	const int E_SOCKET_INTR = EINTR;
	const int E_SOCKET_AGAIN = EAGAIN;
	const int E_SOCKET_INPROGRESS = EINPROGRESS;
#endif

	int SocketInit();
	int SocketShutdown();
	int SocketError();

	int CloseSocket(int fd);

	int SetSocketNoBlocking(int fd, bool block);

	int SocketSend(int fd, const char *buf, int len, int &err, bool block = false);
	int SocketRecv(int fd, char *buf, int len, int &err, bool block = false);

	int SocketClientTCP(const char* hostname, int port);
	int SocketClientUDP();

	int SocketServerTCP(int port,int &fd);//return 0 succed
}