#pragma once


#ifdef _WIN64
#define WINDOW_SYSTEM
#endif // _WIN64
#ifndef WINDOW_SYSTEM
#ifdef _WIN32
#define WINDOW_SYSTEM
#endif
#endif // !1

#include <time.h>
#include <string>
#ifdef WINDOW_SYSTEM
#include <winsock2.h>
#include <Windows.h>
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
#endif // WINDOW_SYSTEM

namespace panutils {

	const int E_SOCKET_ERROR = -1;
#ifdef WINDOW_SYSTEM
	void	gettimeval(struct timeval *tp);
	const int E_SOCKET_WOULDBLOCK = WSAEWOULDBLOCK;
	const int E_SOCKET_INTR = WSAEINTR;
	const int E_SOCKET_AGAIN = WSAEWOULDBLOCK;
	const int E_SOCKET_INPROGRESS = WSAEINPROGRESS;
	const int E_SOCKET_NOBUFS = WSAENOBUFS;
#else
	const int E_SOCKET_WOULDBLOCK = EWOULDBLOCK;
	const int E_SOCKET_INTR = EINTR;
	const int E_SOCKET_AGAIN = EAGAIN;
	const int E_SOCKET_INPROGRESS = EINPROGRESS;
	const int E_SOCKET_NOBUFS = ENOBUFS;
#endif

	

	int SocketInit();
	int SocketShutdown();
	int SocketError();

	int CloseSocket(int fd);

	int SetSocketNoBlocking(int fd, bool block);

	int SocketSend(int fd, const char *buf, int len, int &err, bool block = false);
	int SocketRecv(int fd, char *buf, int len, int &err, bool block = false);

	int SocketSendAll(int fd, const char *buf, int len, int &err);

	int SocketClientTCP(const char* hostname, int port);
	int SocketClientUDP();
	int SocketBufSize(int fd, bool bsend);

	int GetIPPort(sockaddr_in *addr, std::string &ip, int &port);

	/*
	create socket,bind and listen
	*/
	int SocketServerTCP(int port,int &fd);//return 0 succed

	void SocketSetTimeout(int fd, timeval time,bool b_recv);


}
