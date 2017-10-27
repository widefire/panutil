#include <string>
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

		if (_head <= _tail)
		{
			return _tail - _head;
		}

		return _size + _tail - _head;
	}

	unsigned char* RingBuffer::Read(int size, int &readed)
	{
		std::lock_guard<std::mutex> guard(_mtx);

		if (size <= 0)
		{
			readed = 0;
			return 0;
		}
		if (size > CalCanRead())
		{
			size = CalCanRead();
		}

		auto data_out = new	unsigned char[size];
		if (nullptr == data_out)
		{
			readed = 0;
			return nullptr;
		}

		for (int i = 0; i < size; i++)
		{
			data_out[i] = _data[_head++];
			if (_head == _size)
			{
				_head = 0;
			}
		}

		readed = size;

		return data_out;
	}

	int RingBuffer::Read(unsigned char * data, int size)
	{
		std::lock_guard<std::mutex> guard(_mtx);
		if (size <= 0 || nullptr == data)
		{
			return 0;
		}

		if (size > CalCanRead())
		{
			size = CalCanRead();
		}

		for (int i = 0; i < size; i++)
		{
			data[i] = _data[_head++];
			if (_head == _size)
			{
				_head = 0;
			}
		}
		return size;
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
			ApplyNewDataSpace();
		}

		for (int i = 0; i < size; i++)
		{
			_data[_tail++] = data[i];
			if (_tail == _size)
			{
				_tail = 0;
			}
		}

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
		for (int i = 0; i < size; i++)
		{
			_head++;
			if (_head == _size)
			{
				_head = 0;
			}
		}
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
				for (int i = 0; i < can_read; i++)
				{
					pData[i] = _data[_head++];
					if (_head == _size)
					{
						_head = 0;
					}
				}
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

		if (_head <= _tail)
		{
			return _tail - _head;
		}

		return _size + _tail - _head;
	}

}