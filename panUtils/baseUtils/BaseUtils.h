#pragma once

#include <memory>
#include <string>
#include <stdio.h>
#include <time.h>
#include <string>
#include <thread>
#include <map>
#include <list>
#include<mutex>
#include<condition_variable>
#include <vector>
#ifdef _WIN64
#define WINDOW_SYSTEM
#endif // _WIN64
#ifndef WINDOW_SYSTEM
#ifdef _WIN32
#define WINDOW_SYSTEM
#endif
#endif // !1



#ifdef WINDOW_SYSTEM

#include <winsock2.h>
#include <Windows.h>
#include <objbase.h>
#include <direct.h>
#else
#include <uuid/uuid.h>

#include<sys/stat.h>
#include<sys/types.h>
#include <dirent.h>
#include <iconv.h>
#define GUID uuid_t
#include<sys/sem.h>
#include<semaphore.h>
#include<pthread.h>
union semun
{
	int val;
	struct semid_ds *buf;
	ushort* seminfo;
	void* __pad;
};
#endif

#ifndef BYTE_ORDER

#define LITTLE_ENDIAN	1234
#define BIG_ENDIAN	4321

#if defined(sun) || defined(__BIG_ENDIAN) || defined(NET_ENDIAN)
#define BYTE_ORDER	BIG_ENDIAN
#else
#define BYTE_ORDER	LITTLE_ENDIAN
#endif

#endif

#include "../structs/DataPacket.h"

namespace panutils {

	std::shared_ptr<DataPacket>	ReadFileAll(std::string fileName);
	void generateGUID64(std::string &strGUID);
	long long timeofMillSecond();
	struct panDate
	{
		int tm_sec;   // seconds after the minute - [0, 60] including leap second
		int tm_min;   // minutes after the hour - [0, 59]
		int tm_hour;  // hours since midnight - [0, 23]
		int tm_mday;  // day of the month - [1, 31]
		int tm_mon;   // months since January - [0, 11]
		int tm_year;  // years since 1900
		int tm_wday;  // days since Sunday - [0, 6]
		int tm_yday;  // days since January 1 - [0, 365]
	};
	panDate	timeofDate();

	void SleepMillsec(long long millSec);

	int createDirectory(const char *dirName);

	void ToLittleEndianBuf(void * bufSrc, int srcSize, int srcStride, unsigned char * bufDst);
	void ToBigEndianBuf(void *bufSrc, int srcSize, int srcStride, unsigned char *bufDst);
	unsigned int DecodeIntBigBuf(unsigned char *buf, int size);
	unsigned int DecodeIntLitBuf(unsigned char *buf, int size);

	std::shared_ptr<std::vector<char>>	Locale_2_UTF_8(const char *input, int inputLen);
	std::shared_ptr<std::vector<char>> UTF_8_2_Locale(const char *input, int inputLen);
}

