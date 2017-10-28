#pragma once

#include <time.h>
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/epoll.h>
#include <signal.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#endif // _WIN32

namespace panutils {

	const int E_SOCKET_ERROR = -1;
#ifdef _WIN32
	void	gettimeval(struct timeval *tp);
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

	/*
	create socket,bind and listen
	*/
	int SocketServerTCP(int port,int &fd);//return 0 succed

	void SocketSetTimeout(int fd, timeval time,bool b_recv);
}
