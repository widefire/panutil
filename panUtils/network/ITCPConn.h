#pragma once

#include <memory>
#include <functional>
#include <mutex>

namespace panutils
{
	class ITCPConn
	{
	public:
		static std::shared_ptr<ITCPConn>	CreateConn(int fd);
		ITCPConn();
		virtual ~ITCPConn();
		/*
		return data sended
		return -1 for error.
		*/
		virtual int Send(const char *data, const int size) = 0 ;
		virtual void Close() = 0;
		std::string	remoteAddr;
	};

}

