#include "SocketFunc.h"

int panutils::SocketSendAll(int fd, const char * buf, int len, int & err)
{
	int Sended = 0;
	while (Sended<len)
	{
		auto ret = panutils::SocketSend(fd, buf + Sended, len - Sended, err, true);
		if (ret<0)
		{
			break;
		}
		Sended += ret;
	}
	return Sended;
}
