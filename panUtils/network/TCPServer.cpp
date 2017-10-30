#include "TCPServer.h"
#include "SocketFunc.h"

namespace panutils {


	TCPServer::TCPServer():_port(0)
	{
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



}