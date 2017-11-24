#include "panUtils/thread/Locker.h"
#include "panUtils/structs/RingBuffer.h"
#include "panUtils/network/SocketFunc.h"
#include "socketTest.h"

int main(int argc, char **argv) {
	panutils::SocketInit();

	//TestClient();
	TestServer();

	panutils::SocketShutdown();
	return 1;
}