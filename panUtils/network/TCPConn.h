#pragma once
#include <memory>
#include <string>
//used for TCP server connect ,no block.

class TCPConn
{
public:

	TCPConn(int fd);
	/*
	send data to SendTask list
	*/
	int SendData(unsigned char *data,int size);
	/*
	recv from epoll or iocp,cached
	*/
	virtual int RecvData(unsigned char *data, int size) = 0;
	/*
	close socket,remove from send task and recv task
	*/
	int Close();
	void EnableWrite(bool enable);
	virtual ~TCPConn();
	void SetRemoteAddr(std::string remote_addr);
	void SetPort(int port);
	std::string GetRemoteAddr();
	int GetPort();
private:
	TCPConn(const TCPConn&) = delete;
};


