#include "TCPConn.h"
#include "SocketFunc.h"
#include "../baseUtils/BaseUtils.h"

namespace panutils {
	TCPConn::TCPConn(int fd) :_fd(fd), _closed(false), _writeable(true)
	{
		_sendBuffer = new RingBuffer(0xfffffff);
		_recvBuffer = new RingBuffer(0xfffffff);
		panutils::generateGUID64(_id);
	}
	int TCPConn::Send(unsigned char * data, int size)
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


		_mtxSend.Lock();
		
		auto ret = _sendBuffer->Write(data, size);
		if (ret<0)
		{
			_mtxSend.Unlock();
			return -1;
		}
#ifdef WINDOW_SYSTEM

		/*if (_writeable)
		{
			RealSend();
		}*/
		RealSend();
#else
		RealSend();
#endif

		
		_mtxSend.Unlock();

		return size;
	}

	void TCPConn::Sended(int size)
	{
	}

	//void TCPConn::Sended(int size)
	//{
	//	if (size>0)
	//	{
	//		_mtxSend.Lock();
	//		_sendBuffer->Ignore(size);
	//		_writeable = true;
	//		RealSend();
	//		_mtxSend.Unlock();
	//	}
	//}


	int TCPConn::Recved(unsigned char * data, int size)
	{
		return -1;
		//do nothing here
		_mtxRecv.Lock();
		auto ret = _recvBuffer->Write(data, size);
		_mtxRecv.Unlock();
		return ret;
	}
	std::shared_ptr<DataPacket> TCPConn::ReadAll()
	{
		_mtxRecv.Lock();
		if (_recvBuffer->CanRead() <= 0) {

			_mtxRecv.Unlock();
			return nullptr;
		}
		std::shared_ptr<DataPacket> pkt(new DataPacket());
		pkt->_size = _recvBuffer->CanRead();
		pkt->_data = new unsigned char[pkt->_size];
		_recvBuffer->Read(pkt->_data, pkt->_size);

		_mtxRecv.Unlock();
		return pkt;
	}
	int TCPConn::Close()
	{
		_mtxStatus.Lock();
		if (_closed==false)
		{
			CloseSocket(_fd);
			_fd = -1;
			_closed = true;
		}
		_mtxStatus.Unlock();

		_mtxSend.Lock();
		_writeable = false;
		_mtxSend.Unlock();

		return 0;
	}
	void TCPConn::EnableWrite(bool enable)
	{
		_mtxSend.Lock();
		_writeable = enable;
		if (_writeable&&_sendBuffer->CanRead()>0)
		{
			RealSend();
		}

		_mtxSend.Unlock();
	}
	std::string TCPConn::ID()
	{
		return _id;
	}
	TCPConn::~TCPConn()
	{
		_mtxStatus.Lock();
		if (false==_closed)
		{
			CloseSocket(_fd);
			_fd = -1;
			_closed = true;
		}
		_mtxStatus.Unlock();
		_mtxSend.Lock();
		delete _sendBuffer;
		_sendBuffer = nullptr;
		_writeable = false;
		_mtxSend.Unlock();

		_mtxRecv.Lock();
		if (_recvBuffer!=nullptr)
		{
			delete _recvBuffer;
			_recvBuffer = nullptr;
		}
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
	bool TCPConn::Closed()
	{
		return this->_closed;
	}
	void TCPConn::RealSend()
	{

		while (_sendBuffer->CanRead()>0)
		{
			auto size_send = _sendBuffer->CanRead();
			size_send = size_send < s_MTU ? size_send : s_MTU;
			auto ptr = _sendBuffer->GetPtr(size_send, size_send);
			int err=0;
//#ifdef WINDOW_SYSTEM
#if false
			DWORD	NumberOfBytesSent;
			DWORD	flags = 0;
			WSABUF wsaBuf;
			wsaBuf.buf = (char*)ptr;
			wsaBuf.len = size_send;
			WSASend(_fd, &wsaBuf, 1, &NumberOfBytesSent, flags, (LPWSAOVERLAPPED)_paramSocket, 0);
			_writeable = false;
			break;
#else
			auto sendResult = SocketSend(_fd, (const char*)ptr, size_send, err);
			if (sendResult <= 0 )
			{
				_writeable = false;
				return ;
			}
			_sendBuffer->Ignore(sendResult);
#endif // WINDOW_SYSTEM

		}

	}
	


}