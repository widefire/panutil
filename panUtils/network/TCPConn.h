#pragma once
#include <memory>
#include <string>
#include "SocketFunc.h"
#include "../thread/Locker.h"
#include "../structs/RingBuffer.h"
#include "../structs/DataPacket.h"
//used for TCP server connect ,no block.

namespace panutils {
	class TCPServer;

	//typedef  void (*TCPConnReleaseFunc)(void *param);
#ifdef WINDOW_SYSTEM
	struct PER_IO_DATA
	{
		static const int IOCP_RECV_SIZE = 4096;
		OVERLAPPED overlapped;
		WSABUF wsaBuf;
		char buf[IOCP_RECV_SIZE];
		int len;
	};
#endif // WINDOW_SYSTEM


	class TCPConn
	{
	public:

		TCPConn(int fd);
		/*
		send data to send cache
		*/
		int Send(unsigned char *data, int size);
		/*
		for iocp ,send end,only called by iocp server
		*/
		void Sended(int size);
		/*
		user rewrite it, self process data,not use cache and ReadData func
		*/
		virtual int Recved(unsigned char *data, int size);

		std::shared_ptr<DataPacket> ReadAll();
		/*
		close socket
		*/
		int Close();//close 
		virtual ~TCPConn();
		void SetRemoteAddr(std::string remote_addr);
		void Setport(int port);
		std::string GetRemoteAddr();
		int GetPort();
		bool Closed();
		/*
		called by server
		*/
		void EnableWrite(bool enable);

		std::string ID();
		/*void * _usrData = nullptr;
		TCPConnReleaseFunc	_releasefunc = nullptr;*/
	private:
		TCPConn(const TCPConn&) = delete;
		void RealSend();

		int _fd;
		bool _closed;
		bool _writeable;
		int _port;
		std::string _remoteAddr;
		SpinLock _mtxStatus;//for closed and writeable
		RingBuffer *_sendBuffer=nullptr;//buffer need send
		SpinLock _mtxSend;
		static const int s_MTU = 1400;
		RingBuffer *_recvBuffer = nullptr;
		SpinLock _mtxRecv;
		std::string _id;
	};

}
