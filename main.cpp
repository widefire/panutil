#include "panUtils/thread/Locker.h"
#include "panUtils/structs/RingBuffer.h"
#include "panUtils/network/SocketFunc.h"
#include "socketTest.h"
#include "panUtils/baseUtils/BaseUtils.h"

int main(int argc, char **argv) {

	double a = 1.0, b = 1.01;
	auto ed = panutils::RealEqual(a, b, 0.1);
	float af = 1.0f, bf = 1.01f;
	auto ef = panutils::RealEqual(af,bf, 0.1f);

	panutils::SocketInit();

	//TestClient();
	TestServer();

	panutils::SocketShutdown();
	return 1;
}