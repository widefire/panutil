#include "BaseUtils.h"
#include <string.h>

namespace panutils {

	std::shared_ptr<DataPacket> ReadFileAll(std::string fileName)
	{
		auto fp = fopen(fileName.c_str(), "rb");
		if (nullptr == fp)
		{
			return nullptr;
		}
		fseek(fp, 0, SEEK_END);
		auto size = ftell(fp);
		if (size <= 0)
		{
			fclose(fp);
			return nullptr;
		}

		fseek(fp, 0, SEEK_SET);
		std::shared_ptr<DataPacket> pkt(new DataPacket());
		pkt->_size = size;
		pkt->_data = new unsigned char[pkt->_size];
		fread(pkt->_data, pkt->_size, 1, fp);
		fclose(fp);

		return pkt;
	}

	void generateGUID64(std::string & strGUID)
	{
		GUID guid;
		char chGUID[64];
#ifdef WINDOW_SYSTEM
		CoCreateGuid(&guid);

		sprintf(chGUID, "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);

#else
		uuid_generate(reinterpret_cast<unsigned char *>(&guid));
		uuid_unparse(guid, chGUID);
#endif


		strGUID = chGUID;
	}


	long long timeofMillSecond()
	{
		return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
	}

	panDate timeofDate()
	{
		auto tt = std::chrono::system_clock::to_time_t
		(std::chrono::system_clock::now());
		auto pTime = localtime(&tt);
		panDate date;
		date.tm_sec = pTime->tm_sec;
		date.tm_min = pTime->tm_min;  
		date.tm_hour = pTime->tm_hour;
		date.tm_mday = pTime->tm_mday;
		date.tm_mon = pTime->tm_mon;  
		date.tm_year = pTime->tm_year;
		date.tm_wday = pTime->tm_wday;
		date.tm_yday = pTime->tm_yday;

		return date;
	}

	void SleepMillsec(long long millSec)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(millSec));
	}


	int checkDirectoryExist(const char * dirName)
	{
		int iRet = 0;
#ifdef WINDOW_SYSTEM
		WIN32_FIND_DATAA   wfd;
		BOOL rValue = FALSE;
		HANDLE hFind = FindFirstFileA(dirName, &wfd);
		if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			rValue = TRUE;
		}
		FindClose(hFind);
		if (rValue == TRUE)
		{
			iRet = 0;
		}
		else
		{
			iRet = -1;
		}
#else
		auto dirPtr = opendir(dirName);
		if (dirPtr == nullptr)
		{
			iRet = -1;
		}
		else
		{
			closedir(dirPtr);
		}
