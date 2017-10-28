#pragma once
#include "TCPConn.h"
#include "../thread/Locker.h"
#include <map>
#include <list>
#include <thread>

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
		void EnableWrite(std::shared_ptr<TCPConn> conn);
		void NewFd(std::shared_ptr<TCPConn> conn, std::string addr, int port);//create conn,call onNewConn
	private:
		struct ConnContainer
		{
			std::shared_ptr<TCPConn> conn;
		};
		int _port;
		int _fd;
#ifdef _WIN32
#else
		int _epfd;
		std::thread _threadEpoll;
		void EpollLoop();
		volatile bool _endEpoll;
#endif // _WIN32

	};

}

