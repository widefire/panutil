#ifndef _WIN32
#include <signal.h>
#include "SocketFunc.h"


void signalHandler(int n_signal) {

}

int SocketInit() {
	sigaction action;
	action.sa_handler = signalHandler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGPIPE, &action, NULL);
	return 0;
}

int SocketShutdown() {
	return 0;
}
int SocketError() {
	return errno;
}

int CloseSocket(int fd) {
	return close(fd);
}

int SetSocketNoBlocking(int fd, bool block) {
	int opts;
	int opt = 1;
	iRet = setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof opt);

	opts = fcntl(socketId, F_GETFL, 0);
	if (opts<0)
	{
		return opts;
	}
	if (block)
	{
		fcntl(sockfd, F_SETFL, opts&~O_NONBLOCK);
	}
	else {

		opts |= O_NONBLOCK;
		opts = fcntl(socketId, F_SETFL, opts);
		if (opts<0)
		{
			return opts;
		}
	}
	return 0;
}

int SocketSend(int fd, const char *buf, int len, int &err, bool block) {
	if (fd<0 || nullptr == buf || len <= 0)
	{
		return 0;
	}
	auto result = 0;
	if (block)
	{
		result = send(fd, buf, len, MSG_NOSIGNAL);
		if (result<0)
		{
			err = SocketError();
		}
	}
	else {
		while ((result = send(fd, buf, len, MSG_NOSIGNAL)) < 0 && E_SOCKET_INTR == (err = SocketError()));
	}

	return result;
}

int SocketRecv(int fd, char *buf, int len, int &err, bool block) {
	if (fd<0 || nullptr == buf || len<1)
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