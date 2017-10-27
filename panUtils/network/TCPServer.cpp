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
		return 0;
	}


	int TCPServer::Start()
	{
		return 0;
	}

	int TCPServer::Stop()
	{
		return 0;
	}

	void TCPServer::OnNewConn(std::shared_ptr<TCPConn> conn)
	{
		//do nothing here
	}

	void TCPServer::OnData(std::shared_ptr<TCPConn> conn)
	{
		//do nothing here
	}

	void TCPServer::OnErr(std::shared_ptr<TCPConn> conn, int err)
	{
		//do nothing here
	}

	void TCPServer::EnableWrite(int fd)
	{
		_mtxConns.Lock();
		auto it	 = _conns.find(fd);
		if (it!=_conns.end())
		{
			it->second->EnableWrite(true);
		}
		_mtxConns.Unlock();
	}

	void TCPServer::NewFd(int fd, std::string addr, int port)
	{
		std::shared_ptr<TCPConn> conn(new TCPConn(fd));
		_mtxConns.Lock();
		_conns[fd] = conn;
		conn->SetRemoteAddr(addr);
		conn->Setport(port);
		_mtxConns.Unlock();
		/*
		notify
		*/
		OnNewConn(conn);
	}

	void TCPServer::RecvData(int fd, unsigned char * data, int size)
	{
		_mtxConns.Lock();
		auto it = _conns.find(fd);
		if (it==_conns.end())
		{
			_mtxConns.Unlock();
			return;
		}
		_mtxConns.Unlock();
		it->second->Recved(data, size);
		/*
		notify
		*/
		OnData(it->second);
	}

	void TCPServer::CloseFd(int fd) {
		_mtxConns.Lock();
		auto it = _conns.find(fd);
		if (it!=_conns.end())
		{
			it->second->Close();
			_conns.erase(it);
			/*
			notify
			*/
			OnErr(it->second, -1);
		}
		else {
			CloseSocket(fd);
		}
		_mtxConns.Unlock();
	}


}