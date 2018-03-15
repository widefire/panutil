#include "Log4P.h"

#include <ctime>
#include <iomanip>
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
#endif
namespace panLogger
{
	const int Log4p::s_maxBufSize = 4096;

	Log4pStream::Log4pStream(char * buf, int size)
	{
		memset(buf, 0, size);
		_begin = buf;
		_end = buf + size;
		_cur = buf;
	}
	Log4pStream::Log4pStream(const Log4pStream & rt)
	{
		_begin = rt._begin;
		_end = rt._end;
		_cur = rt._cur;
	}
	Log4pStream::~Log4pStream()
	{
	}
	Log4pStream & Log4pStream::operator<<(const void *param)
	{
		// TODO: 在此处插入 return 语句
		if (sizeof(param)==4)
		{
			int ptr = reinterpret_cast<int>(param);
			auto strParam = std::to_string(ptr) + " ";
			this->AddBuf(strParam.data(), (int)strParam.size());
		}
		else
		{
			auto ptr = reinterpret_cast<long long>(param);
			auto strParam = std::to_string(ptr) + " ";
			this->AddBuf(strParam.data(), (int)strParam.size());
		}
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const char *param)
	{
		// TODO: 在此处插入 return 语句
		std::string strParam = param;
		strParam += +" ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const unsigned char *param)
	{
		// TODO: 在此处插入 return 语句
		std::string strParam = (char*)param;
		strParam += " ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const std::string param)
	{
		// TODO: 在此处插入 return 语句
		this->AddBuf(param.data(), (int)param.size());
		this->AddBuf(" ", 1);
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const bool param)
	{
		// TODO: 在此处插入 return 语句
		std::string strParam = param ? "true " : "false ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const char param)
	{
		// TODO: 在此处插入 return 语句
		auto strParam = std::to_string(param) + " ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const unsigned char param)
	{
		// TODO: 在此处插入 return 语句
		auto strParam = std::to_string(param) + " ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const short param)
	{
		// TODO: 在此处插入 return 语句
		auto strParam = std::to_string(param) + " ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const unsigned short param)
	{
		// TODO: 在此处插入 return 语句
		auto strParam = std::to_string(param) + " ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const int param)
	{
		// TODO: 在此处插入 return 语句
		auto strParam = std::to_string(param) + " ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const unsigned int param)
	{
		// TODO: 在此处插入 return 语句
		auto strParam = std::to_string(param) + " ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const long long param)
	{
		// TODO: 在此处插入 return 语句
		auto strParam = std::to_string(param) + " ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const unsigned long long param)
	{
		// TODO: 在此处插入 return 语句
		auto strParam = std::to_string(param) + " ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const float param)
	{
		// TODO: 在此处插入 return 语句
		auto strParam = std::to_string(param) + " ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	Log4pStream & Log4pStream::operator<<(const double param)
	{
		// TODO: 在此处插入 return 语句
		auto strParam = std::to_string(param) + " ";
		this->AddBuf(strParam.data(), (int)strParam.size());
		return *this;
	}
	void Log4pStream::AddBuf(const char * data, int size)
	{
		if (nullptr==data||size<=0||_cur+size>=_end)
		{
			*_cur = 0;
			return;
		}
		memcpy(_cur, data, size);
		_cur += size;
		*_cur = 0;
	}
	Log4p::Log4p()
	{
		_beginDay = std::chrono::system_clock::now().time_since_epoch() / std::chrono::hours(24);
	}
	Log4p::~Log4p()
	{
		this->Stop();

	}
	Log4p & Log4p::Instance()
	{
		// TODO: 在此处插入 return 语句
		static Log4p slog;
		return slog;
	}
	void Log4p::SetDir(std::string dir)
	{
		_dir = dir;
	}
	void Log4p::SetFileName(std::string fileName)
	{
		_fileName = fileName;
	}
	void Log4p::SetLogLevel(ENUM_LOG_LEVEL level)
	{
		_level = level;
	}
	bool Log4p::Start()
	{
		if (nullptr!=_thWrite||_fp!=nullptr)
		{
			return false;
		}
		bool bRet = false;
		do
		{
			std::string fullName = _dir;
			#ifdef WINDOW_SYSTEM
				_mkdir(_dir.c_str());
			#else
				iRet = mkdir(dirName, 0777);
			#endif // WINDOW_SYSTEM
			
				fullName += this->_fileName + this->GetTimeStr() + ".log";
			_fp = fopen(fullName.c_str(), "wb");
			if (nullptr==_fp)
			{
				break;
			}

			_endLog = false;
			_thWrite = new std::thread(&Log4p::WriteLoop, this);
			if (nullptr==_thWrite)
			{
				break;
			}

			if (nullptr==_sendBuf)
			{
				_sendBuf = new char[s_maxBufSize];
				_bufCur = 0;
			}

			bRet = true;
		} while (0);
		if (!bRet)
		{
			this->Stop();
		}
		return bRet;
	}
	bool Log4p::Stop()
	{
		_endLog = true;
		if (_thWrite!=nullptr)
		{
			_conWrite.notify_one();
			if (_thWrite->joinable())
			{
				_thWrite->join();
			}
			delete _thWrite;
			_thWrite = nullptr;
		}
		if (nullptr!=_fp)
		{
			/*
			flush all
			*/
			std::lock_guard<std::mutex> guard(_mtxWrite);
			for (auto it:_listDataPacket)
			{
				this->AddToSendBuf(it.data, it.size);
			}
			_listDataPacket.clear();
			this->AddToSendBuf(nullptr, 0);
			fclose(_fp);
			_fp = nullptr;
		}


		if (nullptr != _sendBuf)
		{
			delete[]_sendBuf;
			_sendBuf = nullptr;
			_bufCur = 0;
		}

		return true;
	}

	void Log4p::RecodeFileLine(bool file, bool line)
	{
		_writeFile = file;
		_writeLine = line;
	}

	void Log4p::SetOutPut(bool toConsole, bool toFile)
	{
		_toConsole = toConsole;
		_toFile = toFile;
	}

	void Log4p::SetLogDays(int day)
	{
		_maxLogDays = day;
		if (_maxLogDays<1)
		{
			_maxLogDays = 1;
		}
	}

	void Log4p::AddLog(ENUM_LOG_LEVEL level, char* logBuf, const char * file, const int line)
	{
		if (level<_level)
		{
			return;
		}
		auto nowDay = std::chrono::system_clock::now().time_since_epoch() / std::chrono::hours(24);
		if (nowDay-_beginDay>=_maxLogDays)
		{
			_beginDay = nowDay;
			this->Stop();
			this->Start();
		}
		LogPacket pkt;
		pkt.size = s_maxBufSize;
		pkt.data = new char[s_maxBufSize];
		memset(pkt.data, 0, s_maxBufSize);

		auto strTime = this->GetLogTimeFmtStr();
		strcpy(pkt.data, strTime.data());
		
		if (_writeFile&&file)
		{
			auto begin = file + strlen(file);
			while (true)
			{
				if (*begin=='\\'||*begin=='/')
				{
					break;
				}
				begin--;
			}
			begin++;
			strcat(pkt.data, begin);
			strcat(pkt.data, ":");
		}
		
		if (_writeLine)
		{
			strcat(pkt.data, std::to_string(line).data());
			strcat(pkt.data, " ");
		}
		
		switch (level)
		{
		case panLogger::LOG_LEVEL_TRACE:
			strcat(pkt.data, "TRACE:");
			break;
		case panLogger::LOG_LEVEL_DEBUG:
			strcat(pkt.data, "DEBUG:");
			break;
		case panLogger::LOG_LEVEL_INFO:
			strcat(pkt.data, "INFO:");
			break;
		case panLogger::LOG_LEVEL_WARN:
			strcat(pkt.data, "WARN:");
			break;
		case panLogger::LOG_LEVEL_ERROR:
			strcat(pkt.data, "ERROR:");
			break;
		case panLogger::LOG_LEVEL_ALARM:
			strcat(pkt.data, "ALARM:");
			break;
		case panLogger::LOG_LEVEL_FATAL:
			strcat(pkt.data, "FATAL:");
			break;
		default:
			break;
		}

		if (logBuf!=nullptr&&strlen(logBuf)>0)
		{
			strcat(pkt.data, logBuf);
		}
		strcat(pkt.data, "\r\n");

		if (_toConsole)
		{
			std::cout << pkt.data;
		}
		if (_toFile)
		{
			std::lock_guard<std::mutex>	guard(_mtxWrite);
			_listDataPacket.push_back(pkt);
			_conWrite.notify_one();
		}
	}

	std::string Log4p::GetTimeStr()
	{
		char buf[100];
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::strftime(buf, 100, "%Y%M%d_%H%M%S_", std::localtime(&now));
		std::chrono::milliseconds;
		int mill=(int)(std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1))%1000;
		strcat(buf, std::to_string(mill).data());
		std::string strRet = buf;
		return strRet;
	}
	std::string Log4p::GetLogTimeFmtStr()
	{
		char buf[100];
		auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::strftime(buf, 100, "%Y-%m-%d %H:%M:%S:", std::localtime(&now));
		std::chrono::milliseconds;
		int mill = (int)(std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1)) % 1000;
		sprintf(buf, "%s%d ", buf, mill);
		std::string strRet = buf;
		return strRet;
	}
	void Log4p::WriteLoop()
	{
		while (!_endLog)
		{
			std::unique_lock<std::mutex> lk(_mtxWrite);
			_conWrite.wait(lk, [this] {return this->_endLog || this->_listDataPacket.size() > 0;});
			if (_endLog)
			{
				break;
			}
			auto pkt = _listDataPacket.front();
			_listDataPacket.pop_front();
			this->AddToSendBuf(pkt.data, pkt.size);
		}
	}
	void Log4p::AddToSendBuf(char * data, int size)
	{
		int strSize = data == nullptr ? 0 : strlen(data);
		if (data!=nullptr&&size>0)
		{
			if (strSize>0)
			{
				if (strSize>s_maxBufSize)
				{
					fwrite(_sendBuf, _bufCur, 1, _fp);
					_bufCur = 0;
					fwrite(data, strSize, 1, _fp);
				}
				else
				{
					if (strSize + _bufCur>=s_maxBufSize)
					{
						fwrite(_sendBuf, _bufCur, 1, _fp);
						memcpy(_sendBuf, data, strSize);
						_bufCur = strSize;
					}
					else
					{
						memcpy(_sendBuf + _bufCur, data, strSize);
						_bufCur += strSize;
					}
				}
			}
			delete[]data;
		}
		else
		{
			//flush all data
			if (_bufCur>0)
			{
				fwrite(_sendBuf, _bufCur, 1, _fp);
				_bufCur = 0;
			}
		}
	}


}
