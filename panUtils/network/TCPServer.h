#pragma once
#include "TCPConn.h"
#include "../thread/Locker.h"
#include <map>
#include <list>

namespace panutils {
	class TCPServer
	{
	public:
		TCPServer();
		~TCPServer();
		int Init(int port);
		int Start();
		int Stop();

		virtual void OnNewConn(std::shared_ptr<TCPConn> conn);//notify new connect
		virtual void OnData(std::shared_ptr<TCPConn> conn);//notify new data can read
		virtual void OnErr(std::shared_ptr<TCPConn> conn, int err);//notify conn err
	private:
		void EnableWrite(int fd);
		void NewFd(int fd, std::string addr, int port);//create conn,call onNewConn
		void RecvData(int fd, unsigned char *data, int size);//write data to conn cache
		void CloseFd(int fd);//close  and remove conn,call new fd
	private:
		int _port;
		std::map<int, std::shared_ptr<TCPConn>> _conns; 
		SpinLock _mtxConns;
	};

}

