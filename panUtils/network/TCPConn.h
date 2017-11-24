#pragma once
#include <memory>
#include <string>
#include "../thread/Locker.h"
#include "../structs/RingBuffer.h"
//used for TCP server connect ,no block.
namespace panutils {
	class TCPServer;
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
		//void Sended(int size);
		/*
		user rewrite it, self process data,not use cache and ReadData func
		*/
		virtual int Recved(unsigned char *data, int size);
		/*
		close socket
		*/
		int Close();//close 
		virtual ~TCPConn();
		void SetRemoteAddr(std::string remote_addr);
		void Setport(int port);
		std::string GetRemoteAddr();
		int GetPort();
		/*
		called by server
		*/
		void EnableWrite(bool enable);
	private:
		TCPConn(const TCPConn&) = delete;
		void RealSend();

		int _fd;
		bool _closed;
		bool _writeable;
		int _port;
		std::string _remoteAddr;
		SpinLock _mtxStatus;//for closed and writeable
		RingBuffer *_sendBuffer;//buffer need send
		SpinLock _mtxSend;
		static const int s_MTU = 1400;
	};

}
