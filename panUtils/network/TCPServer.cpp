#include "TCPServer.h"
#include "SocketFunc.h"

namespace panutils {


	TCPServer::TCPServer() :_port(0), _fd(-1),_endLoop(true)
	{
#ifdef _WIN32
		_hCompletionPort = nullptr;
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		_numThread = sysInfo.dwNumberOfProcessors * 2;
#else
		_epfd = -1;
#endif // _WIN32

	}

	TCPServer::~TCPServer()
	{
	}

	int TCPServer::Init(int port)
	{
		_port = port;

		auto err = SocketServerTCP(port, _fd);
		if (err!=0)
		{
			return err;
		}
		return 0;
	}

	void TCPServer::OnNewConn(std::shared_ptr<TCPConn> conn)
	{
		//do nothing here
	}


	void TCPServer::OnErr(std::shared_ptr<TCPConn> conn, int err)
	{
		//do nothing here
	}


}