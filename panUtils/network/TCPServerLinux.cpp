
#include "TCPServer.h"
#include "SocketFunc.h"
#include <iostream>

#ifndef WINDOW_SYSTEM

#include "TCPConnLinux.h"

namespace panutils {


	int TCPServer::Start()
	{
		//create a thread to loop 
		/*
		iocp use thread and task list
		epoll not
		*/

		std::cout << __FILE__ << __LINE__ << " begin loop" << std::endl;
		_threadEpoll = std::move(&TCPServer::EpollLoop, this);
		return 0;
	}

	void TCPServer::EpollLoop() {
		_epfd = epoll_create1(0);
		if (-1 == _epfd) {
			return;
		}

		const int EPOLL_MAX_EVENT = 128;
		const int EPOLL_RECV_SIZE = 2048;
		struct epoll_event events[EPOLL_MAX_EVENT];
		epoll_event ev;
		ev.data.fd = _fd;
		ev.events = EPOLLIN | EPOLLET;
		auto ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, _fd, &ev);
		if (-1 == ret) {
			return;
		}
		_endLoop = false;
		int nfds;
		char recvBuf[EPOLL_RECV_SIZE];

		while (_endLoop == false) {
			nfds = epoll_wait(_epfd, events, EPOLL_MAX_EVENT, -1);
			if (_endLoop)
			{
				std::cout << __LINE__ << "end loop" << std::endl;
				break;
			}

			if (nfds == -1) {

				std::cout << __LINE__ << " nfds " << nfds << std::endl;
				std::cout << SocketError() << std::endl;
				break;
			}
			for (int i = 0; i < nfds; i++) {

				if ((events[i].events&EPOLLERR) ||
					(events[i].events&EPOLLHUP)) {
					/*
					close connect
					*/

					CloseClient(events[i].data.fd);

					epoll_ctl(_epfd, EPOLL_CTL_DEL, events[i].data.fd, 0);
				}
				else if (events[i].data.fd == _fd) {

					/*
					new connect
					*/
					for (;;) {
						sockaddr inAddr;
						socklen_t addrLen;
						int infd;
						char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
						addrLen = sizeof inAddr;
						infd = accept(_fd, &inAddr, &addrLen);
						if (infd < 0) {
							break;
						}
						
						
						SetSocketNoBlocking(infd, false);
						ev.data.fd = infd;
						ev.events = EPOLLET | EPOLLIN | EPOLLOUT;
						epoll_ctl(_epfd, EPOLL_CTL_ADD, infd, &ev);

						this->NewClient(infd);
						auto it = _mapConn.find(fd);
						if (it != _mapConn.end())
						{
							auto client = it->second;
							char hostname[NI_MAXHOST];
							char servInfo[NI_MAXSERV];
							getnameinfo((const SOCKADDR*)&addrRemote, addrLen,
								hostname,
								NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV);
							client->hostname = hostname;

							struct sockaddr_in *ipv4 = &addrRemote;
							char ipAddress[INET_ADDRSTRLEN];
							inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);
							client->remoteAddr = ipAddress;
							client->port = ipv4->sin_port;

						}
						break;
					}
				}
				else if (events[i].events & EPOLLIN) {
					/*
					read data or close connect
					*/

					auto errcode = 0, ret = 0;
					while (true) {
						ret = SocketRecv(events[i].data.fd, recvBuf, EPOLL_RECV_SIZE, errcode);
						if (ret > 0) {
							this->NewData(events[i].data.fd, (unsigned char*)recvBuf, ret);
						}
						else if (ret < 0 && (errcode == E_SOCKET_WOULDBLOCK ||
							errcode == E_SOCKET_INTR || errcode == E_SOCKET_AGAIN ||
							errcode == E_SOCKET_INPROGRESS || errcode == E_SOCKET_NOBUFS))
						{
							break;
						}
						else {
							CloseClient(events[i].data.fd);
							epoll_ctl(_epfd, EPOLL_CTL_DEL, events[i].data.fd, 0);
							break;
						}
					}


				}
				else if (events[i].events&EPOLLOUT) {
					//std::cout << __LINE__ << " out" << std::endl;
					Sended(events[i].data.fd);

				}
			}
		}
		close(_epfd);
	}

	int TCPServer::Stop()
	{
		_endLoop = true;
		CloseSocket(_fd);
		close(_epfd);
		if (_threadEpoll.joinable()) {
			_threadEpoll.join();
		}
		_fd = -1;
		//wait for thread loop end;
		return 0;
	}

	void TCPServer::CloseClient(int fd)
	{
		auto it = _mapConn.find(fd);
		if (it != _mapConn.end())
		{
			it->second->Close();
			_mapConn.erase(fd);

		}
		else
		{
			panutils::CloseSocket(fd);
		}
	}

	void TCPServer::NewClient(int fd)
	{
		auto it = _mapConn.find(fd);
		if (it != _mapConn.end())
		{
			std::cout << "new client has existed" << std::endl;
			it->second->Close();
		}
		auto conn = ITCPConn::CreateConn(fd);
		_mapConn[fd] = conn;
		auto param = _dataParam.lock();
		if (param != nullptr)
		{
			this->_dataCallback(it->second, nullptr, 0, _dataParam.lock());
		}
	}

	void TCPServer::NewData(int fd, const char * data, int size)
	{
		auto it = _mapConn.find(fd);
		if (it != _mapConn.end())
		{
			auto conn = dynamic_cast<TCPConnLinux*>(it->second.get());
			if (conn != nullptr)
			{
				auto param = _dataParam.lock();
				if (param != nullptr)
				{
					this->_dataCallback(it->second, data, size, _dataParam.lock());
				}
			}
			else
			{
				std::cout << "cast NewData error" << std::endl;
			}
		}
		else
		{
			std::cout << "NewData err" << std::endl;
		}
	}

	void TCPServer::Sended(int fd)
	{
		auto it = _mapConn.find(fd);
		if (it != _mapConn.end())
		{
			auto conn = dynamic_cast<TCPConnLinux*>(it->second.get());
			if (conn != nullptr)
			{
				conn->EnableWrite();
			}
			else
			{
				std::cout << "cast sended error" << std::endl;
			}
		}
		else
		{
			std::cout << "sended err" << std::endl;
		}
	}

}
#endif // !WINDOW_SYSTEM

	