
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
		_epfd = epoll_create1(0);
		if (-1 == _epfd) {
			return -1;
		}
		epoll_event ev;
		ev.data.fd = _fd;
		ev.events = EPOLLIN | EPOLLET;
		auto ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, _fd, &ev);
		if (-1 == ret) {
			return -1;
		}
		std::cout << __FILE__ << __LINE__ << " begin loop" << std::endl;
		std::thread loopThread(std::mem_fun(&TCPServer::EpollLoop), this);
		_threadEpoll = std::move(loopThread);

		return 0;
	}

	void TCPServer::EpollLoop() {
		_endEpoll = false;
		int nfds;
		const int EPOLL_MAX_EVENT = 128;
		const int EPOLL_RECV_SIZE = 2048;
		struct epoll_event events[EPOLL_MAX_EVENT];
		struct epoll_event ev;
		char recvBuf[EPOLL_RECV_SIZE];
		std::cout << __FILE__ << __LINE__ << "begin while" << std::endl;
		while (_endEpoll == false) {
			nfds = epoll_wait(_epfd, events, EPOLL_MAX_EVENT, 1000);
			if (_endEpoll)
			{
				break;
			}
			/*
			监听的skocket只需要EpollIn就足够了，EpollErr和EpollHup会自动加上
			*/
			for (int i = 0; i < nfds; i++) {
				
				if ((events[i].events&EPOLLERR) ||
					(events[i].events&EPOLLHUP)) {
					/*
					close connect
					*/
					auto ptr = (ConnContainer*)(events[i].data.ptr);
					if (ptr == nullptr) {
						CloseSocket(events[i].data.fd);
					}
					else {
						ptr->conn->Close();
						delete ptr;
					}
					epoll_ctl(_epfd, EPOLL_CTL_DEL, events[i].data.fd, 0);
				}
				else if(events[i].data.fd==_fd){
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
						auto ptr = new ConnContainer();
						std::shared_ptr<TCPConn> tmpConn(new TCPConn(infd));
						ptr->conn = tmpConn;
						ptr->conn->SetRemoteAddr(hbuf);
						ptr->conn->Setport(atoi(sbuf));
						ev.data.ptr = ptr;
						this->OnNewConn(ptr->conn);
						epoll_ctl(_epfd, EPOLL_CTL_ADD, infd, &ev);
						break;
					}
				}
				else if (events[i].events & EPOLLIN) {
					/*
					read data or close connect
					*/
					RingBuffer buf(0xfffffff);
					auto errcode = 0,ret=0;
					bool closed = false;
					while (true) {
						ret = SocketRecv(events[i].data.fd, recvBuf, EPOLL_RECV_SIZE, errcode);
						if (ret > 0) {
							buf.Write((unsigned char*)recvBuf, ret);
						}
						else if (ret < 0 && (errcode == E_SOCKET_WOULDBLOCK ||
							errcode == E_SOCKET_INTR || errcode == E_SOCKET_AGAIN ||
							errcode == E_SOCKET_INPROGRESS))
						{
							break;
						}
						else if (ret == 0) {
							closed = true;
							break;
						}
						else {
							break;
						}
					}
					if (buf.CanRead() > 0) {
						auto ptr = (ConnContainer*)(events[i].data.ptr);
						if (nullptr != ptr) {
							auto size = buf.CanRead();
							auto data = buf.Read(size, size);
							ptr->conn->Recved(data, size);
							delete[]data;
						}
					}
					if (closed) {
						auto ptr = (ConnContainer*)(events[i].data.ptr);
						if (ptr == nullptr) {
							CloseSocket(events[i].data.fd);
						}
						else {
							ptr->conn->Close();
							delete ptr;
						}
						epoll_ctl(_epfd, EPOLL_CTL_DEL, events[i].data.fd, 0);
					}

				}
				else if (events[i].events&EPOLLOUT) {
					/*
					write able
					*/
					auto ptr = (ConnContainer*)(events[i].data.ptr);
					if (ptr != nullptr) {
						ptr->conn->EnableWrite(true);
					}
				}
			}
		}
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
#endif // !_WIN32

	
}