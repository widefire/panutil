#include "socketTest.h"
#include "panUtils/network/SocketFunc.h"
#include <iostream>

void TestClient()
{
	//auto fd = panutils::SocketClientTCP("www.baidu.com", 80);
	auto fd = panutils::SocketClientTCP("127.0.0.1", 8081);
	const char *buf = "ahahaha\n";
	char cache[20];
	int err;
	//panutils::SetSocketNoBlocking(fd, false);
	int ret=panutils::SocketSend(fd, buf, 8,err);
	ret = panutils::SocketRecv(fd, cache, 20, err);
	std::cout << cache << std::endl;
}
