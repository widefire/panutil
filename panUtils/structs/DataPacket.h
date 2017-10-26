#pragma once
namespace panutils {
	class DataPacket
	{
	public:
		DataPacket();
		/*
		copy data to data packet
		*/
		DataPacket(unsigned char *data, int size);
		~DataPacket();
		DataPacket(const DataPacket&rt);
		unsigned char *_data;
		int _size;
	};
}

