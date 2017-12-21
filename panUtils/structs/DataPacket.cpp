#include <string.h>
#include "DataPacket.h"
namespace panutils {


	DataPacket::DataPacket() :_data(nullptr), _size(0),_type(0)
	{
	}

	DataPacket::DataPacket(unsigned char * data, int size, int type) :
		_data(nullptr), _size(0), _type(type)
	{
		if (nullptr != data&&size > 0)
		{
			_data = new unsigned char[size];
			memcpy(_data, data, size);
			_size = size;
		}
	}


	DataPacket::~DataPacket()
	{
		if (nullptr != _data)
		{
			delete[]_data;
			_data = nullptr;
		}
		_size = 0;
	}

	DataPacket::DataPacket(const DataPacket & rh) :_data(nullptr), _size(0)
	{
		_size = rh._size;
		_type = rh._type;
		if (nullptr != rh._data && 0 != rh._size)
		{
			_data = new unsigned char[rh._size];
			memcpy(_data, rh._data, _size);
		}
	}
	DataPacket & DataPacket::operator=(const DataPacket &rh)
	{
		// TODO: insert return statement here
		if (&rh==this)
		{
			return *this;
		}
		if (this->_data!=nullptr)
		{
			delete[]this->_data;
			this->_data = nullptr;
		}
		this->_size = rh._size;
		this->_type = rh._type;
		if (rh._data!=nullptr&&rh._size>0)
		{
			this->_data = new unsigned char[rh._size];
			memcpy(_data, rh._data, rh._size);
		}
		return *this;
	}
}