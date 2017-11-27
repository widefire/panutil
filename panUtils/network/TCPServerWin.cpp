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
		PER_IO_DATA() {
			memset(&overlapped, 0, sizeof(OVERLAPPED));
			wsaBuf.buf = buf;
			wsaBuf.len = EPOLL_RECV_SIZE;
			len = 0;
		}
	};

	struct PER_HANDLE_DATA
	{
		PER_HANDLE_DATA(int fd,sockaddr_in addr) {
			this->fd = fd;
			memcpy(&this->clientAddr, &addr, sizeof(addr));
			iodata = new PER_IO_DATA;
			std::shared_ptr<TCPConn> conn(new TCPConn(fd));
			iodata->conn = conn;

			std::string ip;
			int port;
			GetIPPort(&addr, ip, port);
			conn->SetRemoteAddr(ip);
			conn->Setport(port);
		}
		~PER_HANDLE_DATA()
		{
			delete iodata;
		}
		int	fd;
		sockaddr_in	clientAddr;
		PER_IO_DATA *iodata;
	};
	//HANDLE hCompletion;
	int TCPServer::Start()
	{
		auto tmpHandle= CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
		_hICompletionPort = new HANDLE;
		memcpy(_hICompletionPort, &tmpHandle, sizeof(HANDLE));
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
			CloseHandle(*(HANDLE*)_hICompletionPort);
			delete (HANDLE*)_hICompletionPort;
			_hICompletionPort = nullptr;
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
		std::vector<std::thread> recvWorkers;
		for (int i = 0; i < _numThread; i++)
		{
			std::thread thread_I(std::mem_fun(&TCPServer::RecvWorker), this, _hICompletionPort);
			recvWorkers.push_back(std::move(thread_I));


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

			auto handleData = new PER_HANDLE_DATA(infd, addrRemote);

			memcpy(&handleData->clientAddr, &addrRemote, addrLen);
			//CreateIoCompletionPort((HANDLE)infd, hCompletion, (ULONG_PTR)handleData, 0);
			CreateIoCompletionPort((HANDLE)infd, (*(HANDLE*)_hICompletionPort), (ULONG_PTR)handleData, 0);
			memset(&handleData->iodata->overlapped, 0, sizeof(OVERLAPPED));
			handleData->iodata->wsaBuf.buf = handleData->iodata->buf;
			handleData->iodata->wsaBuf.len = EPOLL_RECV_SIZE;
			OnNewConn(handleData->iodata->conn);
			//recv
			//no send

			DWORD	recvBytes;
			DWORD	flags = 0;
			WSARecv(handleData->fd, &(handleData->iodata->wsaBuf), 1, 
				&recvBytes, &flags, &(handleData->iodata->overlapped), 0);
		}


		for (auto i = 0; i < recvWorkers.size(); i++)
		{
			if (recvWorkers[i].joinable())
			{
				recvWorkers[i].join();
			}

			/*if (sendWorkers[i].joinable())
			{
				sendWorkers[i].join();
			}*/
		}
	}

	void TCPServer::RecvWorker(void * lpParam)
	{
		if (nullptr==lpParam)
		{
			return;
		}
		HANDLE				completionPort = static_cast<HANDLE>(*(HANDLE*)lpParam);
		DWORD				bytesTransferred;
		LPOVERLAPPED		lpOverlapped = 0;
		PER_HANDLE_DATA		*lpHandleData = nullptr;
		DWORD				recvBytes = 0;
		DWORD				flags = 0;
		int					iRet = 0;

		while (!_endLoop)
		{
			iRet = GetQueuedCompletionStatus(completionPort, &bytesTransferred, 
				(PULONG_PTR)&lpHandleData,
				(LPOVERLAPPED*)&lpOverlapped, INFINITE);
			if (0==iRet)
			{
				lpHandleData->iodata->conn->Close();
				OnErr(lpHandleData->iodata->conn, 0);
				delete lpHandleData;
				continue;
			}

			auto iodata=(PER_IO_DATA*)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, overlapped);
			if (0==bytesTransferred)
			{
				lpHandleData->iodata->conn->Close();
				OnErr(lpHandleData->iodata->conn, 0);
				delete lpHandleData;
				continue;
			}

			lpHandleData->iodata->conn->Recved((unsigned char*)lpHandleData->iodata->wsaBuf.buf,
				bytesTransferred);
			WSARecv(lpHandleData->fd, &(lpHandleData->iodata->wsaBuf),
				1, &recvBytes, &flags, &(lpHandleData->iodata->overlapped), 0);
		}
	}

#endif

}