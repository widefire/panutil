#pragma once

#include <string>
#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <list>
#include <string.h>
#include <condition_variable>


namespace panLogger
{
	class Log4p;
	class Log4pStream;
	enum ENUM_LOG_LEVEL
	{
		LOG_LEVEL_TRACE = 0,
		LOG_LEVEL_DEBUG,
		LOG_LEVEL_INFO,
		LOG_LEVEL_WARN,
		LOG_LEVEL_ERROR,
		LOG_LEVEL_ALARM,
		LOG_LEVEL_FATAL,
	};


	class Log4pStream {
	public:
		Log4pStream(char *buf,int size);
		Log4pStream(const Log4pStream &rt);
		~Log4pStream();
		Log4pStream&operator<<(const void*);
		Log4pStream&operator<<(const char*);
		Log4pStream&operator<<(const unsigned char*);
		Log4pStream&operator<<(const std::string);
		Log4pStream&operator<<(const bool param);
		Log4pStream&operator<<(const char param);
		Log4pStream&operator<<(const unsigned char param);
		Log4pStream&operator<<(const short param);
		Log4pStream&operator<<(const unsigned short param);
		Log4pStream&operator<<(const int param);
		Log4pStream&operator<<(const unsigned int param);
		Log4pStream&operator<<(const long long param);
		Log4pStream&operator<<(const unsigned long long param);
		Log4pStream&operator<<(const float param);
		Log4pStream&operator<<(const double param);
	private:
		void	AddBuf(const char * data,int size);
	private:
		char *_begin = nullptr;
		char *_end = nullptr;
		char *_cur = nullptr;
	};

	class Log4p {
	public:
		Log4p();
		~Log4p();
		static Log4p& Instance();
		void SetDir(std::string dir);
		void SetFileName(std::string fileName);
		void SetLogLevel(ENUM_LOG_LEVEL level);
		bool Start();
		bool Stop();
		void RecodeFileLine(bool file=true, bool line=true);
		void SetOutPut(bool toConsole=true,bool toFile=true);
		void SetLogDays(int day = 7);
		void AddLog(ENUM_LOG_LEVEL level, char* logBuf, const char *file, const int line);
	private:
		std::string GetTimeStr();
		std::string GetLogTimeFmtStr();
		void WriteLoop();
		void FlushLoop();
		void AddToSendBuf(char *data, int size);
	private:
		struct LogPacket
		{
			char *data = nullptr;
			int size = 0;
		};

		static const int s_maxBufSize;

		std::string	_dir = "log/";
		std::string _fileName = "";
		FILE	*_fp = nullptr;
		std::thread		_thWrite ;
		std::condition_variable _conWrite;
		std::list<LogPacket>	_listDataPacket;
		std::mutex				_mtxWrite;

		std::thread _thFlush ;
		std::condition_variable _conFlush;
		std::mutex				_mtxFlush;
		
		char	*_sendBuf =nullptr;
		int		_bufCur = 0;
		volatile	bool _endLog = false;

		bool _writeFile = true;
		bool _writeLine = true;
		bool _toConsole = true;
		bool _toFile = true;

		long long _beginDay = 0;
		int		_maxLogDays = 7;
		ENUM_LOG_LEVEL	_level = LOG_LEVEL_TRACE;
	};

}




#define LOG_STREAM(log,level)	\
do\
{\
char logBuf[4096];\
	panLogger::Log4pStream ss(logBuf, 4096);\
	ss << log;\
	panLogger::Log4p::Instance().AddLog(level, logBuf, __FILE__, __LINE__);\
		\
	} while (0)

#define LOGT(log)	LOG_STREAM(log, panLogger::LOG_LEVEL_TRACE)
#define LOGD(log)	LOG_STREAM(log,panLogger::LOG_LEVEL_DEBUG)
#define LOGI(log)	LOG_STREAM(log, panLogger::LOG_LEVEL_INFO)
#define LOGW(log)	LOG_STREAM(log, panLogger::LOG_LEVEL_WARN)
#define LOGE(log)	LOG_STREAM(log, panLogger::LOG_LEVEL_ERROR)
#define LOGA(log)	LOG_STREAM(log, panLogger::LOG_LEVEL_ALARM)
#define LOGF(log)	LOG_STREAM(log, panLogger::LOG_LEVEL_FATAL)
