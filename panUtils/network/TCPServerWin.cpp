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
		return 0;
	}

	void TCPServer::EnableWrite(int fd)
	{

	}

	void TCPServer::NewFd(int fd, std::string addr, int port)
	{
		/*
		notify
		*/
		//OnNewConn(conn);
	}

	void TCPServer::CloseFd(int fd) {

	}
	void TCPServer::NewData(int fd, unsigned char *data, int size) {

	}
#endif

}