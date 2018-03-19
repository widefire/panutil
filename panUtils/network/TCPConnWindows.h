#pragma once

#ifdef _WIN64
#define WINDOW_SYSTEM
#endif // _WIN64
#ifndef WINDOW_SYSTEM
#ifdef _WIN32
#define WINDOW_SYSTEM
#endif
#endif // !1

#ifdef WINDOW_SYSTEM

#include "ITCPConn.h"
#include "../structs/RingBuffer.h"
#include "SocketFunc.h"
#include <thread>
#include <mutex>
#include <string>

namespace panutils
{
	class TCPConnWindows;
	const int IOCP_RECV_SIZE = 1024;

	enum IOCP_IO_TYPE
	{
		IO_READ,
		IO_WRITE
	};

	struct IOCP_IO_DATA
	{
		IOCP_IO_DATA();
		OVERLAPPED overlapped;
		WSABUF wsaBuf;
		char buf[IOCP_RECV_SIZE];
		int len = 0;
		IOCP_IO_TYPE	ioType = IO_READ;
	};

	struct IOCP_HANDLER
	{
		std::shared_ptr<ITCPConn> conn ;
	};


	class TCPConnWindows :public ITCPConn, public std::enable_shared_from_this<TCPConnWindows>
	{
	public:
		TCPConnWindows(int fd);
		virtual ~TCPConnWindows();
		virtual int Send(const char *data, const int size) __override;
		virtual void Close() __override;
		int Sended(int size);
		void CallWindowRecv();
		const char *Buf();
	private:
		void CallWindowSend();
	private:
		std::mutex _mutex;
		int _fd = -1;
		RingBuffer _sendBuf;
		bool _sending = false;
		std::shared_ptr<IOCP_IO_DATA>	_recvOverlapped, _sendOverlapped;
	};


}

#endif // WINDOW_SYSTEM


