#pragma once



#include "TCPConn.h"
#include "../thread/Locker.h"
#include <map>
#include <list>
#include <thread>
#include <vector>

#ifdef _WIN64
#define WINDOW_SYSTEM
#endif // _WIN64
#ifndef WINDOW_SYSTEM
#ifdef _WIN32
#define WINDOW_SYSTEM
#endif
#endif // !1

namespace panutils {
	class TCPServer
	{
	public:
		TCPServer();
		virtual ~TCPServer();
		/*
		return 0 succeed
		*/
		int Init(int port);
		int Start();
		int Stop();

		virtual void OnNewConn(std::shared_ptr<TCPConn> conn);//notify new connect
		virtual void OnErr(std::shared_ptr<TCPConn> conn, int err);//notify conn err,closed
		virtual void OnNewData(std::shared_ptr<TCPConn> conn, unsigned char *data, int size);
	private:

		void EnableWrite(int fd);
		void NewFd(int fd, std::string addr, int port);//create conn,call onNewConn
		void CloseFd(int fd);
		void NewData(int fd, unsigned char *data, int size);
#ifdef WINDOW_SYSTEM
		void IocpLoop();
		void RecvWorker(void *lpParam);
		void* _hICompletionPort;
#endif // _

	private:
		
		int _port;
		int _fd;
		volatile bool _endLoop;
		
#ifdef WINDOW_SYSTEM
		std::vector<std::map<int,std::shared_ptr<TCPConn>>> _vecConnPtr;
		std::vector<std::shared_ptr<RWLock>> _vecConnMtx;
		int _numThread;//for windows ,mul thread .
		std::thread _threadIocp;
#else
		std::map<int, std::shared_ptr<TCPConn>> _mapConn;//epoll onethread ,not need mutex
		int _epfd;
		std::thread _threadEpoll;
		void EpollLoop();
#endif // WINDOW_SYSTEM
		
	};

}

