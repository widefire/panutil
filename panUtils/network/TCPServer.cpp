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

	void TCPServer::EnableWrite(std::shared_ptr<TCPConn> conn)
	{
		conn->EnableWrite(true);
	}

	void TCPServer::NewFd(std::shared_ptr<TCPConn> conn, std::string addr, int port)
	{
		
		conn->SetRemoteAddr(addr);
		conn->Setport(port);
		/*
		notify
		*/
		OnNewConn(conn);
	}




}