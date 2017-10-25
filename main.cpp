#include "panUtils/thread/Locker.h"
#include "panUtils/structs/RingBuffer.h"

int main(int argc, char **argv) {

	RWLock rwl;
	rwl.RLock();
	rwl.RLock();
	rwl.RUnlock();
	rwl.RUnlock();
	rwl.Lock();
	rwl.Unlock();

	RingBuffer ring_buffer(20);

	for (int i = 0; i < 5; i++)
	{
		unsigned char data = 'a' + i;
		ring_buffer.Write(&data, 1);
	}
	auto can_read = ring_buffer.CanRead();
	auto buf = ring_buffer.Read(1, can_read);
	ring_buffer.Ignore(1);
	unsigned char dat[2];
	ring_buffer.Read(dat, 2);
	return 1;
}