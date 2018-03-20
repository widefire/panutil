#include "TCPConnLinux.h"
#include <mutex>

namespace panutils
{
	TCPConnLinux::TCPConnLinux(int fd) :
		_fd(fd)
	{
	}


	TCPConnLinux::~TCPConnLinux()
	{
	}
	int TCPConnLinux::Send(const char * data, const int size)
	{
		if (data==nullptr||size<=0)
		{
			return -1;
		}
		std::lock_guard<std::mutex> guard(_mutex);
		if (_fd<0)
		{
			return -1;
		}
		if (size!=_sendBuf.Write((unsigned char*)data,size))
		{
			return -1;
		}
		if (!_sending)
		{
			this->SendLinux();
		}
		return 0;
	}
	void TCPConnLinux::Close()
	{
		std::lock_guard<std::mutex> guard(_mutex);
		if (_fd>=0)
		{
			panutils::CloseSocket(_fd);
			_fd = -1;
		}

	}
	void TCPConnLinux::EnableWrite()
	{
		std::lock_guard<std::mutex> guard(_mutex);
		if (_fd<0)
		{
			return ;
		}
		if (_sendBuf.CanRead()>0)
		{
			SendLinux();
		}
		else
		{
			_sending = false;
		}
	}
	void TCPConnLinux::SendLinux()
	{
		_sending = true;
		int size = _sendBuf.CanRead();
		if (size>0)
		{

			auto data = _sendBuf.GetPtr(size, size);
			int err;
			auto ret=panutils::SocketSend(_fd, (const char*)data, size,err);
			if (ret>0)
			{
				_sendBuf.Ignore(ret);
			}
			else
			{
				_sending = true;
			}
		}
	}
}

