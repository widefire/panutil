#pragma once



//#include "TCPConn.h"
#include "../thread/Locker.h"
#include <map>
#include <list>
#include <thread>
#include <vector>
#include "ITCPConn.h"

#ifdef _WIN64
#define WINDOW_SYSTEM
#endif // _WIN64
#ifndef WINDOW_SYSTEM
#ifdef _WIN32
#define WINDOW_SYSTEM
#endif
#endif // !1

namespace panutils {


	/*
	when data is null,it is new connect
	*/
	typedef  std::function<void(std::shared_ptr<ITCPConn> conn,const char*, const int, std::shared_ptr<void> param)> ITCP_DATA_CALLBACK;
	/*
	socket error
	*/
	typedef  std::function<void(std::shared_ptr<ITCPConn> conn,const int, std::shared_ptr<void> param)> ITCP_ERR_CALLBACK;

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
		void SetDataCallback(ITCP_DATA_CALLBACK callback, std::shared_ptr<void> param);
		void SetErrCallback(ITCP_ERR_CALLBACK callback, std::shared_ptr<void> param);

	private:

#ifdef WINDOW_SYSTEM
		void IocpLoop();
		void RecvWorker(void *lpParam);
		void ProcessError(void* lpHandleData);
		void* _hICompletionPort;
#endif // _

	private:
		ITCP_DATA_CALLBACK	_dataCallback;
		std::weak_ptr<void>	_dataParam;
		ITCP_ERR_CALLBACK	_errCallback;
		std::weak_ptr<void>	_errParam;
		int _port;
		int _fd;
		volatile bool _endLoop;
		
		
#ifdef WINDOW_SYSTEM
		int _numThread;//for windows ,mul thread .
		std::thread _threadIocp;
#else
		std::map<int, std::shared_ptr<ITCPConn>> _mapConn;//epoll onethread ,not need mutex
		int _epfd;
		std::thread _threadEpoll;
		void EpollLoop();
		void CloseClient(int fd);
		void NewClient(int fd);
		void NewData(int fd, const char *data, int size);
		void Sended(int fd);
#endif // WINDOW_SYSTEM
		
	};

}

