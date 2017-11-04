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
		int SendData(unsigned char *data, int size);
		/*
		recv from recv cache buf,return 0 nodata, -1 for error
		*/
		int RecvData(unsigned char *data, int size);
		/*
		append data to recv cache
		*/
		virtual int Recved(unsigned char *data, int size);
		/*
		close socket
		*/
		int Close();//close 
		void EnableWrite(bool enable);
		~TCPConn();
		void SetRemoteAddr(std::string remote_addr);
		void Setport(int port);
		std::string GetRemoteAddr();
		int GetPort();
	private:
		TCPConn(const TCPConn&) = delete;
		void RealSend();
		int RealSendPtr();

		int _fd;
		bool _closed;
		bool _writeable;
		int _port;
		std::string _remoteAddr;
		SpinLock _mtxStatus;//for closed and writeable
		RingBuffer *_sendBuffer;//buffer need send
		unsigned char *_pSend;
		int _size_pSend;
		int _cur_pSend;
		RingBuffer *_recvBuffer;//buffer read out
		SpinLock _mtxSend;
		SpinLock _mtxRecv;
		static const int s_MTU = 1500;
	};

}
