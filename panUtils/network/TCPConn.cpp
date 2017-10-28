#include "TCPConn.h"
#include "SocketFunc.h"

namespace panutils {
	TCPConn::TCPConn(int fd):_fd(fd),_closed(false),_writeable(true),
		_pSend(nullptr)
	{
		_sendBuffer = new RingBuffer(0xfffffff);
		_recvBuffer = new RingBuffer(0xfffffff);
	}
	int TCPConn::SendData(unsigned char * data, int size)
	{
		if (nullptr==data||size<=0)
		{
			return -1;
		}

		_mtxStatus.Lock();
		if (true==_closed)
		{
			_mtxStatus.Unlock();
			return -1;
		}
		_mtxStatus.Unlock();

		_mtxStatus.Lock();
		_mtxSend.Lock();

		auto ret = _sendBuffer->Write(data, size);
		if (ret<0)
		{
			_mtxStatus.Unlock();
			_mtxSend.Unlock();
			return -1;
		}

		if (_writeable)
		{
			RealSend();
		}

		_mtxStatus.Unlock();
		_mtxSend.Unlock();

		return size;
	}
	int TCPConn::RecvData(unsigned char * data, int size)
	{
		_mtxRecv.Lock();
		if (size>_recvBuffer->CanRead())
		{
			size = _recvBuffer->CanRead();
			size = _recvBuffer->Read(data, size);
		}
		_mtxRecv.Unlock();
		return size;
	}
	int TCPConn::Recved(unsigned char * data, int size)
	{
		_mtxRecv.Lock();
		auto ret = _recvBuffer->Write(data, size);
		_mtxRecv.Unlock();
		return ret;
	}
	int TCPConn::Close()
	{
		_mtxStatus.Lock();
		if (_closed==false)
		{
			CloseSocket(_fd);
			_fd = -1;
			_closed = true;
			_writeable = false;
		}
		_mtxStatus.Unlock();
		return 0;
	}
	void TCPConn::EnableWrite(bool enable)
	{
		_mtxStatus.Lock();
		_writeable = enable;
		if (_writeable&&(nullptr!=_pSend||_sendBuffer->CanRead()>0))
		{
			_mtxStatus.Unlock();
			_mtxSend.Lock();
			RealSend();
			_mtxSend.Unlock();
		}

		_mtxStatus.Unlock();
	}
	TCPConn::~TCPConn()
	{
		_mtxStatus.Lock();
		if (false==_closed)
		{
			CloseSocket(_fd);
			_fd = -1;
			_closed = true;
			_writeable = false;
		}
		_mtxStatus.Unlock();
		_mtxSend.Lock();
		delete _sendBuffer;
		_sendBuffer = nullptr;
		_mtxSend.Unlock();

		_mtxRecv.Lock();
		delete _recvBuffer;
		_recvBuffer = nullptr;
		_mtxRecv.Unlock();
	}
	void TCPConn::SetRemoteAddr(std::string remote_addr)
	{
		_remoteAddr = remote_addr;
	}
	void TCPConn::Setport(int port)
	{
		_port = port;
	}
	std::string TCPConn::GetRemoteAddr()
	{
		return _remoteAddr;
	}
	int TCPConn::GetPort()
	{
		return _port;
	}
	void TCPConn::RealSend()
	{
		//send ptr
		auto ret = RealSendPtr();
		if (ret<0)
		{
			return;
		}
		

		//read send buffer
		if (_sendBuffer->CanRead()>0)
		{
			_size_pSend = _sendBuffer->CanRead();
			_cur_pSend = 0;
			_pSend = _sendBuffer->Read(_size_pSend, _size_pSend);
			if (_pSend==nullptr||_size_pSend==0)
			{
				return;
			}
			ret = RealSendPtr();
			if (ret<0)
			{
				return;
			}
		}
		

	}
	int TCPConn::RealSendPtr()
	{
		/*
		linux use socket send
		windows use iocp
		*/
		while (_pSend != nullptr&&_cur_pSend<_size_pSend)
		{
			int err;
			auto sendResult = SocketSend(_fd, (const char*)_pSend + _cur_pSend, _size_pSend - _cur_pSend, err);
			if (err == E_SOCKET_WOULDBLOCK)
			{
				_writeable = false;
				return -1;
			}
			if (sendResult <= 0)
			{
				//socket closed or other error
				_writeable = false;
				return -1;
			}
			_cur_pSend += sendResult;
			if (_cur_pSend == _size_pSend)
			{
				delete[]_pSend;
				_pSend = nullptr;
				_cur_pSend = _size_pSend = 0;
			}
		}

		return 0;
	}
}