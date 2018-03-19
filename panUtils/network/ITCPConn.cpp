#include "ITCPConn.h"
#include "TCPConnWindows.h"
#include "TCPConnLinux.h"

namespace panutils {

	std::shared_ptr<ITCPConn> ITCPConn::CreateConn(int fd)
	{
#ifdef WINDOW_SYSTEM
		return std::shared_ptr<TCPConnWindows>(new TCPConnWindows(fd));
#else
		return std::shared_ptr<TCPConnLinux>(new TCPConnLinux(fd));
#endif // WINDOW_SYSTEM

		return std::shared_ptr<ITCPConn>();
	}

	ITCPConn::ITCPConn()
	{
	}


	ITCPConn::~ITCPConn()
	{
	}
}

