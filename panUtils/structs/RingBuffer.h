#pragma once
#include <mutex>
namespace panutils {
	class RingBuffer
	{
	public:
		RingBuffer(int maxSize);
		~RingBuffer();
		int CanRead();//size can read
		int Write(unsigned char *data, int size);//size writed
		unsigned char* Read(int size, int &readed);//size readed,return new data 
		int Read(unsigned char *data, int size);
		unsigned char *GetPtr(int size, int &size_data);//get the ptr,not thread safe
		int Ignore(int size);//size ignored
		int Reset();//reset head tail
	private:
		int ApplyNewDataSpace();//return new size,-1 for failed
		int CalCanWrite();//calculate how many bytes can write
		int CalCanRead();
		void MoveDataToHead();

		RingBuffer(const RingBuffer&) = delete;
	private:
		static const int s_base_size;
		unsigned char *_data;
		int _max_size;
		int _size;//data can store=_size - 1;
		int _head;
		int _tail;//tail no data,if tail==head no data
		std::mutex _mtx;
	};
}