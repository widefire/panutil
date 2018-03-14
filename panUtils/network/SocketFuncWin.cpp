

#include "SocketFunc.h"

#if defined(WINDOW_SYSTEM)

namespace panutils {
	void	gettimeval(struct timeval *tp) {
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;
		GetLocalTime(&wtm);
		tm.tm_year = wtm.wYear - 1900;
		tm.tm_mon = wtm.wMonth - 1;
		tm.tm_mday = wtm.wDay;
		tm.tm_hour = wtm.wHour;
		tm.tm_min = wtm.wMinute;
		tm.tm_sec = wtm.wSecond;
		tm.tm_isdst = -1;
		clock =(long) mktime(&tm);
		tp->tv_sec = (long)clock;
		tp->tv_usec = wtm.wMilliseconds * 1000;
		return ;
	}

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
		auto result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof opt);
		if (0 != result)
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

	int SocketSend(int fd, const char *buf, int len, int &err, bool block) {
		if (fd<0 || nullptr == buf || len <= 0)
		{
			return 0;
		}
		auto result = 0;
		err = 0;
		if (block)
		{
			result = send(fd, buf, len, 0);
			if (result<0)
			{
				err = SocketError();
			}
		}
		else {
			result = send(fd, buf, len, 0);
			if (result<0)
			{
				err = SocketError();
				if ((err == E_SOCKET_WOULDBLOCK ||
					err == E_SOCKET_INTR || err == E_SOCKET_AGAIN ||
					err == E_SOCKET_INPROGRESS || err == E_SOCKET_NOBUFS)) {
					err = E_SOCKET_AGAIN;
				}
			}
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
			//while ((result = recv(fd, buf, len, 0)) < 0 && E_SOCKET_INTR == (err = SocketError()));
			result = recv(fd, buf, len, 0);
			if (result<0)
			{
				err = SocketError();
				if ((err == E_SOCKET_WOULDBLOCK ||
					err == E_SOCKET_INTR || err == E_SOCKET_AGAIN ||
					err == E_SOCKET_INPROGRESS || err == E_SOCKET_NOBUFS)) {
					err = E_SOCKET_AGAIN;
				}
			}
		}

		return result;
	}

	int SocketClientTCP(const char *hostname, int port) {

		auto host = gethostbyname(hostname);
		if (nullptr==host)
		{
			return -1;
		}
		for (int i = 0; nullptr!=host->h_addr_list[i]; i++)
		{
			int fd;
			sockaddr_in svrAddr;

			memset(&svrAddr, 0, sizeof svrAddr);
			svrAddr.sin_family = AF_INET;
			svrAddr.sin_addr = *(in_addr*)host->h_addr_list[0];
			svrAddr.sin_port = htons(port);

			fd = (int)socket(AF_INET, SOCK_STREAM, 0);
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
		return (int)socket(AF_INET, SOCK_DGRAM, 0);
	}

	int SocketBufSize(int fd, bool bsend) {
		int bufSize;
		int sizeSize = sizeof(bufSize);
		auto opt = bsend ? SO_SNDBUF : SO_RCVBUF;
		getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&bufSize, &sizeSize);

		if (bufSize<1500)
		{
			bufSize = 1500;
		}

		return bufSize;
	}

	int SocketServerTCP(int port, int &fd) {
		auto err = 0;

		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_addr.s_addr = htons(INADDR_ANY);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);

		fd = (int)socket(AF_INET, SOCK_STREAM, 0);
		//fd = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
		if (fd<0)
		{
			err = SocketError();
			CloseSocket(fd);
			fd = -1;
			return err;
		}

		err=SetSocketNoBlocking(fd, false);
		if (err!=0)
		{
			CloseSocket(fd);
			fd = -1;
			return err;
		}

		err=bind(fd, ((const sockaddr*)&addr), sizeof(addr));
		if (err!=0)
		{
			CloseSocket(fd);
			fd = -1;
			return err;
		}

		err = listen(fd,SOMAXCONN );
		if (err!=0)
		{
			CloseSocket(fd);
			fd = -1;
			return err;
		}

		return err;
	}

	void SocketSetTimeout(int fd, timeval time, bool b_recv) {
		int out = time.tv_sec * 1000 + time.tv_usec / 1000;
		if (b_recv)
		{
			setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)(&out), sizeof(out));
		}
		else
		{
			setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)(&out), sizeof(out));
		}
	}

	int GetIPPort(sockaddr_in *addr, std::string &ip, int &port) {
		if (nullptr==addr)
		{
			return -1;
		}
		ip= inet_ntoa(addr->sin_addr);
		port = addr->sin_port;
		return 0;
	}
}


#endif