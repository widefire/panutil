#include "TCPServer.h"
#include "SocketFunc.h"
#include<chrono>

namespace panutils {


#ifdef _WIN32
	const int EPOLL_RECV_SIZE = 2048;
	struct PER_IO_DATA {
		PER_IO_DATA(){
			wsaBuf.buf = buf;
			wsaBuf.len = EPOLL_RECV_SIZE;
			len = 0;
		}
		OVERLAPPED overlapped;
		WSABUF wsaBuf;
		char buf[EPOLL_RECV_SIZE];
		int len;
		std::shared_ptr<TCPConn> conn;
	};

	struct PER_HANDLE_DATA
	{
		PER_HANDLE_DATA() {
			iodata = new PER_IO_DATA;
			std::shared_ptr<TCPConn> conn(new TCPConn(fd, (void*)&iodata->overlapped));
			iodata->conn = conn;
		}
		~PER_HANDLE_DATA()
		{
			delete iodata;
		}
		int	fd;
		sockaddr_in	clientAddr;
		PER_IO_DATA *iodata;
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

			auto handleData = new PER_HANDLE_DATA;
			handleData->fd = infd;

			memcpy(&handleData->clientAddr, &addrRemote, addrLen);
			CreateIoCompletionPort((HANDLE)infd, _hICompletionPort, (ULONG_PTR)handleData, 0);
			CreateIoCompletionPort((HANDLE)infd, _hOCompletionPort, (ULONG_PTR)handleData, 0);

			OnNewConn(handleData->iodata->conn);
			//recv
			//no send

			DWORD	recvBytes;
			DWORD	flags = 0;
			WSARecv(handleData->fd, &(handleData->iodata->wsaBuf), 1, &recvBytes, &flags, &(handleData->iodata->overlapped), 0);
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
		HANDLE				completionPort = static_cast<HANDLE>(lpParam);
		DWORD				bytesTransferred;
		LPOVERLAPPED		lpOverlapped = 0;
		PER_HANDLE_DATA		*lpHandleData = nullptr;
		DWORD				recvBytes = 0;
		DWORD				flags = 0;
		int					iRet = 0;

		while (!_endLoop)
		{
			iRet = GetQueuedCompletionStatus(completionPort, &bytesTransferred, (PULONG_PTR)&lpHandleData,
				(LPOVERLAPPED*)&lpOverlapped, INFINITE);
			if (0==iRet)
			{
				lpHandleData->iodata->conn->Close();
				delete lpHandleData;
				OnErr(lpHandleData->iodata->conn, 0);
				continue;
			}

			auto iodata=(PER_IO_DATA*)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, overlapped);
			if (0==bytesTransferred)
			{
				lpHandleData->iodata->conn->Close();
				delete lpHandleData;
				OnErr(lpHandleData->iodata->conn, 0);
				continue;
			}

			lpHandleData->iodata->conn->Recved((unsigned char*)lpHandleData->iodata->wsaBuf.buf,
				bytesTransferred);
			WSARecv(lpHandleData->fd, &(lpHandleData->iodata->wsaBuf), 1, &recvBytes, &flags, &(lpHandleData->iodata->overlapped), 0);
		}
	}

	void TCPServer::SendWorker(void * lpParam)
	{
		HANDLE				completionPort = static_cast<HANDLE>(lpParam);
		DWORD				bytesTransferred;
		LPOVERLAPPED		lpOverlapped = 0;
		PER_HANDLE_DATA		*lpHandleData = nullptr;
		DWORD				recvBytes = 0;
		DWORD				flags = 0;
		int					iRet = 0;

		while (!_endLoop) 
		{
			iRet = GetQueuedCompletionStatus(completionPort, &bytesTransferred, (PULONG_PTR)&lpHandleData,
				(LPOVERLAPPED*)&lpOverlapped, INFINITE);
			if (0 == iRet)
			{
				lpHandleData->iodata->conn->Close();
				delete lpHandleData;
				OnErr(lpHandleData->iodata->conn, 0);
				continue;
			}

			auto iodata = (PER_IO_DATA*)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, overlapped);
			if (0 == bytesTransferred)
			{
				lpHandleData->iodata->conn->Close();
				delete lpHandleData;
				OnErr(lpHandleData->iodata->conn, 0);
				continue;
			}

			lpHandleData->iodata->conn->Sended(bytesTransferred);
		}
	}
#endif

}