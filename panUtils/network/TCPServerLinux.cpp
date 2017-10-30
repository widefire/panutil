
#include "TCPServer.h"
#include "SocketFunc.h"
#include <iostream>

namespace panutils {

#ifndef _WIN32

	int TCPServer::Start()
	{
		//create a thread to loop 
		/*
		iocp use thread and task list
		epoll not
		*/
		
		std::cout << __FILE__ << __LINE__ << " begin loop" << std::endl;
		std::thread loopThread(std::mem_fun(&TCPServer::EpollLoop), this);
		_threadEpoll = std::move(loopThread);

		return 0;
	}

	void TCPServer::EpollLoop() {
		_epfd = epoll_create1(0);
		if (-1 == _epfd) {
			return -1;
		}

		const int EPOLL_MAX_EVENT = 128;
		const int EPOLL_RECV_SIZE = 2048;
		struct epoll_event events[EPOLL_MAX_EVENT];
		epoll_event ev;
		ev.data.fd = _fd;
		ev.events = EPOLLIN | EPOLLET;
		auto ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, _fd, &ev);
		if (-1 == ret) {
			return -1;
		}
		_endEpoll = false;
		int nfds;
		char recvBuf[EPOLL_RECV_SIZE];
		std::cout << __FILE__ << __LINE__ << "begin while" << std::endl;
		while (_endEpoll == false) {
			nfds = epoll_wait(_epfd, events, EPOLL_MAX_EVENT, -1);
			if (_endEpoll)
			{
				std::cout << __LINE__ << "end loop" << std::endl;
				break;
			}
			/*
			监听的skocket只需要EpollIn就足够了，EpollErr和EpollHup会自动加上
			*/
			if (nfds == -1) {

				std::cout << __LINE__ << " nfds " << nfds << std::endl;
				std::cout << SocketError() << std::endl;
				break;
			}
			for (int i = 0; i < nfds; i++) {
				std::cout << __LINE__ << " event " << events[i].events << std::endl;
				if ((events[i].events&EPOLLERR) ||
					(events[i].events&EPOLLHUP)) {
					/*
					close connect
					*/
					std::cout << __LINE__ << "err " << events[i].data.fd << std::endl;
					CloseFd(events[i].data.fd);
					epoll_ctl(_epfd, EPOLL_CTL_DEL, events[i].data.fd, 0);
				}
				else if(events[i].data.fd==_fd){
					std::cout << __LINE__ << "new conn" << std::endl;
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
						auto iRet = getnameinfo(&inAddr, addrLen,
							hbuf, sizeof hbuf,
							sbuf, sizeof sbuf,
							NI_NUMERICHOST | NI_NUMERICSERV);
						SetSocketNoBlocking(infd, false);
						ev.data.fd = infd;
						ev.events = EPOLLET | EPOLLIN | EPOLLOUT;
						epoll_ctl(_epfd, EPOLL_CTL_ADD, infd, &ev);
						NewFd(infd,hbuf,atoi(sbuf));
						break;
					}
				}
				else if (events[i].events & EPOLLIN) {
					/*
					read data or close connect
					*/
					std::cout << __LINE__ << " in" << events[i].data.fd << std::endl;
					auto errcode = 0,ret=0;
					while (true) {
						ret = SocketRecv(events[i].data.fd, recvBuf, EPOLL_RECV_SIZE, errcode);
						if (ret > 0) {
							NewData(events[i].data.fd, (unsigned char*)recvBuf, ret);
						}
						else if (ret < 0 && (errcode == E_SOCKET_WOULDBLOCK ||
							errcode == E_SOCKET_INTR || errcode == E_SOCKET_AGAIN ||
							errcode == E_SOCKET_INPROGRESS))
						{
							break;
						}
						else  {
							CloseFd(events[i].data.fd);
							epoll_ctl(_epfd, EPOLL_CTL_DEL, events[i].data.fd, 0);
							break;
						}
					}
					

				}
				else if (events[i].events&EPOLLOUT) {
					std::cout << __LINE__ << " out" << std::endl;
					EnableWrite(events[i].data.fd);
				}
			}
		}
		close(_epfd);
	}

	int TCPServer::Stop()
	{
		_endEpoll = true;
		CloseSocket(_fd);
		close(_epfd);
		if (_threadEpoll.joinable()) {
			_threadEpoll.join();
		}
		_fd = -1;
		//wait for thread loop end;
		return 0;
	}

	void TCPServer::EnableWrite(int fd)
	{
		auto it = _mapConn.find(fd);
		if (it != _mapConn.end()) {
			it->second->EnableWrite(true);
		}
	}

	void TCPServer::NewFd(int fd, std::string addr, int port)
	{
		std::shared_ptr<TCPConn> conn(new TCPConn(fd));
		auto it = _mapConn.find(fd);
		if (it != _mapConn.end())
		{
			it->second->Close();
		}
		_mapConn[fd] = conn;
		/*
		notify
		*/
		OnNewConn(conn);
	}

	void TCPServer::CloseFd(int fd) {
		auto it = _mapConn.find(fd);
		if (it != _mapConn.end())
		{
			it->second->Close();
			_mapConn.erase(it);
		}
	}
	void TCPServer::NewData(int fd, unsigned char *data, int size) {
		auto it = _mapConn.find(fd);
		if (it != _mapConn.end())
		{
			it->second->Recved(data, size);
		}
	}
#endif // !_WIN32

	
}