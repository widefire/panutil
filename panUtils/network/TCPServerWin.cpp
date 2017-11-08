#include "TCPServer.h"
#include "SocketFunc.h"
#include<chrono>

namespace panutils {


#ifdef _WIN32
	const int EPOLL_RECV_SIZE = 2048;
	struct PER_IO_DATA {
		OVERLAPPED overlapped;
		WSABUF wsaBuf;
		char buf[EPOLL_RECV_SIZE];
		int len;
		std::shared_ptr<TCPConn> conn;
	};

	struct PER_HANDLE_DATA
	{
		int	fd;
		sockaddr_in	clientAddr;
	};

	int TCPServer::Start()
	{
		_hICompletionPort = (void*)CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
		_hOCompletionPort = (void*)CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
		_endLoop = false;
		//create a thread to loop 
		/*
		iocp use thread and task list
		epoll not
		*/
		std::thread iocpThread(std::mem_fun(&TCPServer::IocpLoop), this);
		_threadIocp = std::move(iocpThread);


		return 0;
	}

	int TCPServer::Stop()
	{
		_endLoop = true;
		if (_hICompletionPort!=nullptr)
		{
			CloseHandle((HANDLE)_hICompletionPort);
			_hICompletionPort = nullptr;
		}
		if (_hOCompletionPort!=nullptr)
		{
			CloseHandle((HANDLE)_hOCompletionPort);
			_hOCompletionPort = nullptr;
		}
		CloseSocket(_fd);
		_fd = -1;

		if (_threadIocp.joinable())
		{
			_threadIocp.join();
		}
		return 0;
	}

	void TCPServer::EnableWrite(int fd)
	{

	}

	void TCPServer::NewFd(int fd, std::string addr, int port)
	{
		/*
		notify
		*/
		//OnNewConn(conn);
	}

	void TCPServer::CloseFd(int fd) {

	}
	void TCPServer::NewData(int fd, unsigned char *data, int size) {

	}


	void TCPServer::IocpLoop()
	{
		std::vector<std::thread> recvWorkers,sendWorkers;
		for (int i = 0; i < _numThread; i++)
		{
			std::thread thread_I(std::mem_fun(&TCPServer::RecvWorker), this, _hICompletionPort);
			recvWorkers.push_back(std::move(thread_I));

			std::thread thread_O(std::mem_fun(&TCPServer::SendWorker), this, _hOCompletionPort);
			sendWorkers.push_back(std::move(thread_O));

		}

		while (_endLoop==false)
		{
			SOCKADDR_IN	addrRemote;
			int addrLen = sizeof(addrRemote);
			auto infd = accept(_fd, (sockaddr*)&addrRemote, &addrLen);
			if (infd==-1)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}

			//new conn
			//recv
			//no send

		}


		for (auto i = 0; i < recvWorkers.size(); i++)
		{
			if (recvWorkers[i].joinable())
			{
				recvWorkers[i].join();
			}

			if (sendWorkers[i].joinable())
			{
				sendWorkers[i].join();
			}
		}
	}

	void TCPServer::RecvWorker(void * lpParam)
	{
	}

	void TCPServer::SendWorker(void * lpParam)
	{
	}
#endif

}