#include <string>
#include "DataPacket.h"



DataPacket::DataPacket():_data(nullptr),_size(0)
{
}

DataPacket::DataPacket(unsigned char * data, int size):_data(nullptr),_size(0)
{
	if (nullptr!=data&&size>0)
	{
		_data = new unsigned char[size];
		memcpy(_data, data, size);
		_size = size;
	}
}


DataPacket::~DataPacket()
{
	if (nullptr!=_data)
	{
		delete[]_data;
		_data = nullptr;
	}
	_size = 0;
}

DataPacket::DataPacket(const DataPacket & rt) :_data(nullptr), _size(0)
{
	if (nullptr!=rt._data&&0!=rt._size)
	{
		_data = new unsigned char[rt._size];
		memcpy(_data, rt._data, _size);
		_size = rt._size;
	}
}
