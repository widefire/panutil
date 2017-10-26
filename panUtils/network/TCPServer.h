#pragma once
namespace panutils {
	class TCPServer
	{
	public:
		TCPServer();
		~TCPServer();
		int Init(int port);
		int Start();
		int Stop();
		virtual void NewFd(int fd);//add fd 
		virtual void FdClosed(int fd);//remove fd
		virtual void CloseFd(int fd);//close and remove fd
		virtual void OnData(unsigned char *data, int size);//recv data,notify fd TCPConn
	private:

	};

}

