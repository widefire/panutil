
#include "TCPConnWindows.h"
#ifdef WINDOW_SYSTEM

#include "SocketFunc.h"

namespace panutils {

	TCPConnWindows::TCPConnWindows(int fd) :
		_fd(fd)
	{
		_sendOverlapped = std::shared_ptr<IOCP_IO_DATA>(new IOCP_IO_DATA());
		_sendOverlapped->ioType = IO_WRITE;

		_recvOverlapped = std::shared_ptr<IOCP_IO_DATA>(new IOCP_IO_DATA());
		_recvOverlapped->ioType = IO_READ;
	}

	TCPConnWindows::~TCPConnWindows()
	{
	}

	int TCPConnWindows::Send(const char * data, const int size)
	{
		if (data == nullptr || size <= 0)
		{
			return 0;
		}
		std::lock_guard<std::mutex> gurad(_mutex);
		if (_fd<0)
		{
			return -1;
		}

		if (_sendBuf.Write((unsigned char*)data, size) != size)
		{
			return  -1;
		}

		if (!_sending)
		{
			CallWindowSend();
		}

		return 0;
	}

	void TCPConnWindows::Close()
	{
		std::lock_guard<std::mutex> guard(_mutex);
		if (_fd>0)
		{
			closesocket(_fd);
			_fd = -1;
		}
	}
	int TCPConnWindows::Sended(int size)
	{
		std::lock_guard<std::mutex> guard(_mutex);
		if (_fd<0)
		{
			return -1;
		}

		_sendBuf.Ignore(size);

		if (_sendBuf.CanRead()>0)
		{
			CallWindowSend();
		}
		else
		{
			_sending = false;
		}

		return 0;
	}
	void TCPConnWindows::CallWindowRecv()
	{
		std::lock_guard<std::mutex> guard(_mutex);
		DWORD recvBytes = 0;
		DWORD recvFlags = 0;
		WSARecv(_fd,&_recvOverlapped->wsaBuf,
			1,&recvBytes,
			&recvFlags, &_recvOverlapped->overlapped, 0);
	}

	const char * TCPConnWindows::Buf()
	{
		return _recvOverlapped->buf;
	}

	void TCPConnWindows::CallWindowSend()
	{
		auto size = _sendBuf.CanRead() < IOCP_RECV_SIZE ? _sendBuf.CanRead() : IOCP_RECV_SIZE;
		if (size<=0)
		{
			return;
		}
		_sending = true;
		DWORD bytesSend = size;
		DWORD sendFlags = 0;

		_sendBuf.Read((unsigned char*)_sendOverlapped->buf, size);
		WSASend(_fd,&_sendOverlapped->wsaBuf,1,
			&bytesSend,sendFlags,
			&_sendOverlapped->overlapped, 0);
	}

	IOCP_IO_DATA::IOCP_IO_DATA()
	{
		memset(&overlapped, 0, sizeof(OVERLAPPED));
		wsaBuf.buf = this->buf;
		wsaBuf.len = IOCP_RECV_SIZE;
		this->len = 0;
	}
}

#endif



