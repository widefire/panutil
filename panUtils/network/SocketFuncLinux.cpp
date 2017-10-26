#ifndef _WIN32
#include <signal.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include "SocketFunc.h"

namespace panutils {

void signalHandler(int n_signal) {

}

int SocketInit() {
	struct sigaction action;
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
	auto iRet = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof opt);
	
	if (iRet<0){
		return iRet;
}	

	opts = fcntl(fd, F_GETFL, 0);
	if (opts<0)
	{
		return opts;
	}
	if (block)
	{
		fcntl(fd, F_SETFL, opts&~O_NONBLOCK);
	}
	else {

		opts |= O_NONBLOCK;
		opts = fcntl(fd, F_SETFL, opts);
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

int SocketClientTCP(const char *hostname, int port) {

	auto host = gethostbyname(hostname);
	if (nullptr == host)
	{
		return -1;
	}
	for (int i = 0; nullptr != host->h_addr_list[i]; i++)
	{
		int fd;
		sockaddr_in svrAddr;

		memset(&svrAddr, 0, sizeof svrAddr);
		svrAddr.sin_family = AF_INET;
		svrAddr.sin_addr = *(in_addr*)host->h_addr_list[0];
		svrAddr.sin_port = htons(port);

		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd<0) {
			return fd;
		}

		auto ret = connect(fd, (struct sockaddr *)&svrAddr, sizeof(svrAddr));
		if (ret<0)
		{
			auto err = SocketError();
			CloseSocket(fd);
			continue;
		}
		else
		{
			return fd;
		}

	}


	return -1;

	}


int SocketClientUDP() {
	return socket(AF_INET, SOCK_DGRAM, 0);
}


int SocketServerTCP(int port, int &fd) {
	auto err = 0;

	char hostname[256];
	gethostname(hostname, 256);
	auto host = gethostbyname(hostname);

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_addr = *(in_addr*)host->h_addr_list[0];
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd<0)
	{
		err = SocketError();
		CloseSocket(fd);
		fd = -1;
		return err;
	}

	err = SetSocketNoBlocking(fd, false);
	if (err != 0)
	{
		CloseSocket(fd);
		fd = -1;
		return err;
	}

	err = bind(fd, ((const sockaddr*)&addr), sizeof(addr));
	if (err != 0)
	{
		CloseSocket(fd);
		fd = -1;
		return err;
	}

	err = listen(fd, SOMAXCONN);
	if (err != 0)
	{
		CloseSocket(fd);
		fd = -1;
		return err;
	}

	return err;
}


}
#endif