#endif // WIN32
		return iRet;
	}

	int createDirectory(const char * dirName)
	{
		int iRet = 0;
		if (0 != checkDirectoryExist(dirName))
		{
#ifdef WINDOW_SYSTEM
			iRet = mkdir(dirName);
#else
			//iRet = mkdir(dirName, S_IRWXU);
			//iRet = mkdir(dirName, S_IRWXO);
			iRet = mkdir(dirName, 0777);
#endif // WIN32
		}
		return iRet;
	}

	void ToLittleEndianBuf(void * bufSrc, int srcSize, int srcStride, unsigned char * bufDst)
	{
		if (bufSrc == nullptr || srcSize == 0 || srcStride == 0 || bufDst == nullptr)
		{
			return;
		}
#if (BYTE_ORDER==LITTLE_ENDIAN)
		memcpy(bufDst, bufSrc, srcSize);
#else
		auto count = srcSize / srcStride;
		unsigned char* ptrSrc = static_cast<unsigned char*>(bufSrc);
		int cur = 0;
		for (auto i = 0; i < count; i++)
		{
			for (auto j = 1; j <= srcStride; j++)
			{
				bufDst[cur++] = ptrSrc[i*srcStride + srcStride - j];
			}
		}
#endif // (BYTE_ORDER==LITTLE_ENDIAN)

	}

	void ToBigEndianBuf(void *bufSrc, int srcSize, int srcStride, unsigned char *bufDst) {
		if (bufSrc == nullptr || srcSize == 0 || srcStride == 0 || bufDst == nullptr)
		{
			return;
		}
#if (BYTE_ORDER!=LITTLE_ENDIAN)
		memcpy(bufDst, bufSrc, srcSize);
#else
		auto count = srcSize / srcStride;
		unsigned char* ptrSrc = static_cast<unsigned char*>(bufSrc);
		int cur = 0;
		for (auto i = 0; i < count; i++)
		{
			for (auto j = 1; j <= srcStride; j++)
			{
				bufDst[cur++] = ptrSrc[i*srcStride + srcStride - j];
			}
		}
#endif // (BYTE_ORDER==LITTLE_ENDIAN)

	}
	unsigned int DecodeIntBigBuf(unsigned char * buf, int size)
	{
		if (buf == nullptr || size <4 )
		{
			return -1;
		}

		unsigned int ret = 0;
#if (BYTE_ORDER==LITTLE_ENDIAN)
		ret = buf[0] << 24;
		ret |= buf[1] << 16;
		ret |= buf[2] << 8;
		ret |= buf[3];
#else	
		ret = buf[3] << 24;
		ret |= buf[2] << 16;
		ret |= buf[1] << 8;
		ret |= buf[0];
#endif
		return ret;
	}
	unsigned int DecodeIntLitBuf(unsigned char * buf, int size)
	{
		if (buf == nullptr || size <4)
		{
			return -1;
		}

		unsigned int ret = 0;

#if (BYTE_ORDER==LITTLE_ENDIAN)
		ret = buf[3] << 24;
		ret |= buf[2] << 16;
		ret |= buf[1] << 8;
		ret |= buf[0];
#else	
		ret = buf[0] << 24;
		ret |= buf[1] << 16;
		ret |= buf[2] << 8;
		ret |= buf[3];
#endif

		return ret;
	}
	std::shared_ptr<std::vector<char>> Locale_2_UTF_8(const char * input, int inputLen)
	{
#ifdef WINDOW_SYSTEM
		if (input == nullptr || inputLen <= 0)
		{
			return nullptr;
		}
		auto wlen = MultiByteToWideChar(CP_ACP, 0, input, inputLen, nullptr, 0);
		if (wlen <= 0)
		{
			return nullptr;
		}

		std::vector<wchar_t>	wTmp;
		wTmp.resize(wlen);
		MultiByteToWideChar(CP_ACP, 0, input, inputLen, wTmp.data(), wlen);

		auto outLen = WideCharToMultiByte(CP_UTF8, 0, wTmp.data(), wlen, nullptr, 0, 0, 0);
		if (outLen <= 0)
		{
			return	nullptr;
		}
		std::shared_ptr<std::vector<char>>	out(new std::vector<char>);
		out->resize(outLen + 1, 0);
		WideCharToMultiByte(CP_UTF8, 0, wTmp.data(), wlen, out->data(), outLen, 0, 0);

		return out;
#else
		if (input == nullptr || inputLen <= 0)
		{
			return nullptr;
		}
		auto conv = iconv_open("", "UTF-8");
		std::shared_ptr<std::vector<char>>	out(new std::vector<char>);
		size_t sinputLen=(size_t)inputLen;
		size_t outputLen = sinputLen << 1;
		out->resize(outputLen, 0);
		char* pOut=(char*)out->data();
		char* pIn=(char*)input;
		auto ret = iconv(conv, &pIn, &sinputLen, &pOut, &outputLen);
		if (ret==-1)
		{
			out = nullptr;
		}
		iconv_close(conv);
#endif // WINDOW_SYSTEM

	}
	std::shared_ptr<std::vector<char>> UTF_8_2_Locale(const char * input, int inputLen)
	{
#ifdef WINDOW_SYSTEM
		if (input == nullptr || inputLen <= 0)
		{
			return nullptr;
		}
		auto wlen = MultiByteToWideChar(CP_UTF8, 0, input, inputLen, nullptr, 0);
		if (wlen <= 0)
		{
			return nullptr;
		}

		std::vector<wchar_t>	wTmp;
		wTmp.resize(wlen);
		MultiByteToWideChar(CP_UTF8, 0, input, inputLen, wTmp.data(), wlen);

		auto outLen = WideCharToMultiByte(CP_ACP, 0, wTmp.data(), wlen, nullptr, 0, 0, 0);
		if (outLen <= 0)
		{
			return	nullptr;
		}
		std::shared_ptr<std::vector<char>>	out(new std::vector<char>);
		out->resize(outLen + 1, 0);
		WideCharToMultiByte(CP_ACP, 0, wTmp.data(), wlen, out->data(), outLen, 0, 0);

		return out;
#else
		if (input == nullptr || inputLen <= 0)
		{
			return nullptr;
		}
		auto conv = iconv_open("UTF-8", "");
		std::shared_ptr<std::vector<char>>	out(new std::vector<char>);
		size_t sinputLen=(size_t)intpuLen;
		size_t outputLen = sinputLen << 1;
		out->resize(outputLen, 0);
		char *pOut=out->data();
		auto ret = iconv(conv, &input, &sinputLen, &out->data(), &outputLen);
		if (ret == -1)
		{
			out = nullptr;
		}
		iconv_close(conv);
#endif // WINDOW_SYSTEM

	}
}
