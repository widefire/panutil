#include "TCPServer.h"
#include "SocketFunc.h"

namespace panutils {


	TCPServer::TCPServer() :_port(0), _fd(-1),_endLoop(true)
	{
#ifdef WINDOW_SYSTEM
		_hICompletionPort = nullptr;
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		_numThread = sysInfo.dwNumberOfProcessors * 2;
#else
		_epfd = -1;
#endif // WINDOW_SYSTEM

	}

	TCPServer::~TCPServer()
	{
		this->Stop();
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

	void TCPServer::OnNewData(std::shared_ptr<TCPConn> conn, unsigned char * data, int size)
	{
	}




}