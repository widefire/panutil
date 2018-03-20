#pragma once

#include "ITCPConn.h"
#include "SocketFunc.h"
#include "../structs/RingBuffer.h"

namespace panutils
{
	class TCPConnLinux :public ITCPConn, public std::enable_shared_from_this<TCPConnLinux>
	{
	public:
		TCPConnLinux(int fd);
		virtual ~TCPConnLinux();
		virtual int Send(const char *data, const int size) override;
		virtual void Close() override;
		void	EnableWrite();
	private:
		void SendLinux();
	private:
		std::mutex	_mutex;
		int _fd = -1;
		RingBuffer _sendBuf;
		bool _writeable = true;
	};
}


