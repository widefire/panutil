#pragma once
namespace panutils {
	class DataPacket
	{
	public:
		DataPacket();
		/*
		copy data to data packet
		*/
		DataPacket(unsigned char *data, int size,int type=0);
		~DataPacket();
		DataPacket(const DataPacket&rh);
		DataPacket &operator=(const DataPacket &) ;
		unsigned char *_data = nullptr;
		int _size = 0;
		int _type = 0;
	};
}

