#include "socketTest.h"
#include "panUtils/network/SocketFunc.h"
#include "panUtils/network/TCPServer.h"
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
	std::cout << cache<<buf << std::endl;
	panutils::TCPServer svr;
	err=svr.Init(8080);
	if (err != 0) {
		std::cout <<__LINE__<< err << std::endl;
		return;
	}
	std::cout << __LINE__ << std::endl;
	std::shared_ptr<int> p(new int(3));
	std::shared_ptr<int> p2 = p;
	err=svr.Start();
	if (err != 0) {
		std::cout <<__LINE__<< err << std::endl;
		return;
	}
	std::cout << __LINE__ << std::endl;
	/*svr.Stop();
	std::cout << __LINE__ << " stoped" << std::endl;*/
	std::cin >> err;
}
