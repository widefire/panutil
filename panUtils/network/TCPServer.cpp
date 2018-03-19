#include "TCPServer.h"
#include "TCPServer.h"
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

	void TCPServer::SetDataCallback(ITCP_DATA_CALLBACK callback, std::shared_ptr<void> param)
	{
		_dataParam = param;
		_dataCallback = callback;
	}

	void TCPServer::SetErrCallback(ITCP_ERR_CALLBACK callback, std::shared_ptr<void> param)
	{
		_errParam = param;
		_errCallback = callback;
	}





}