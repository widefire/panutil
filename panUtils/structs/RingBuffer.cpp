#include <string>
#include <string.h>
#include "RingBuffer.h"

namespace panutils {
	const int RingBuffer::s_base_size = 4;
	RingBuffer::RingBuffer(int maxSize) :_data(nullptr), _max_size(maxSize), _size(0), _head(0), _tail(0)
	{
		if (_max_size < s_base_size)
		{
			_max_size = s_base_size;
		}
		ApplyNewDataSpace();
	}

	RingBuffer::~RingBuffer()
	{
		if (nullptr != _data)
		{
			delete[]_data;
			_data = nullptr;
		}
	}

	int RingBuffer::CanRead()
	{
		std::lock_guard<std::mutex> guard(_mtx);
		if ((nullptr == _data))
		{
			return 0;
		}

		return _tail - _head;
	}

	unsigned char* RingBuffer::Read(int size, int &readed)
	{
		std::lock_guard<std::mutex> guard(_mtx);

		if (size <= 0)
		{
			readed = 0;
			return nullptr;
		}
		size = size < CalCanRead() ? size : CalCanRead();

		auto data_out = new	unsigned char[size];
		if (nullptr == data_out)
		{
			readed = 0;
			return nullptr;
		}

		memmove(data_out, _data + _head, size);
		_head += size;
		readed = size;

		return data_out;
	}

	int RingBuffer::Read(unsigned char * data, int size)
	{
		if (nullptr==data||0>=size)
		{
			return -1;
		}
		std::lock_guard<std::mutex> guard(_mtx);
		size= size < CalCanRead() ? size : CalCanRead();
		if (size<=0)
		{
			return -1;
		}

		memcpy(data, _data, size);
		_head += size;

		return size;
	}

	unsigned char * RingBuffer::GetPtr(int size, int & size_data)
	{
		std::lock_guard<std::mutex> guard(_mtx);
		size_data = size < CalCanRead() ? size : CalCanRead();
		if (size_data<=0)
		{
			return nullptr;
		}
		return _data + _head;
	}



	int RingBuffer::Write(unsigned char * data, int size)
	{
		std::lock_guard<std::mutex> guard(_mtx);
		if (nullptr == data || size == 0 || size > _max_size - 1 - CalCanRead())
		{
			return -1;
		}

		while (size > CalCanWrite())
		{
			if (ApplyNewDataSpace() < 0) {
				return -1;
			}
		}
		if (size+_tail>=_size)
		{
			MoveDataToHead();
		}

		memcpy(_data + _tail, data, size);
		_tail += size;


		return size;
	}

	int RingBuffer::Ignore(int size)
	{
		std::lock_guard<std::mutex> guard(_mtx);
		if (size <= 0)
		{
			return 0;
		}
		if (size > CalCanRead())
		{
			size = CalCanRead();
		}
		_head += size;
		return size;
	}

	int RingBuffer::Reset()
	{
		_head = _tail = 0;
		return 0;
	}

	int RingBuffer::ApplyNewDataSpace()
	{
		auto newSize = 0;
		if (_size >= _max_size)
		{
			return -1;
		}
		if (_size == 0)
		{
			//just base size
			newSize = s_base_size;
		}
		else
		{
			newSize = _size << 1;
			if (newSize > _max_size)
			{
				newSize = _max_size;
			}
		}

		auto pData = new unsigned char[newSize];
		if (pData == nullptr)
		{
			return -1;
		}
		if (nullptr != _data)
		{
			auto can_read = CalCanRead();
			if (can_read > 0)
			{
				MoveDataToHead();
				memcpy(pData, _data, can_read);
			}
			_head = 0;
			_tail = can_read;

			delete[]_data;
			_data = nullptr;
		}
		else
		{
			_head = _tail = 0;
		}

		_data = pData;
		_size = newSize;

		return newSize;
	}

	int RingBuffer::CalCanWrite()
	{
		auto can_write = _size - 1 - CalCanRead();
		if (can_write < 0)
		{
			can_write = 0;
		}
		return can_write;
	}

	int RingBuffer::CalCanRead()
	{
		if ((nullptr == _data))
		{
			return 0;
		}

		return _tail - _head;
	}

	void RingBuffer::MoveDataToHead()
	{
		if (_head!=_tail&&_data!=nullptr)
		{
			memmove(_data, _data + _head, _tail - _head);
			_tail -= _head;
			_head = 0;
		}
		else
		{
			_head = _tail = 0;
		}
	}

}
