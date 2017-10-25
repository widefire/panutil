#if defined(_WIN32)
#include <WinSock2.h>
#include "SocketFunc.h"
int SocketInit() {
	WORD wVersion;
	WSADATA wsaData;
	wVersion = MAKEWORD(2, 2);
	return WSAStartup(wVersion, &wsaData);
}
int SocketShutdown() {
	
	return WSACleanup();
}
int SocketError() {
	return WSAGetLastError();
}

int CloseSocket(int fd) {
	return closesocket(fd);
}

int SetSocketNoBlocking(int fd, bool block) {
	int opt = 1;
	auto result=setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof opt);
	if (0!=result)
	{
		return result;
	}
	u_long argp = 1;
	if (block)
	{
		argp = 0;
	}

	result = ioctlsocket(fd, FIONBIO, &argp);

	return result;
}

int SocketSend(int fd, const char *buf, int len,int &err, bool block) {
	if (fd<0||nullptr==buf||len<=0)
	{
		return 0;
	}
	auto result = 0;
	if (block)
	{
		result = send(fd, buf, len, 0);
		if (result<0)
		{
			err = SocketError();
		}
	}
	else {
		while ((result = send(fd, buf, len, 0)) < 0 && E_SOCKET_INTR == (err = SocketError()));
	}

	return result;
}
int SocketRecv(int fd,char *buf, int len,int &err, bool block) {
	if (fd<0||nullptr==buf||len<1)
	{
		return 0;
	}
	auto result = 0;
	if (block)
	{
		result = recv(fd, buf, len, 0);
		if (result<0)
		{
			err = SocketError();
		}
	}
	else {
		while ((result = recv(fd, buf, len, 0)) < 0 && E_SOCKET_INTR == (err = SocketError()));
	}

	return result;
}
#endif