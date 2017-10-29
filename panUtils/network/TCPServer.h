#pragma once
#include "TCPConn.h"
#include "../thread/Locker.h"
#include <map>
#include <list>
#include <thread>
#include <vector>

namespace panutils {
	class TCPServer
	{
	public:
		TCPServer();
		~TCPServer();
		int Init(int port);//return 0 succeed
		int Start();
		int Stop();

		virtual void OnNewConn(std::shared_ptr<TCPConn> conn);//notify new connect
		virtual void OnErr(std::shared_ptr<TCPConn> conn, int err);//notify conn err,closed
	private:

		void EnableWrite(int fd);
		void NewFd(int fd, std::string addr, int port);//create conn,call onNewConn
		void CloseFd(int fd);
		void NewData(int fd, unsigned char *data, int size);
	private:
		
		int _port;
		int _fd;
		std::vector<std::map<int,std::shared_ptr<TCPConn>>> _vecConnPtr;
		std::vector<std::shared_ptr<RWLock>> _vecConnMtx;
		int _numThread;//for windows ,mul thread . for linux less lock conflict
#ifdef _WIN32
#else
		int _epfd;
		std::thread _threadEpoll;
		void EpollLoop();
		volatile bool _endEpoll;
#endif // _WIN32
		
	};

}

