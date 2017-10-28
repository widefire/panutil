#include "TCPServer.h"
#include "SocketFunc.h"

namespace panutils {


#ifdef _WIN32
	int TCPServer::Start()
	{
		//create a thread to loop 
		/*
		iocp use thread and task list
		epoll not
		*/
		return 0;
	}

	int TCPServer::Stop()
	{
		CloseSocket(_fd);
		_fd = -1;
		//wait for thread loop end;
		return 0;
	}
#endif

}